/*	$Id: fpdummy.c,v 1.3 2004/02/20 16:09:05 monaka Exp $	*/

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
	UINT32 op, madr;

	GET_PCBYTE(op);
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}
