// 
// ��PD8253C �^�C�}LSI
// 

#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"beep.h"


// #define	uPD71054								// NP2��uPD8253C�x�[�X


// --- Interval timer

// ver0.31 ��ɉ񂷁c
static void setsystimerevent_noint(BOOL absolute) {

	nevent_set(NEVENT_ITIMER, pc.multiple << 16, systimer_noint, absolute);
}

void systimer_noint(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		setsystimerevent_noint(NEVENT_RELATIVE);
	}
}

static void setsystimerevent(BOOL absolute) {

	SINT32	cnt;

	cnt = pit.value[0].w;
	if (cnt > 8) {									// �����Ȃ�
		cnt *= pc.multiple;
	}
	else {
		cnt = pc.multiple << 16;
	}
	nevent_set(NEVENT_ITIMER, cnt, systimer, absolute);
}

void systimer(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[0] & 0x0c) == 0x04) {
			// ���[�g�W�F�l���[�^
			setsystimerevent(NEVENT_RELATIVE);
		}
		else {
			setsystimerevent_noint(NEVENT_RELATIVE);
		}
	}
	pic_setirq(0);
}


// --- Beep

static void setbeepevent(BOOL absolute) {

	SINT32	cnt;

	cnt = pit.value[1].w;
	if (cnt > 2) {
		cnt *= pc.multiple;
	}
	else {
		cnt = pc.multiple << 16;
	}
	nevent_set(NEVENT_BEEP, cnt, beeponeshot, absolute);
}

void beeponeshot(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if (!(pit.mode[1] & 0x0c)) {								// ver0.30
			beep_lheventset(0);
		}
#ifdef uPD71054
		if ((pit.mode[1] & 0x06) == 0x02)
#else
		if (pit.mode[1] & 0x02)
#endif
		{
			setbeepevent(NEVENT_RELATIVE);
		}
	}
}


// --- RS-232C

static void setrs232cevent(BOOL absolute) {

	SINT32	cnt;

	if (pit.value[2].w > 1) {
		cnt = pc.multiple * pit.value[2].w * rs232c.mul;			// ver0.29
	}
	else {
		cnt = (pc.multiple << 16) * rs232c.mul;						// ver0.29
	}
	nevent_set(NEVENT_RS232C, cnt, rs232ctimer, absolute);
}

void rs232ctimer(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[2] & 0x0c) == 0x04) {
			// ���[�g�W�F�l���[�^
			setrs232cevent(NEVENT_RELATIVE);
		}
	}
	rs232c_callback();
}

// ---------------------------------------------------------------------------

static UINT16 itimer_latch(int ch) {

	SINT32	clock;

	if (ch == 1) {
		switch(pit.mode[1] & 0x06) {
			case 0x00:
			case 0x04:
				return(pit.value[1].w);
#ifdef uPD71054
			case 0x06:
				return(pit.value[1].w & 0xfffe);
#endif
		}
	}
	clock = nevent_getremain(NEVENT_ITIMER + ch);
	if (clock >= 0) {
		return((UINT16)(clock / pc.multiple));
	}
	return(0);
}


void itimer_setflag(int ch, BYTE value) {

	pit.flag[ch] = 0;
	if (value & 0x30) {
		pit.mode[ch] = value;
	}
	else {														// latch
		pit.mode[ch] &= ~0x30;
		pit.latch[ch].w = itimer_latch(ch);
	}
}

BOOL itimer_setcount(int ch, BYTE value) {

	switch(pit.mode[ch] & 0x30) {
		case 0x10:		// access low
			pit.value[ch].w = value;
			break;

		case 0x20:		// access high
			pit.value[ch].w = value << 8;
			break;

		case 0x30:		// access word
			if (!(pit.flag[ch] & 2)) {
				pit.value[ch].w &= 0xff00;
				pit.value[ch].w += value;
				pit.flag[ch] ^= 2;
				return(TRUE);
			}
			pit.value[ch].w &= 0x00ff;
			pit.value[ch].w += value << 8;
			pit.flag[ch] ^= 2;
			break;
	}
	return(FALSE);
}

BYTE itimer_getcount(int ch) {

	BYTE	ret;
	union {
		BYTE	b[2];
		UINT16	w;
	} tim;

	if (!(pit.mode[ch] & 0x30)) {
		tim.w = pit.latch[ch].w;
	}
	else {
		tim.w = itimer_latch(ch);									// ver0.30
	}
	switch(pit.mode[ch] & 0x30) {
		case 0x10:						// access low
			return((BYTE)tim.w);

		case 0x20:						// access high
			return((BYTE)(tim.w >> 8));
	}
										// access word
	if (!(pit.flag[ch] & 1)) {
		ret = (BYTE)tim.w;
	}
	else {
		ret = (BYTE)(tim.w >> 8);
	}
	pit.flag[ch] ^= 1;
	return(ret);
}


// ---- I/O

// system timer
static void IOOUTCALL pit_o71(UINT port, BYTE dat) {

	if (itimer_setcount(0, dat)) {
		return;
	}
	pic.pi[0].irr &= (~1);
	setsystimerevent(NEVENT_ABSOLUTE);
	(void)port;
}

// beep
static void IOOUTCALL pit_o73(UINT port, BYTE dat) {

	if (itimer_setcount(1, dat)) {
		return;
	}
	setbeepevent(NEVENT_ABSOLUTE);
	if (!(pit.mode[1] & 0x0c)) {
		beep_lheventset(1);
	}
	else {
		beep_hzset(pit.value[1].w);
	}
	(void)port;
}

// rs-232c
static void IOOUTCALL pit_o75(UINT port, BYTE dat) {

	if (itimer_setcount(2, dat)) {
		return;
	}
	rs232c_open();
	setrs232cevent(NEVENT_ABSOLUTE);
	(void)port;
}

// ctrl
static void IOOUTCALL pit_o77(UINT port, BYTE dat) {

	int		ch;

	ch = (dat >> 6) & 3;
	if (ch != 3) {
		itimer_setflag(ch, dat);
		if (ch == 0) {			// �����݂� itimer��irr�����Z�b�g�����c
			pic.pi[0].irr &= (~1);
		}
		if (ch == 1) {
			beep_modeset();
		}
	}
	(void)port;
}

static BYTE IOINPCALL pit_i71(UINT port) {

	return(itimer_getcount((port >> 1) & 3));
}


// ---- I/F

static const IOOUT pito71[4] = {
					pit_o71,	pit_o73,	pit_o75,	pit_o77};

static const IOINP piti71[4] = {
					pit_i71,	pit_i71,	pit_i71,	NULL};

void itimer_reset(void) {

	ZeroMemory(&pit, sizeof(pit));
	if (pc.cpumode & CPUMODE_8MHz) {
		pit.value[1].w = 998;			// 4MHz
	}
	else {
		pit.value[1].w = 1229;			// 5MHz
	}
	pit.mode[0] = 0x30;
	pit.mode[1] = 0x56;
	pit.mode[2] = 0xb6;
	pit.mode[3] = 0x36;
	setsystimerevent(NEVENT_ABSOLUTE);
	beep_hzset(pit.value[1].w);
}

void itimer_bind(void) {

	iocore_attachsysoutex(0x0071, 0x0cf1, pito71, 4);
	iocore_attachsysinpex(0x0071, 0x0cf1, piti71, 4);
	iocore_attachout(0x3fdb, pit_o73);
	iocore_attachout(0x3fdf, pit_o77);
	iocore_attachinp(0x3fdb, pit_i71);
}

