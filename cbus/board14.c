#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board14.h"
#include	"sound.h"
#include	"fmboard.h"


// �ǂ��� 8253C-2�� 4MHz/16�炷���H
// �Ƃ肠���� 1996800/8����͂��Ă݂�... (ver0.71)


// ---- 8253C-2

static UINT16 pit3_latch(void) {

	SINT32	clock;

	clock = nevent_getremain(NEVENT_MUSICGEN);
	if (clock >= 0) {
		clock /= pc.multiple;
		clock /= 8;
		if (pc.baseclock == PCBASECLOCK25) {
			clock = clock * 13 / 16;
		}
		return((UINT16)clock);
	}
	return(0);
}

static void pit3_setflag(BYTE value) {

	pit.flag[3] = 0;
	if (value & 0x30) {
		pit.mode[3] = value;
	}
	else {
		pit.mode[3] &= ~0x30;
		pit.latch[3] = pit3_latch();
	}
}

static BOOL pit3_setcount(BYTE value) {

	switch(pit.mode[3] & 0x30) {
		case 0x10:		// access low
			pit.value[3] = value;
			break;

		case 0x20:		// access high
			pit.value[3] = value << 8;
			break;

		case 0x30:		// access word
			if (!(pit.flag[3] & 2)) {
				pit.value[3] &= 0xff00;
				pit.value[3] += value;
				pit.flag[3] ^= 2;
				return(TRUE);
			}
			pit.value[3] &= 0x00ff;
			pit.value[3] += value << 8;
			pit.flag[3] ^= 2;
			break;
	}
	return(FALSE);
}

static BYTE pit3_getcount(void) {

	BYTE	ret;
	UINT16	w;

	if (!(pit.mode[3] & 0x30)) {
		w = pit.latch[3];
	}
	else {
		w = pit3_latch();
	}
	switch(pit.mode[3] & 0x30) {
		case 0x10:						// access low
			return((BYTE)w);

		case 0x20:						// access high
			return((BYTE)(w >> 8));
	}
										// access word
	if (!(pit.flag[3] & 1)) {
		ret = (BYTE)w;
	}
	else {
		ret = (BYTE)(w >> 8);
	}
	pit.flag[3] ^= 1;
	return(ret);
}


// ---- intr

static void setmusicgenevent(BOOL absolute) {

	SINT32	cnt;

	if (pit.value[3] > 4) {						// �����Ȃ�
		cnt = pc.multiple * pit.value[3];
	}
	else {
		cnt = pc.multiple << 16;
	}
	if (pc.baseclock == PCBASECLOCK25) {
		cnt = cnt * 16 / 13;					// cnt * 2457600 / 1996800
	}
	cnt *= 8;
	nevent_set(NEVENT_MUSICGEN, cnt, musicgenint, absolute);
}

void musicgenint(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[3] & 0x0c) == 0x04) {
			// ���[�g�W�F�l���[�^
			setmusicgenevent(NEVENT_RELATIVE);
		}
	}
	pic_setirq(0x0c);
	(void)item;
}


// ---- I/O

static void IOOUTCALL musicgen_o088(UINT port, BYTE dat) {

	musicgen.porta = dat;
	(void)port;
}

static void IOOUTCALL musicgen_o08a(UINT port, BYTE dat) {

	musicgen.portb = dat;
	(void)port;
}

static void IOOUTCALL musicgen_o08c(UINT port, BYTE dat) {

	if (dat & 0x80) {
		if (!(musicgen.portc & 0x80)) {
			musicgen.sync = 1;
			musicgen.ch = 0;
		}
		else if (musicgen.sync) {
			musicgen.sync = 0;
			sound_sync();
			musicgen.key[musicgen.ch] = dat;
			tms3631_setkey(&tms3631, (BYTE)musicgen.ch, dat);
		}
		else if ((!(dat & 0x40)) && (musicgen.portc & 0x40)) {
			musicgen.sync = 1;
			musicgen.ch = (musicgen.ch + 1) & 7;
		}
	}
	musicgen.portc = dat;
	(void)port;
}

static void IOOUTCALL musicgen_o188(UINT port, BYTE dat) {

	sound_sync();
	musicgen.mask = dat;
	tms3631_setenable(&tms3631, dat);
	(void)port;
}

static void IOOUTCALL musicgen_o18c(UINT port, BYTE dat) {

	if (!pit3_setcount(dat)) {
		setmusicgenevent(NEVENT_ABSOLUTE);
	}
	(void)port;
}

static void IOOUTCALL musicgen_o18e(UINT port, BYTE dat) {

	pit3_setflag(dat);
	(void)port;
}

static BYTE IOINPCALL musicgen_i088(UINT port) {

	(void)port;
	return(musicgen.porta);
}

static BYTE IOINPCALL musicgen_i08a(UINT port) {

	(void)port;
	return(musicgen.portb);
}

static BYTE IOINPCALL musicgen_i08c(UINT port) {

	(void)port;
	return(musicgen.portc);
}

static BYTE IOINPCALL musicgen_i08e(UINT port) {

	(void)port;
	return(0x08);
}

static BYTE IOINPCALL musicgen_i188(UINT port) {

	(void)port;
	return(musicgen.mask);
}

static BYTE IOINPCALL musicgen_i18c(UINT port) {

	(void)port;
	return(pit3_getcount());
}

static BYTE IOINPCALL musicgen_i18e(UINT port) {

	(void)port;
#if 1
	return(0x80);					// INT-5
#else
	return(0x40);					// INT-5
#endif
}


// ----

static const IOOUT musicgen_o0[4] = {
		musicgen_o088,	musicgen_o08a,	musicgen_o08c,	NULL};

static const IOOUT musicgen_o1[4] = {
		musicgen_o188,	musicgen_o188,	musicgen_o18c,	musicgen_o18e};

static const IOINP musicgen_i0[4] = {
		musicgen_i088,	musicgen_i08a,	musicgen_i08c,	musicgen_i08e};

static const IOINP musicgen_i1[4] = {
		musicgen_i188,	musicgen_i188,	musicgen_i18c,	musicgen_i18e};


void board14_reset(void) {

	ZeroMemory(&musicgen, sizeof(musicgen));
	soundrom_load(0xcc000, "14");
}

void board14_bind(void) {

	sound_streamregist(&tms3631, (SOUNDCB)tms3631_getpcm);
	cbuscore_attachsndex(0x088, musicgen_o0, musicgen_i0);
	cbuscore_attachsndex(0x188, musicgen_o1, musicgen_i1);
}

void board14_allkeymake(void) {

	int		i;

	for (i=0; i<8; i++) {
		tms3631_setkey(&tms3631, (BYTE)i, musicgen.key[i]);
	}
}

