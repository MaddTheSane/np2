#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


static const _PICITEM def_master = {
							0, {0, 0, 0, 0, 0, 0, 0, 0},
								{7, 6, 5, 4, 3, 2, 1, 0},
								{0, 0x08, 0x00, 0},
								0x7d, 0, 0, 0,
								0, 0, 0, 0};

static const _PICITEM def_slave = {
							0, {0, 0, 0, 0, 0, 0, 0, 0},
								{7, 6, 5, 4, 3, 2, 1, 0},
								{0, 0x10, 0x07, 0},
								0x71, 0, 0, 0,
								0, 0, 0, 0};


// ----

static void pic_rolpry(PICITEM pi) {

	// あははーっ　酷いね…こりゃ
	*(UINT32 *)(&pi->pry[0]) =
			(*(UINT32 *)(&pi->pry[0]) + 0x01010101) & 0x07070707;
	*(UINT32 *)(&pi->pry[4]) =
			(*(UINT32 *)(&pi->pry[4]) + 0x01010101) & 0x07070707;
}


static void pic_downbylevel(PICITEM picp, UINT level) {

	int		i;

	for (i=0; i<8; i++) {
		if (picp->pry[i] < picp->pry[level]) {
			(picp->pry[i])++;
		}
	}
	picp->pry[level] = 0;
}


// eoi処理
static void pic_forceeoibylevel(PICITEM picp, UINT level) {

	int		i;

	if (picp->isr & (1 << level)) {
		picp->isr &= ~(1 << level);
		picp->levels--;
		for (i=0; (i<picp->levels) && (picp->level[i] != level); i++) { }
		for (; i<picp->levels; i++) {
			picp->level[i] = picp->level[i+1];
		}
	}
}


void pic_irq(void) {

	int		i;
	BYTE	bit;
	SINT8	pry;
	BYTE	irq;
	BYTE	sirq;
	BYTE	targetbit;
	PIC		p;

	p = &pic;

	// 割込み許可で　要求あり？
	if ((CPU_isEI) &&
			((p->pi[0].irr & (~p->pi[0].imr)) ||
			(p->pi[1].irr & (~p->pi[1].imr)))) {

		// マスターの処理
		if (!p->pi[0].levels) {
			pry = -1;
		}
		else {
			pry = (SINT8)p->pi[0].pry[p->pi[0].level[p->pi[0].levels - 1]];
		}
		irq = 0xff;
		targetbit = 0;
		for (bit=1, i=0; i<8; bit<<=1, i++) {
			if ((p->pi[0].irr & bit) &&
				(!((p->pi[0].imr | p->pi[0].isr) & bit))) {
				if ((SINT8)p->pi[0].pry[i] > pry) {
					pry = p->pi[0].pry[i];
					irq = (BYTE)i;
					targetbit = bit;
				}
			}
		}
		// スレーヴの要求はある？
		if (!(p->pi[1].irr & (~p->pi[1].imr))) {
			sirq = 0xff;
		}
		else {
			sirq = p->pi[1].icw[2] & 7;
			bit = 1 << sirq;
			if (!((p->pi[0].imr | p->pi[0].isr) & bit)) {
				if ((SINT8)p->pi[0].pry[sirq] > pry) {
					irq = sirq;
					targetbit = bit;
				}
			}
		}
		// マスタの割込
		if (irq != sirq) {
			if (targetbit) {
				p->pi[0].isr |= targetbit;
				p->pi[0].irr &= ~targetbit;
				p->pi[0].level[p->pi[0].levels++] = irq;
				if (irq == 0) {									// ver0.28
					nevent_reset(NEVENT_PICMASK);
				}
// TRACEOUT(("hardware-int %.2x", (p->pi[0].icw[1] & 0xf8) | irq));
				CPU_INTERRUPT((REG8)((p->pi[0].icw[1] & 0xf8) | irq));
				return;
			}
			if ((!p->pi[0].levels) ||
				(p->pi[0].level[p->pi[0].levels - 1] != sirq)) {
				return;
			}
		}
		// スレーヴの処理
		if (!p->pi[1].levels) {
			pry = -1;
		}
		else {
			pry = (SINT8)p->pi[1].pry[p->pi[1].level[p->pi[1].levels - 1]];
		}
		targetbit = 0;
		for (bit=1, i=0; i<8; bit<<=1, i++) {
			if ((p->pi[1].irr & bit) &&
				(!((p->pi[1].imr | p->pi[1].isr) & bit))) {
				if ((SINT8)p->pi[1].pry[i] > pry) {
					pry = p->pi[1].pry[i];
					irq = (BYTE)i;
					targetbit = bit;
				}
			}
		}
		// スレーヴの割込
		if (targetbit) {
			if (!(p->pi[0].icw[2] & targetbit)) {
				p->pi[1].isr |= targetbit;
				p->pi[1].irr &= ~targetbit;
				p->pi[1].level[p->pi[1].levels++] = irq;
				if ((!p->pi[0].levels) ||
					(p->pi[0].level[p->pi[0].levels - 1] != sirq)) {
					p->pi[0].isr |= (1 << sirq);
					p->pi[0].irr &= ~(1 << sirq);
					p->pi[0].level[p->pi[0].levels++] = sirq;
				}
// TRACEOUT(("hardware-int %.2x", (p->pi[1].icw[1] & 0xf8) | irq));
				CPU_INTERRUPT((REG8)((p->pi[1].icw[1] & 0xf8) | irq));
			}
		}
	}
}


