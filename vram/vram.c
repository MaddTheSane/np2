#include	"compiler.h"
#include	"cpucore.h"
#include	"vram.h"


	VRAM_T	vramop;
	BYTE	tramupdate[0x1000];
	BYTE	vramupdate[0x8000];


void vram_initialize(void) {

	vramop.operate = 0;
	i286_vram_dispatch(0);
}

