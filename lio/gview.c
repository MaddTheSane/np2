#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"lio.h"


typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	vdraw_bg;
	BYTE	vdraw_ln;
} MEMGVIEW;


void lio_makeviewmask(void) {

	if (lio.gview.x1 >= 0) {
		lio.x1 = lio.gview.x1;
	}
	else {
		lio.x1 = 0;
	}
	if (lio.gview.x2 <= 639) {
		lio.x2 = lio.gview.x2;
	}
	else {
		lio.x2 = 639;
	}
	if (lio.gview.y1 >= 0) {
		lio.y1 = lio.gview.y1;
	}
	else {
		lio.y1 = 0;
	}
	if (lio.gview.y2 < lio.scrn.lines) {
		lio.y2 = lio.gview.y2;
	}
	else {
		lio.y2 = lio.scrn.lines-1;
	}
}

REG8 lio_gview(void) {

	MEMGVIEW	dat;
	SINT16		x1;
	SINT16		y1;
	SINT16		x2;
	SINT16		y2;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	x1 = (SINT16)LOADINTELWORD(dat.x1);
	y1 = (SINT16)LOADINTELWORD(dat.y1);
	x2 = (SINT16)LOADINTELWORD(dat.x2);
	y2 = (SINT16)LOADINTELWORD(dat.y2);
	if (((dat.x1 >= dat.x2) || (dat.y1 >= dat.y2)) ||
		((dat.vdraw_bg >= lio.gcolor1.palmax) && (dat.vdraw_bg != 0xff)) ||
		((dat.vdraw_ln >= lio.gcolor1.palmax) && (dat.vdraw_ln != 0xff))) {
		return(5);
	}
	lio.gview.x1 = x1;
	lio.gview.y1 = y1;
	lio.gview.x2 = x2;
	lio.gview.y2 = y2;
	lio.gview.vdraw_bg = dat.vdraw_bg;
	lio.gview.vdraw_ln = dat.vdraw_ln;
	lio_makeviewmask();
	return(0);
}

