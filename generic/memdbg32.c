#include	"compiler.h"

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"memdbg32.h"


typedef struct {
	UINT	mode;
	int		width;
	int		height;
	CMNPAL	pal[MEMDBG32_PALS];
} MEMDBG32;

static	MEMDBG32	memdbg32;



void memdbg32_initialize(void) {

	memdbg32.width = MEMDBG32_BLOCKW * 128;
	memdbg32.height = (MEMDBG32_BLOCKH * 2 * 16) + 0;
}

void memdbg32_setpal(CMNPALFN *palfn) {

	cmndraw_getpals(palfn, memdbg32.pal, MEMDBG32_PALS);
}

void memdbg32_getsize(int *width, int *height) {

	if (width) {
		*width = memdbg32.width;
	}
	if (height) {
		*height = memdbg32.height;
	}
}

BOOL memdbg32_paint(CMNVRAM *vram, BOOL redraw) {

	UINT	mode;
	UINT8	use[16*256];
	UINT	i, j;
	UINT32	pde;
	UINT32	pte;

	mode = 0;
	if (CPU_STAT_PM) {
		mode = 1;
	}
	if (CPU_STAT_VM86) {
		mode = 2;
	}
	if (memdbg32.mode != mode) {
		memdbg32.mode = mode;
		redraw = TRUE;
	}
	if ((!redraw) && (!CPU_STAT_PAGING)) {
		return(FALSE);
	}

	cmddraw_fill(vram, 0, 0, memdbg32.width, memdbg32.height,
											memdbg32.pal + MEMDBG32_PALBG);
	ZeroMemory(use, sizeof(use));
	if (CPU_STAT_PAGING) {
		for (i=0; i<1024; i++) {
			pde = cpu_memoryread_d(CPU_STAT_PDE_BASE + (i * 4));
			if (pde & CPU_PDE_PRESENT) {
				pde &= CPU_PDE_BASEADDR_MASK;
				for (j=0; j<1024; j++) {
					pte = cpu_memoryread_d(pde + (j * 4));
					if ((pte & CPU_PTE_PRESENT) && (pte < 0x1000000)) {
						use[pte >> 12] = MEMDBG32_PALPAGE;
					}
				}
			}
		}
	}
	else {
		FillMemory(use, 256, MEMDBG32_PALREAL);
		FillMemory(use + (0xfa0000 >> 12), (0x60000 >> 12), MEMDBG32_PALREAL);
		if ((CPU_STAT_PM) && (pccore.extmem)) {
			FillMemory(use + 256, 256 * pccore.extmem, MEMDBG32_PALPM);
		}
	}
	for (i=0; i<32; i++) {
		for (j=0; j<128; j++) {
			cmddraw_fill(vram, j * MEMDBG32_BLOCKW, i * MEMDBG32_BLOCKH,
									MEMDBG32_BLOCKW - 1, MEMDBG32_BLOCKH - 1,
									memdbg32.pal + use[(i * 128) + j]);
		}
	}
	return(TRUE);
}

#endif

