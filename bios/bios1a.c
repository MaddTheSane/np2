#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"


static void printerbios_11(void) {

	if (iocore_inp8(0x42) & 0x04) {				// busy?
		I286_AH = 0x01;
		iocore_out8(0x40, I286_AL);
#if 0
		iocore_out8(0x46, 0x0e);
		iocore_out8(0x46, 0x0f);
#endif
	}
	else {
		I286_AH = 0x02;
	}
}

void bios0x1a_main(void) {											// ver0.30

	switch(I286_AH & 0x0f) {
		case 0x00:
			if (I286_AH == 0x30) {
				if (I286_CX) {
					do {
						I286_AL = i286_membyte_read(I286_ES, I286_BX);
						printerbios_11();
						if (I286_AH & 0x02) {
							I286_AH = 0x02;
							return;
						}
						I286_BX++;
					} while(--I286_CX);
					I286_AH = 0x00;
				}
				else {
					I286_AH = 0x02;
				}
			}
			else {
				iocore_out8(0x37, 0x0d);				// printer f/f
				iocore_out8(0x46, 0x82);				// reset
				iocore_out8(0x46, 0x0f);				// PSTB inactive
				iocore_out8(0x37, 0x0c);				// printer f/f
				I286_AH = (iocore_inp8(0x42) >> 2) & 1;
			}
			break;
		case 0x01:
			printerbios_11();
			break;
		case 0x02:
			I286_AH = (iocore_inp8(0x42) >> 2) & 1;
			break;
		default:
			I286_AH = 0x00;
			break;
	}
}

void bios0x1a(void) {

	if (I286_AH & 0x10) {
		bios0x1a_main();											// ver0.30
	}
	else {
		if (I286_AH == 0x04) {
			I286_AH = 0x02;
		}
		else {
			I286_AH = 0x00;
		}
	}
}

