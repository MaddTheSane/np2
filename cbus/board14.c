#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board14.h"
#include	"sound.h"
#include	"fmboard.h"


// ---- intr

static void setmusicgenevent(BOOL absolute) {

	SINT32	cnt;

	if (pit.value[3].w > 8) {						// 根拠なし
		cnt = pc.multiple * pit.value[3].w;
	}
	else {
		cnt = pc.multiple << 16;
	}
	nevent_set(NEVENT_MUSICGEN, cnt, musicgenint, absolute);
}

void musicgenint(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[3] & 0x0c) == 0x04) {
			// レートジェネレータ
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

	itimer_setcount(3, dat);
	setmusicgenevent(NEVENT_ABSOLUTE);
	(void)port;
}

static void IOOUTCALL musicgen_o18e(UINT port, BYTE dat) {

	itimer_setflag(3, dat);
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
	return(itimer_getcount(3));
}

static BYTE IOINPCALL musicgen_i18e(UINT port) {

	(void)port;
	return(0x40);					// INT-5
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

