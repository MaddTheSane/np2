#include	"compiler.h"
#include	"cpucore.h"
#include	"vram.h"


	VRAM_T	vramop;
	BYTE	tramupdate[0x1000];
	BYTE	vramupdate[0x8000];
#if defined(SUPPORT_PC9821)
	BYTE	vramex[0x80000];
#endif


void vram_initialize(void) {

	ZeroMemory(&vramop, sizeof(vramop));
	i286_vram_dispatch(0);
}

