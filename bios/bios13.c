#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"


#define	baseport 0x0090

void bios0x13(void) {

	BYTE	stat;
	BYTE	result;
	BYTE	*p;
	BYTE	drv;
	BYTE	drvbit;

//	TRACE_("BIOS", 0x13);
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
			break;
		}
		drv = result & 3;
		drvbit = 1 << drv;
#if 0
		if (result & FDCRLT_IC1) {
			if (mem[0x0055e] & drvbit) {
				continue;
			}
			drvbit = 0;
		}
#endif
		p = &mem[0x00564 + (drv * 8)];
		while(1) {
			*p++ = result;
			stat = iocore_inp8(baseport);
			if ((stat & (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB))
							!= (FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_CB)) {
				break;
			}
			result = iocore_inp8(baseport+2);
		}
		mem[0x0055e] |= drvbit;
	}
	if (mem[0x00480] & 0x10) {
		if (mem[0x00485]) {
			if (!(--mem[0x00485])) {
				mem[0x005a4] |= 0x0f;
			}
		}
	}
}
