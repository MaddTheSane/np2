#include	"compiler.h"
#include	"cpucore.h"
#include	"lio.h"


typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	off[2];
	BYTE	seg[2];
	BYTE	leng[2];
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


// ----

typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	chr[2];
	BYTE	mode;
	BYTE	colorsw;
	BYTE	fg;
	BYTE	bg;
} LIOGPUT2;


REG8 lio_gput2(void) {

	LIOGPUT2	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	



	return(0);
}

