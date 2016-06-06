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

static const UINT8 s_irqtable[8] = {0xff, 0xff, 0xff, 0xff, 0x03, 0x0a, 0x0d, 0x0c};


static void IOOUTCALL pcm86_oa460(UINT port, REG8 val)
{
//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	g_pcm86.cSoundFlags = (g_pcm86.cSoundFlags & 0xfe) | (val & 1);
	fmboard_extenable((REG8)(val & 1));
	(void)port;
}

static void IOOUTCALL pcm86_oa466(UINT port, REG8 val) {

//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	if ((val & 0xe0) == 0xa0) {
		sound_sync();
		g_pcm86.vol5 = (~val) & 15;
		g_pcm86.volume = pcm86cfg.vol * g_pcm86.vol5;
	}
	(void)port;
}

static void IOOUTCALL pcm86_oa468(UINT port, REG8 val) {

	REG8	xchgbit;

//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	sound_sync();
	xchgbit = g_pcm86.cFifoCtrl ^ val;

	/* �o�b�t�@���Z�b�g */
	if ((xchgbit & 8) && (val & 8))
	{
		g_pcm86.readpos = 0;
		g_pcm86.wrtpos = 0;
		g_pcm86.realbuf = 0;
		g_pcm86.virbuf = 0;
		g_pcm86.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		g_pcm86.lastclock <<= 6;
	}

	/* ���荞�݃N���A */
	if ((xchgbit & 0x10) && (!(val & 0x10)))
	{
		g_pcm86.cIrqFlag = 0;
//		g_pcm86.write = 0;
//		g_pcm86.cReqIrq = 0;
	}

	/* �T���v�����O���[�g�ύX */
	if (xchgbit & 7) {
		g_pcm86.rescue = pcm86rescue[val & 7] << g_pcm86.cStepBits;
		pcm86_setpcmrate(val);
	}
	g_pcm86.cFifoCtrl = val;

	/* �Đ��t���O */
	if ((xchgbit & 0x80) && (val & 0x80))
	{
		g_pcm86.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		g_pcm86.lastclock <<= 6;
	}
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46a(UINT port, REG8 val) {

//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	sound_sync();
	if (g_pcm86.cFifoCtrl & 0x20)
	{
#if 1
		if (val != 0xff) {
			g_pcm86.nFifoSize = (UINT16)((val + 1) << 7);
		}
		else {
			g_pcm86.nFifoSize = 0x7ffc;
		}
#else
		if (!val) {
			val++;
		}
		g_pcm86.nFifoSize = (WORD)(val) << 7;
#endif
	}
	else
	{
		g_pcm86.cDacCtrl = val;
		g_pcm86.cStepBits = pcm86bits[(val >> 4) & 7];
		g_pcm86.nStepMask = (1 << g_pcm86.cStepBits) - 1;
		g_pcm86.rescue = pcm86rescue[g_pcm86.cFifoCtrl & 7] << g_pcm86.cStepBits;
	}
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46c(UINT port, REG8 val) {

//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	if (g_pcm86.virbuf < PCM86_LOGICALBUF) {
		g_pcm86.virbuf++;
	}
	g_pcm86.buffer[g_pcm86.wrtpos] = val;
	g_pcm86.wrtpos = (g_pcm86.wrtpos + 1) & PCM86_BUFMSK;
	g_pcm86.realbuf++;
	// �o�b�t�@�I�[�o�[�t���[�̊Ď�
	if (g_pcm86.realbuf >= PCM86_REALBUFSIZE) {
		g_pcm86.realbuf -= 4;
		g_pcm86.readpos = (g_pcm86.readpos + 4) & PCM86_BUFMSK;
	}
//	g_pcm86.write = 1;
	g_pcm86.cReqIrq = 1;
	(void)port;
}

static REG8 IOINPCALL pcm86_ia460(UINT port)
{
	(void)port;
	return g_pcm86.cSoundFlags;
}

static REG8 IOINPCALL pcm86_ia466(UINT port) {

	UINT32	past;
	UINT32	cnt;
	UINT32	stepclock;
	REG8	ret;

	past = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	past <<= 6;
	past -= g_pcm86.lastclock;
	stepclock = g_pcm86.stepclock;
	if (past >= stepclock) {
		cnt = past / stepclock;
		g_pcm86.lastclock += (cnt * stepclock);
		past -= cnt * stepclock;
		if (g_pcm86.cFifoCtrl & 0x80) {
			sound_sync();
			RECALC_NOWCLKWAIT(cnt);
		}
	}
	ret = ((past << 1) >= stepclock)?1:0;
	if (g_pcm86.virbuf >= PCM86_LOGICALBUF) {			// �o�b�t�@�t��
		ret |= 0x80;
	}
	else if (!g_pcm86.virbuf) {						// �o�b�t�@�O
		ret |= 0x40;								// ���ƕρc
	}
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, ret));
	return(ret);
}

static REG8 IOINPCALL pcm86_ia468(UINT port)
{
	REG8 ret;

	ret = g_pcm86.cFifoCtrl & (~0x10);
	if (pcm86gen_intrq())
	{
		ret |= 0x10;
	}
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, ret));
	return ret;
}

static REG8 IOINPCALL pcm86_ia46a(UINT port)
{
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, g_pcm86.cDacCtrl));
	return g_pcm86.cDacCtrl;
}

static REG8 IOINPCALL pcm86_inpdummy(UINT port) {

	(void)port;
	return(0);
}


// ----

/**
 * Reset
 * @param[in] cDipSw Dip switch
 */
void pcm86io_setopt(REG8 cDipSw)
{
	g_pcm86.cSoundFlags = ((~cDipSw) >> 1) & 0x70;
	g_pcm86.cIrqLevel = s_irqtable[(cDipSw >> 2) & 7];
}

void pcm86io_bind(void) {

	sound_streamregist(&g_pcm86, (SOUNDCB)pcm86gen_getpcm);

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

