/*	$Id: fpdummy.c,v 1.2 2004/02/05 16:43:45 monaka Exp $	*/

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
		madr = calc_ea_dst(op);
	}
}
