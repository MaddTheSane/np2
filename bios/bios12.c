#include	"compiler.h"
// #include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"


#define	baseport 0x00c8

void bios0x12(void) {

	BYTE	stat;
	BYTE	result;
	BYTE	*p;
	BYTE	drv;

//	TRACE_("BIOS", 0x12);
	iocore_out8(0x08, 0x20);
	if (!pic.pi[1].isr) {
		iocore_out8(0x00, 0x20);
	}

	stat = iocore_inp8(baseport);
	while(1) {
		if (!(stat & FDCSTAT_CB)) {
			if ((stat & (FDCSTAT_RQM | FDCSTAT_DIO)) != FDCSTAT_RQM) {
				break;
			}
			iocore_out8(baseport+2, 0x08);
			stat = iocore_inp8(baseport);
		}
		if ((stat & (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB))
							!= (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB)) {
			break;
		}
		result = iocore_inp8(baseport+2);
		if (result == FDCRLT_IC1) {
			if (mem[0x005d7]) {
				mem[0x005d7]--;
			}
			break;
		}
		drv = result & 3;
		if (result & (FDCRLT_IC1 | FDCRLT_SE)) {
			p = mem + 0x005d8 + (drv * 2);
		}
		else {
			p = mem + 0x005d0;
		}
		while(1) {
			*p++ = result;
			stat = iocore_inp8(baseport);
			if ((stat & (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB))
							!= (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB)) {
				break;
			}
			result = iocore_inp8(baseport+2);
		}
	}
}

