// 
// μPD8253C タイマLSI
// 

#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"beep.h"


#define	BEEPCOUNTEREX					// BEEPアイドル時のカウンタをα倍に


// --- Interval timer

static void setsystimerevent(BOOL absolute) {

	SINT32	cnt;

	cnt = pit.value[0];
	if (cnt > 8) {									// 根拠なし
		cnt *= pccore.multiple;
	}
	else {
		cnt = pccore.multiple << 16;
	}
	nevent_set(NEVENT_ITIMER, cnt, systimer, absolute);
}

void systimer(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if (pit.intr[0]) {
			pit.intr[0] = 0;
			pic_setirq(0);
		}
		if ((pit.mode[0] & 0x0c) == 0x04) {
			// レートジェネレータ
			pit.intr[0] = 1;
			setsystimerevent(NEVENT_RELATIVE);
		}
		else {
			nevent_set(NEVENT_ITIMER, pccore.multiple << 16,
												systimer, NEVENT_RELATIVE);
		}
	}
}


// --- Beep

#if defined(BEEPCOUNTEREX)
static void setbeepeventex(BOOL absolute) {

	UINT32	cnt;

	cnt = pit.value[1];
	if (cnt > 2) {
		cnt *= pccore.multiple;
	}
	else {
		cnt = pccore.multiple << 16;
	}
	while(cnt < 0x100000) {
		cnt <<= 1;
	}
	nevent_set(NEVENT_BEEP, (SINT32)cnt, beeponeshot, absolute);
}
#endif

static void setbeepevent(BOOL absolute) {

	SINT32	cnt;

	cnt = pit.value[1];
	if (cnt > 2) {
		cnt *= pccore.multiple;
	}
	else {
		cnt = pccore.multiple << 16;
	}
	nevent_set(NEVENT_BEEP, cnt, beeponeshot, absolute);
}

void beeponeshot(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if (!(pit.mode[1] & 0x0c)) {								// ver0.30
			beep_lheventset(0);
		}
#if defined(uPD71054)
		if ((pit.mode[1] & 0x06) == 0x02)
#else
		if (pit.mode[1] & 0x02)
#endif
		{
#if defined(BEEPCOUNTEREX)
			setbeepeventex(NEVENT_RELATIVE);
#else
			setbeepevent(NEVENT_RELATIVE);
#endif
		}
	}
}


// --- RS-232C

static void setrs232cevent(BOOL absolute) {

	SINT32	cnt;

	if (pit.value[2] > 1) {
		cnt = pccore.multiple * pit.value[2] * rs232c.mul;
	}
	else {
		cnt = (pccore.multiple << 16) * rs232c.mul;
	}
	nevent_set(NEVENT_RS232C, cnt, rs232ctimer, absolute);
}

void rs232ctimer(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[2] & 0x0c) == 0x04) {
			// レートジェネレータ
			setrs232cevent(NEVENT_RELATIVE);
		}
	}
	rs232c_callback();
}


// ---------------------------------------------------------------------------

static UINT pit_latch(int ch) {

	SINT32	clock;

	if (ch == 1) {
		switch(pit.mode[1] & 0x06) {
#ifdef uPD71054				// ?
			case 0x00:
#endif
			case 0x04:
				return(pit.value[1]);
#ifdef uPD71054
			case 0x06:
				return(pit.value[1] & (~1));
#endif
		}
#if defined(BEEPCOUNTEREX)
		clock = nevent_getremain(NEVENT_ITIMER + ch);
		if (clock < 0) {
			return(0);
		}
		clock /= pccore.multiple;
		if (pit.value[1] > 2) {
			clock %= pit.value[1];
		}
		else {
			clock = LOW16(clock);
		}
		return(clock);
#endif
	}
	clock = nevent_getremain(NEVENT_ITIMER + ch);
	if (clock >= 0) {
		return(clock / pccore.multiple);
	}
	return(0);
}

void pit_setflag(int ch, REG8 value) {

	pit.flag[ch] = 0;
	if (value & 0x30) {
		pit.mode[ch] = (UINT8)value;
	}
	else {														// latch
		pit.mode[ch] &= ~0x30;
		pit.latch[ch] = (UINT16)pit_latch(ch);
	}
}

BOOL pit_setcount(int ch, REG8 value) {

	switch(pit.mode[ch] & 0x30) {
		case 0x10:		// access low
			pit.value[ch] = value;
			break;

		case 0x20:		// access high
			pit.value[ch] = value << 8;
			break;

		case 0x30:		// access word
			if (!(pit.flag[ch] & 2)) {
				pit.value[ch] &= 0xff00;
				pit.value[ch] += value;
				pit.flag[ch] ^= 2;
				return(TRUE);
			}
			pit.value[ch] &= 0x00ff;
			pit.value[ch] += value << 8;
			pit.flag[ch] ^= 2;
			break;
	}
	return(FALSE);
}

