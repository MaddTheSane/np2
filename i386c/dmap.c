#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"dmap.h"


void dmap(void) {

	DMACH	ch;
	REG8	bit;
	UINT32	addr;

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
				addr = ch->adrs.d;
				if (necio.port0439 & 4) {
					addr = ch->adrs.d & 0xffffff;
				}
				ch->adrs.d += ((ch->mode & 0x20)?-1:1);
				switch(ch->mode & 0x0c) {
					case 0x00:		// verifty
						ch->proc.inproc();
						break;

					case 0x04:		// port->mem
						i286_memorywrite(addr, ch->proc.inproc());
						break;

					default:
						ch->proc.outproc(i286_memoryread(addr));
						break;
				}
			}
			ch++;
			bit <<= 1;
		} while(bit & 0x0f);
	}
}

