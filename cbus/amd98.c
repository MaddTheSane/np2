#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"amd98.h"
#include	"sound.h"
#include	"fmboard.h"


static void setamd98event(BOOL absolute) {

	SINT32	cnt;

	if (pit.value[3].w > 8) {					// 根拠なし
		cnt = pc.multiple * pit.value[3].w;
	}
	else {
		cnt = pc.multiple << 16;
	}
	nevent_set(NEVENT_MUSICGEN, cnt, amd98int, absolute);
}

void amd98int(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if ((pit.mode[3] & 0x0c) == 0x04) {
			// レートジェネレータ
			setamd98event(NEVENT_RELATIVE);
		}
	}
	pic_setirq(0x0d);
	(void)item;
}


// ----

static void IOOUTCALL amd_od8(UINT port, BYTE dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL amd_od9(UINT port, BYTE dat) {

	opn.extreg = dat;
	(void)port;
}

static void IOOUTCALL amd_oda(UINT port, BYTE dat) {

	if (opn.opnreg < 0x0e) {
		psggen_setreg(&psg1, opn.opnreg, dat);
	}
	else if (opn.opnreg == 0x0f) {
		psg1.reg.io2 = dat;
	}
	(void)port;
}

static void IOOUTCALL amd_odb(UINT port, BYTE dat) {

	if (opn.extreg < 0x0e) {
		psggen_setreg(&psg2, opn.extreg, dat);
	}
	else if (opn.extreg == 0x0f) {
		BYTE b;
		b = psg2.reg.io2;
		if ((b & 1) > (dat & 1)) {
			b &= 0xc2;
			if (b == 0x42) {
//				TRACEOUT(0xfff0, psg_1.reg.io2);
				amd98.psg3reg = psg1.reg.io2;
			}
			else if (b == 0x40) {
//				TRACEOUT(0xfff1, psg_1.reg.io2);
				if (amd98.psg3reg < 0x0e) {
					psggen_setreg(&psg3, amd98.psg3reg,
												psg1.reg.io2);
				}
				else if (amd98.psg3reg == 0x0f) {
					int r;
static const BYTE amdr[] = {0x01, 0x08, 0x10, 0x20, 0x06, 0x40};
					sound_sync();
					for (r=0; r<6; r++) {
						if (psg1.reg.io2 & amdr[r]) {
//							rhythm_play(&rhythm, r, 0);
						}
					}
				}
			}
		}
		psg2.reg.io2 = dat;
	}
	(void)port;
}

static void IOOUTCALL amd_odc(UINT port, BYTE dat) {

	itimer_setcount(3, dat);
	setamd98event(NEVENT_ABSOLUTE);
	(void)port;
}

static void IOOUTCALL amd_ode(UINT port, BYTE dat) {

	itimer_setflag(3, dat);
	(void)port;
}


// ----

void amd98_bind(void) {

	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&psg2, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&psg3, (SOUNDCB)psggen_getpcm);
//	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	iocore_attachout(0xd8, amd_od8);
	iocore_attachout(0xd9, amd_od9);
	iocore_attachout(0xda, amd_oda);
	iocore_attachout(0xdb, amd_odb);
	iocore_attachout(0xdc, amd_odc);
	iocore_attachout(0xde, amd_ode);
}