REG8 pit_getcount(int ch) {

	REG8	ret;
	REG16	w;

	if (!(pit.mode[ch] & 0x30)) {
		w = pit.latch[ch];
	}
	else {
		w = pit_latch(ch);
	}
	switch(pit.mode[ch] & 0x30) {
		case 0x10:						// access low
			return((UINT8)w);

		case 0x20:						// access high
			return((UINT8)(w >> 8));
	}
	if (!(pit.flag[ch] & 1)) {			// access word
		ret = (UINT8)w;
	}
	else {
		ret = (UINT8)(w >> 8);
	}
	pit.flag[ch] ^= 1;
	return(ret);
}


// ---- I/O

// system timer
static void IOOUTCALL pit_o71(UINT port, REG8 dat) {

//	TRACEOUT(("pic o71: %x", dat));
	if (pit_setcount(0, dat)) {
		return;
	}
	pic.pi[0].irr &= (~1);
	pit.intr[0] = 1;
	setsystimerevent(NEVENT_ABSOLUTE);
	(void)port;
}

// beep
static void IOOUTCALL pit_o73(UINT port, REG8 dat) {

	if (pit_setcount(1, dat)) {
		return;
	}
	setbeepevent(NEVENT_ABSOLUTE);
	if (!(pit.mode[1] & 0x0c)) {
		beep_lheventset(1);
	}
	else {
		beep_hzset(pit.value[1]);
	}
	(void)port;
}

// rs-232c
static void IOOUTCALL pit_o75(UINT port, REG8 dat) {

	if (pit_setcount(2, dat)) {
		return;
	}
	rs232c_open();
	setrs232cevent(NEVENT_ABSOLUTE);
	(void)port;
}

// ctrl
static void IOOUTCALL pit_o77(UINT port, REG8 dat) {

	int		ch;

//	TRACEOUT(("pic o77: %x", dat));
	ch = (dat >> 6) & 3;
	if (ch != 3) {
		pit_setflag(ch, dat);
		if (ch == 0) {			// 書込みで itimerのirrがリセットされる…
			pic.pi[0].irr &= (~1);
			if (dat & 0x30) {	// 一応ラッチ時は割り込みをセットしない
				pit.intr[0] = 1;
//				setsystimerevent(NEVENT_ABSOLUTE);
			}
		}
		if (ch == 1) {
			beep_modeset();
		}
	}
#if defined(uPD71054)
	else {
		TRACEOUT(("multiple latch commands - %x", dat));
		for (ch=0; ch<3; ch++) {
			if (dat & (2 << ch)) {
				if (!(dat & 0x10)) {
				}
				if (!(dat & 0x20)) {
				}
			}
		}
	}
#endif
	(void)port;
}

static REG8 IOINPCALL pit_i71(UINT port) {

	int		ch;

	ch = (port >> 1) & 3;
#if defined(uPD71054)
	if (pit.stat[ch]) {
		REG8 ret;
		ret = pit.stat[ch];
		pit.stat[ch] = 0;
		TRACEOUT(("stat out -> %d-%x", ch, ret));
		return(ret);
	}
#endif
	return(pit_getcount(ch));
}


// ---- I/F

static const IOOUT pito71[4] = {
					pit_o71,	pit_o73,	pit_o75,	pit_o77};

static const IOINP piti71[4] = {
					pit_i71,	pit_i71,	pit_i71,	NULL};

void itimer_reset(void) {

	ZeroMemory(&pit, sizeof(pit));
	if (pccore.cpumode & CPUMODE_8MHZ) {
		pit.value[1] = 998;				// 4MHz
	}
	else {
		pit.value[1] = 1229;			// 5MHz
	}
	pit.intr[0] = 1;
	pit.mode[0] = 0x30;
	pit.mode[1] = 0x56;
	pit.mode[2] = 0xb6;
	pit.mode[3] = 0x36;
	setsystimerevent(NEVENT_ABSOLUTE);
	beep_hzset(pit.value[1]);
}

void itimer_bind(void) {

	iocore_attachsysoutex(0x0071, 0x0cf1, pito71, 4);
	iocore_attachsysinpex(0x0071, 0x0cf1, piti71, 4);
	iocore_attachout(0x3fd9, pit_o71);
	iocore_attachout(0x3fdb, pit_o73);
	iocore_attachout(0x3fdd, pit_o75);
	iocore_attachout(0x3fdf, pit_o77);
	iocore_attachinp(0x3fd9, pit_i71);
	iocore_attachinp(0x3fdb, pit_i71);
	iocore_attachinp(0x3fdd, pit_i71);
}

