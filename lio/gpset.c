#include	"compiler.h"
#include	"cpucore.h"
#include	"memory.h"
#include	"lio.h"


typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	pal;
} MEMGPSET;


BYTE lio_gpset(void) {

	MEMGPSET	gpset;
	SINT16		x;
	SINT16		y;

	i286_memstr_read(CPU_DS, CPU_BX, &gpset, sizeof(gpset));
	if (gpset.pal >= lio.gcolor1.palmax) {
		if (CPU_AH == 2) {
			gpset.pal = lio.gcolor1.bgcolor;
		}
		else {
			gpset.pal = lio.gcolor1.fgcolor;
		}
	}
	x = (SINT16)LOADINTELWORD(gpset.x);
	y = (SINT16)LOADINTELWORD(gpset.y);
	lio_pset(x, y, gpset.pal);
	return(0);
}

