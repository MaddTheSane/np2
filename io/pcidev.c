#include	"compiler.h"

// À‘•‚Íƒƒ‚ƒŠ‚ğC³‚µ‚Ä‚©‚ç

#if defined(SUPPORT_PC9821)

#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


void IOOUTCALL pcidev_w32(UINT port, UINT32 value) {
}

UINT32 IOOUTCALL pcidev_r32(UINT port) {

	return((UINT32)-1);
}

void pcidev_reset(void) {
}

void pcidev_bind(void) {
}

#endif

