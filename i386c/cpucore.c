/*	$Id: cpucore.c,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

#include	"compiler.h"
#include	"cpucore.h"


	BYTE		szpcflag[0x200];
	BYTE		szpflag_w[0x10000];


void i386c_initialize(void) {

	DWORD	i;
	DWORD	bit;
	BYTE	f;

	for (i=0; i<0x100; i++) {
		f = P_FLAG;
		for (bit=0x80; bit; bit>>=1) {
			if (i & bit) {
				f ^= P_FLAG;
			}
		}
		if (!(i & 0xff)) {
			f |= Z_FLAG;
		}
		if (i & 0x80) {
			f |= S_FLAG;
		}
		szpcflag[i+0x000] = f;
		szpcflag[i+0x100] = f | C_FLAG;
	}

	for (i=0; i<0x10000; i++) {
		f = P_FLAG;
		for (bit=0x80; bit; bit>>=1) {
			if (i & bit) {
				f ^= P_FLAG;
			}
		}
		if (!i) {
			f |= Z_FLAG;
		}
		if (i & 0x8000) {
			f |= S_FLAG;
		}
		szpflag_w[i] = f;
	}

// #if (defined(I286_C) || defined(I286_ASM_C))
	ia32_init();
// #endif
}
