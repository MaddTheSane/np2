#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"


void artic_callback(void) {

	SINT32	mul;
	SINT32	leng;

	mul = pc.multiple;
	if (pc.baseclock == PCBASECLOCK25) {
		mul *= 16;
	}
	else {
		mul *= 13;
	}
	leng = I286_CLOCK + I286_BASECLOCK + I286_REMCLOCK;
	leng *= 2;
	leng -= artic.lastclk2;
	if (leng > 0) {
		leng /= mul;
		artic.counter += leng;
		artic.lastclk2 += leng * mul;
	}
}

static UINT32 artic_getcnt(void) {

	SINT32	mul;
	SINT32	leng;

	mul = pc.multiple;
	if (pc.baseclock != PCBASECLOCK20) {
		mul *= 16;
	}
	else {
		mul *= 13;
	}
	leng = I286_CLOCK + I286_BASECLOCK + I286_REMCLOCK;
	leng *= 2;
	leng -= artic.lastclk2;
	if (leng > 0) {
		leng /= mul;
		return(artic.counter + leng);
	}
	return(artic.counter);
}


// ---- I/O

static void IOOUTCALL artic_o5c(UINT port, BYTE dat) {

	(void)port;
	(void)dat;
	I286_REMCLOCK -= 20;
}

static BYTE IOINPCALL artic_i5c(UINT port) {

	(void)port;
	return((BYTE)artic_getcnt());
}

static BYTE IOINPCALL artic_i5d(UINT port) {

	(void)port;
	return((BYTE)(artic_getcnt() >> 8));
}

static BYTE IOINPCALL artic_i5f(UINT port) {

	(void)port;
	return((BYTE)(artic_getcnt() >> 16));
}


// ---- I/F

void artic_reset(void) {

	ZeroMemory(&artic, sizeof(artic));
}

void artic_bind(void) {

	iocore_attachout(0x005c, artic_o5c);
	iocore_attachinp(0x005c, artic_i5c);
	iocore_attachinp(0x005d, artic_i5d);
	iocore_attachinp(0x005e, artic_i5d);
	iocore_attachinp(0x005f, artic_i5f);
}

UINT16 IOINPCALL artic_r16(UINT port) {

	UINT32	cnt;

	cnt = artic_getcnt();
	if (port & 2) {
		cnt >>= 8;
	}
	return((UINT16)cnt);
}

