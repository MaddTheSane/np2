#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"


// NP2上では MEMORYエラーにならないから不要

static const char msg1[] = "PARITY ERROR - ";
static const char msg2[] = "BASE";
static const char msg3[] = "EXTENDED";
static const char msg4[] = " MEMORY";


static UINT16 textout(UINT16 ptr, const char *str) {

	while(*str) {
		i286_membyte_write(0xa000, ptr, (BYTE)*str);
		i286_membyte_write(0xa200, ptr, 0x43);
		str++;
		ptr += 2;
	}
	return(ptr);
}

void bios0x02(void) {

	BYTE	r;
	UINT16	ptr;

	r = iocore_inp8(0x33);
	if (r & 0x06) {
		ptr = textout(0, msg1);
		if (r & 0x04) {
			ptr = textout(ptr, msg2);
		}
		else {
			ptr = textout(ptr, msg3);
		}
		textout(ptr, msg4);
		mem[0x004f8] = 0xfa;		// cli
		mem[0x004f9] = 0xf4;		// hlt
		CPU_CS = 0;
		CPU_IP = 0x4f8;
	}
}

