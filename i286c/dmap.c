#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"dmap.h"


void dmap_i286(void) {

	DMACH	ch;
	REG8	bit;

	if (dmac.working) {
		ch = dmac.dmach;
		bit = 1;
		do {
			if (dmac.working & bit) {
				// DMA working !
				if (!ch->leng.w) {
					dmac.stat |= bit;
					dmac.working &= ~bit;
					ch->proc.extproc(DMAEXT_END);
				}
				ch->leng.w--;

				switch(ch->mode & 0x0c) {
					case 0x00:		// verifty
						ch->proc.inproc();
						break;

					case 0x04:		// port->mem
						i286_memorywrite(ch->adrs.d, ch->proc.inproc());
						break;

					default:
						ch->proc.outproc(i286_memoryread(ch->adrs.d));
						break;
				}
				ch->adrs.d += ((ch->mode & 0x20)?-1:1);
			}
			ch++;
			bit <<= 1;
		} while(bit & 0x0f);
	}
}

void dmap_v30(void) {

	DMACH	ch;
	REG8	bit;

	if (dmac.working) {
		ch = dmac.dmach;
		bit = 1;
		do {
			if (dmac.working & bit) {
				// DMA working !
				if (!ch->leng.w) {
					dmac.stat |= bit;
					dmac.working &= ~bit;
					ch->proc.extproc(DMAEXT_END);
				}
				ch->leng.w--;

				switch(ch->mode & 0x0c) {
					case 0x00:		// verifty
						ch->proc.inproc();
						break;

					case 0x04:		// port->mem
						i286_memorywrite(ch->adrs.d, ch->proc.inproc());
						break;

					default:
						ch->proc.outproc(i286_memoryread(ch->adrs.d));
						break;
				}
				ch->adrs.w[DMA16_LOW] += ((ch->mode & 0x20)?-1:1);
			}
			ch++;
			bit <<= 1;
		} while(bit & 0x0f);
	}
}