// 簡易モード(SYSTEM TIMERだけ)
void picmask(NEVENTITEM item) {

	PICITEM		pi;

	if (item->flag & NEVENT_SETEVENT) {
		pi = &pic.pi[0];
		pi->irr &= ~(pi->imr & PIC_SYSTEMTIMER);
	}
}

void pic_setirq(REG8 irq) {

	PICITEM	pi;
	REG8	bit;

	pi = pic.pi;
	bit = 1 << (irq & 7);
	if (!(irq & 8)) {
		pi[0].irr |= bit;
		if (pi[0].imr & bit) {
			if (bit & PIC_SYSTEMTIMER) {
				if ((pit.mode[0] & 0x0c) == 0x04) {
					SINT32 cnt;										// ver0.29
					if (pit.value[0] > 8) {
						cnt = pccore.multiple * pit.value[0];
						cnt >>= 2;
					}
					else {
						cnt = pccore.multiple << (16 - 2);
					}
					nevent_set(NEVENT_PICMASK, cnt, picmask, NEVENT_ABSOLUTE);
				}
			}
		}
		if (pi[0].isr & bit) {
			if (bit & PIC_CRTV) {
				pi[0].irr &= ~PIC_CRTV;
				gdc.vsyncint = 1;
			}
		}
	}
	else {
		pi[1].irr |= bit;
	}
}

void pic_resetirq(REG8 irq) {

	PICITEM		pi;

	pi = pic.pi + ((irq >> 3) & 1);
	pi->irr &= ~(1 << (irq & 7));
}


// ---- I/O

static void IOOUTCALL pic_o00(UINT port, REG8 dat) {

	PICITEM		picp;
	UINT		level;

//	TRACEOUT(("pic %x %x", port, dat));
	picp = &pic.pi[(port >> 3) & 1];
	picp->writeicw = 0;
	switch(dat & 0x18) {
		case 0x00:						// eoi
			if (dat & 0x40) {
				level = dat & 7;
			}
			else {
				if (picp->levels == 0) {
					break;
				}
				level = picp->level[picp->levels - 1];
			}
			if (dat & 0x80) {
				if (!(dat & 0x40)) {
					pic_rolpry(picp);
				}
				else {
					pic_downbylevel(picp, level);
				}
			}
			if (dat & 0x20) {
				pic_forceeoibylevel(picp, level);
			}
			nevent_forceexit();				// mainloop exit
			break;

		case 0x08:							// ocw3
			picp->ocw3 = dat;
			break;

		default:
			picp->icw[0] = dat;
			picp->imr = 0;
			picp->irr = 0;										// ver0.28
			picp->ocw3 = 0;										// ver0.25
#if 0
			picp->levels = 0;
			picp->isr = 0;
#endif
			picp->writeicw = 1;
			break;
	}
}

static void IOOUTCALL pic_o02(UINT port, REG8 dat) {

	PICITEM		picp;

//	TRACEOUT(("pic %x %x", port, dat));
	picp = &pic.pi[(port >> 3) & 1];
	if (!picp->writeicw) {
#if 1
		UINT8	set;
		set = picp->imr & (~dat);
		// リセットされたビットは割り込みある？
		if ((CPU_isDI) || (!(picp->irr & set))) {
			picp->imr = dat;
			return;
		}
#endif
		picp->imr = dat;
	}
	else {
		picp->icw[picp->writeicw] = dat;
		picp->writeicw++;
		if (picp->writeicw >= (3 + (picp->icw[0] & 1))) {
			picp->writeicw = 0;
		}
	}
	nevent_forceexit();
}

static REG8 IOINPCALL pic_i00(UINT port) {

	PICITEM		picp;

	picp = &pic.pi[(port >> 3) & 1];
	switch(picp->ocw3 & 0x03) {
		case 0x02:						// read irr
			return(picp->irr);

		case 0x03:						// read isr
			return(picp->isr);

		default:
			return(0x00);				// can can bunny
	}
}

static REG8 IOINPCALL pic_i02(UINT port) {

	PICITEM		picp;

	picp = &pic.pi[(port >> 3) & 1];
	return(picp->imr);
}


// ---- I/F

static const IOOUT pico00[2] = {
					pic_o00,	pic_o02};

static const IOINP pici00[2] = {
					pic_i00,	pic_i02};

void pic_reset(void) {

	pic.pi[0] = def_master;
	pic.pi[1] = def_slave;
}

void pic_bind(void) {

	iocore_attachsysoutex(0x0000, 0x0cf1, pico00, 2);
	iocore_attachsysinpex(0x0000, 0x0cf1, pici00, 2);
}

