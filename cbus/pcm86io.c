#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pcm86io.h"
#include	"sound.h"
#include	"fmboard.h"


extern	PCM86CFG	pcm86cfg;


static const UINT8 pcm86bits[] = {1, 1, 1, 2, 0, 0, 0, 1};
static const SINT32 pcm86rescue[] = {PCM86_RESCUE * 32, PCM86_RESCUE * 24,
									 PCM86_RESCUE * 16, PCM86_RESCUE * 12,
									 PCM86_RESCUE *  8, PCM86_RESCUE *  6,
									 PCM86_RESCUE *  4, PCM86_RESCUE *  3};


static void IOOUTCALL pcm86_oa460(UINT port, REG8 val) {

	pcm86.extfunc = val;
	fmboard_extenable((REG8)(val & 1));
	(void)port;
}

static void IOOUTCALL pcm86_oa466(UINT port, REG8 val) {

	if ((val & 0xe0) == 0xa0) {
		sound_sync();
		pcm86.vol5 = (~val) & 15;
		pcm86.volume = pcm86cfg.vol * pcm86.vol5;
	}
	(void)port;
}

static void IOOUTCALL pcm86_oa468(UINT port, REG8 val) {

	REG8	xchgbit;

	sound_sync();
	xchgbit = pcm86.fifo ^ val;
	// �o�b�t�@���Z�b�g����
	if ((xchgbit & 8) && (val & 8)) {
		pcm86.readpos = 0;				// �o�b�t�@���Z�b�g
		pcm86.wrtpos = 0;
		pcm86.realbuf = 0;
		pcm86.virbuf = 0;
		pcm86.write = 0;
		pcm86.reqirq = 0;
		pcm86.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		pcm86.lastclock <<= 6;
	}
	// �T���v�����O���[�g�ύX
	if (xchgbit & 7) {
		pcm86.rescue = pcm86rescue[val & 7] << pcm86.stepbit;
		pcm86_setpcmrate(val);
	}
	pcm86.fifo = val & (~0x10);
	if ((xchgbit & 0x80) && (val & 0x80)) {
		pcm86.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		pcm86.lastclock <<= 6;
	}
	pcm86.write = 1;
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46a(UINT port, REG8 val) {

	sound_sync();
	if (pcm86.fifo & 0x20) {
#if 1
		if (val != 0xff) {
			pcm86.fifosize = (UINT16)((val + 1) << 7);
		}
		else {
			pcm86.fifosize = 0x7ffc;
		}
#else
		if (!val) {
			val++;
		}
		pcm86.fifosize = (WORD)(val) << 7;
#endif
	}
	else {
		pcm86.dactrl = val;
		pcm86.stepbit = pcm86bits[(val >> 4) & 7];
		pcm86.stepmask = (1 << pcm86.stepbit) - 1;
		pcm86.rescue = pcm86rescue[pcm86.fifo & 7] << pcm86.stepbit;
	}
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46c(UINT port, REG8 val) {

	if (pcm86.virbuf < PCM86_LOGICALBUF) {
		pcm86.virbuf++;
		pcm86.buffer[pcm86.wrtpos] = val;
		pcm86.wrtpos = (pcm86.wrtpos + 1) & PCM86_BUFMSK;
		pcm86.realbuf++;
		// �o�b�t�@�I�[�o�[�t���[�̊Ď�
		if (pcm86.realbuf >= PCM86_REALBUFSIZE) {
			pcm86.realbuf &= 3;				// align4���߃E�`
			pcm86.realbuf += PCM86_REALBUFSIZE - 4;
		}
		pcm86.write = 1;
		pcm86.reqirq = 1;
	}
	(void)port;
}

static REG8 IOINPCALL pcm86_ia460(UINT port) {

	(void)port;
	return(0x40 | (pcm86.extfunc & 1));
}

static REG8 IOINPCALL pcm86_ia466(UINT port) {

	UINT32	nowclk;
	UINT32	past;
	REG8	ret;

	// ����Ȃ�ł���������Ȃ�@�����Ə��������܂��傤�ˁ@�R��c
	if (!(pcm86.fifo & 0x80)) {
		ret = 0;
	}
	else {
		sound_sync();
		nowclk = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		nowclk <<= 6;
		past = nowclk - pcm86.lastclock;
		if (past >= pcm86.stepclock) {
			RECALC_NOWCLKWAIT;
			past = nowclk - pcm86.lastclock;
		}
		ret = ((past << 1) >= pcm86.stepclock)?1:0;
	}
	if (pcm86.virbuf >= PCM86_LOGICALBUF) {			// �o�b�t�@�t��
		ret |= 0x80;
	}
	else if (!pcm86.virbuf) {						// �o�b�t�@�O
		ret |= 0x40;								// ���ƕρc
	}
	(void)port;
	return(ret);
}

static REG8 IOINPCALL pcm86_ia468(UINT port) {

	REG8	ret;

	ret = pcm86.fifo & (~0x10);
	if ((pcm86.write) && (pcm86.fifo & 0x20)) {
//		pcm86.write = 0;
		sound_sync();
		if (pcm86.virbuf <= pcm86.fifosize) {
			pcm86.write = 0;
			ret |= 0x10;
		}
	}
	(void)port;
	return(ret);
}

static REG8 IOINPCALL pcm86_ia46a(UINT port) {

	(void)port;
	return(pcm86.dactrl);
}

static REG8 IOINPCALL pcm86_inpdummy(UINT port) {

	(void)port;
	return(0);
}


// ----

void pcm86io_bind(void) {

	sound_streamregist(&pcm86, (SOUNDCB)pcm86gen_getpcm);

	iocore_attachout(0xa460, pcm86_oa460);
	iocore_attachout(0xa466, pcm86_oa466);
	iocore_attachout(0xa468, pcm86_oa468);
	iocore_attachout(0xa46a, pcm86_oa46a);
	iocore_attachout(0xa46c, pcm86_oa46c);

	iocore_attachinp(0xa460, pcm86_ia460);
	iocore_attachinp(0xa462, pcm86_inpdummy);
	iocore_attachinp(0xa464, pcm86_inpdummy);
	iocore_attachinp(0xa466, pcm86_ia466);
	iocore_attachinp(0xa468, pcm86_ia468);
	iocore_attachinp(0xa46a, pcm86_ia46a);
	iocore_attachinp(0xa46c, pcm86_inpdummy);
	iocore_attachinp(0xa46e, pcm86_inpdummy);
}

