#include	"compiler.h"
#include	"parts.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"calendar.h"
#include	"bios.h"
#include	"biosmem.h"


void bios0x1c_03(void) {

	iocore_out8(0x71, 0x00);
	if (pc.cpumode & CPUMODE_8MHz) {
		iocore_out8(0x71, 0x4e);				// 4MHz
	}
	else {
		iocore_out8(0x71, 0x60);				// 5MHz
	}
	pic.pi[0].imr &= ~(PIC_SYSTEMTIMER);
}

void bios0x1c(void) {

	BYTE	buf[6];

	switch(CPU_AH) {
		case 0x00:					// get system timer
			calendar_get(buf);
			i286_memstr_write(CPU_ES, CPU_BX, buf, 6);
			break;

		case 0x01:					// put system timer
			i286_memstr_read(CPU_ES, CPU_BX, buf, 6);
			mem[MEMB_MSW8] = buf[0];								// ver0.29
			calendar_set(buf);
			break;

		case 0x02:					// set interval timer (single)
			SETBIOSMEM16(0x0001c, CPU_BX);
			SETBIOSMEM16(0x0001e, CPU_ES);
			SETBIOSMEM16(0x0058a, CPU_CX);
			iocore_out8(0x77, 0x36);
			bios0x1c_03();
			break;

		case 0x03:					// continue interval timer
			bios0x1c_03();
			break;
	}
}

