#include	"compiler.h"
#include	"cpucore.h"
#include	"vram.h"


const UINT32 vramplaneseg[4] = {VRAM_E, VRAM_B, VRAM_R, VRAM_G};

	VRAM_T	vramop;
	BYTE	tramupdate[0x1000];
	BYTE	vramupdate[0x8000];


void vram_init(void) {

	vramop.operate = 0;
	i286_vram_dispatch(0);
}

