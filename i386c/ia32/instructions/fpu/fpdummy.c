/*	$Id: fpdummy.c,v 1.1 2003/12/08 00:55:32 yui Exp $	*/

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"


void
FWAIT(void)
{
}

void
ESC0(void)
{
	DWORD op, madr;

	GET_PCBYTE(op);
	if (op >= 0xc0) {
	} else {
		madr = calc_lea(op);
	}
}
