#include	"compiler.h"
#include	"cpucore.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"


void bios0x08(void) {

	UINT16	catimcnt;

	catimcnt = GETBIOSMEM16(MEMW_CA_TIM_CNT);
	catimcnt--;
	SETBIOSMEM16(MEMW_CA_TIM_CNT, catimcnt);
	if (catimcnt == 0) {
		pic.pi[0].imr |= PIC_SYSTEMTIMER;
		iocore_out8(0x00, 0x20);
		CPU_INTERRUPT(7);
	}
	else {
		iocore_out8(0x00, 0x20);
		bios0x1c_03();
	}
}

