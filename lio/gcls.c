#include	"compiler.h"
// #include	"cpucore.h"
#include	"memory.h"
#include	"lio.h"


BYTE lio_gcls(void) {

	SINT16	y;

	for (y=lio.y1; y<=lio.y2; y++) {
		lio_line(lio.x1, lio.x2, y, lio.gcolor1.bgcolor);
	}
	return(0);
}

