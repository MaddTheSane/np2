#include	"compiler.h"
#include	"cpucore.h"
#include	"lio.h"


typedef struct {
	SINT16	x;
	SINT16	y;
	UINT16	off;
	UINT16	seg;
	UINT16	leng;
	BYTE	mode;
	BYTE	colsw;
	BYTE	fgcolor;
	BYTE	bgcolor;
} MEMGPUT1;


BYTE lio_gput1(void) {

	MEMGPUT1	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	return(0);
}

