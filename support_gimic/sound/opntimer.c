/**
 * @file	opntimer.c
 * @brief	Implementation of OPN timer
 */

#include "compiler.h"
#include "opntimer.h"
#include "pccore.h"
#include "iocore.h"
#include "fmboard.h"


static const UINT8 irqtable[4] = {0x03, 0x0d, 0x0a, 0x0c};


static void set_fmtimeraevent(NEVENTPOSITION absolute) {

	SINT32	l;

	l = 18 * (1024 - g_fmtimer.timera);
	if (pccore.cpumode & CPUMODE_8MHZ) {		// 4MHz
		l = (l * 1248 / 625) * pccore.multiple;
	}
	else {										// 5MHz
		l = (l * 1536 / 625) * pccore.multiple;
	}
//	TRACEOUT(("FMTIMER-A: %08x-%d", l, absolute));
	nevent_set(NEVENT_FMTIMERA, l, fmport_a, absolute);
}

static void set_fmtimerbevent(NEVENTPOSITION absolute) {

	SINT32	l;

	l = 288 * (256 - g_fmtimer.timerb);
	if (pccore.cpumode & CPUMODE_8MHZ) {		// 4MHz
		l = (l * 1248 / 625) * pccore.multiple;
	}
	else {										// 5MHz
		l = (l * 1536 / 625) * pccore.multiple;
	}
//	TRACEOUT(("FMTIMER-B: %08x-%d", l, absolute));
	nevent_set(NEVENT_FMTIMERB, l, fmport_b, absolute);
}

void fmport_a(NEVENTITEM item) {

	BOOL	intreq = FALSE;

	if (item->flag & NEVENT_SETEVENT) {
		intreq = pcm86gen_intrq();
		if (g_fmtimer.reg & 0x04) {
			g_fmtimer.status |= 0x01;
			intreq = TRUE;
		}
		if (intreq) {
			pic_setirq(g_fmtimer.irq);
//			TRACEOUT(("fm int-A"));
		}

		set_fmtimeraevent(NEVENT_RELATIVE);

		if ((g_fmtimer.reg & 0xc0) == 0x80) {
			opngen_csm(&g_opna[0].opngen);
		}
	}
}

void fmport_b(NEVENTITEM item) {

	BOOL	intreq = FALSE;

	if (item->flag & NEVENT_SETEVENT) {
		intreq = pcm86gen_intrq();
		if (g_fmtimer.reg & 0x08) {
			g_fmtimer.status |= 0x02;
			intreq = TRUE;
		}
		if (intreq) {
			pic_setirq(g_fmtimer.irq);
//			TRACEOUT(("fm int-B"));
		}

		set_fmtimerbevent(NEVENT_RELATIVE);
	}
}

void fmtimer_reset(UINT irq) {

	memset(&g_fmtimer, 0, sizeof(g_fmtimer));
	g_fmtimer.intr = irq & 0xc0;
	g_fmtimer.intdisabel = irq & 0x10;
	g_fmtimer.irq = irqtable[irq >> 6];
//	pic_registext(g_fmtimer.irq);
}

void fmtimer_setreg(UINT reg, REG8 value) {

//	TRACEOUT(("fm %x %x [%.4x:%.4x]", reg, value, CPU_CS, CPU_IP));

	switch(reg) {
		case 0x24:
			g_fmtimer.timera = (value << 2) + (g_fmtimer.timera & 3);
			break;

		case 0x25:
			g_fmtimer.timera = (g_fmtimer.timera & 0x3fc) + (value & 3);
			break;

		case 0x26:
			g_fmtimer.timerb = value;
			break;

		case 0x27:
			g_fmtimer.reg = value;
			g_fmtimer.status &= ~((value & 0x30) >> 4);
			if (value & 0x01) {
				if (!nevent_iswork(NEVENT_FMTIMERA)) {
					set_fmtimeraevent(NEVENT_ABSOLUTE);
				}
			}
			else {
				nevent_reset(NEVENT_FMTIMERA);
			}

			if (value & 0x02) {
				if (!nevent_iswork(NEVENT_FMTIMERB)) {
					set_fmtimerbevent(NEVENT_ABSOLUTE);
				}
			}
			else {
				nevent_reset(NEVENT_FMTIMERB);
			}

			if (!(value & 0x03)) {
				pic_resetirq(g_fmtimer.irq);
			}
			break;
	}
}

