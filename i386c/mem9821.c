#include	"compiler.h"

#if defined(SUPPORT_PC9821)

#include	"cpucore.h"
#include	"mem9821.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"


void MEMCALL mem9821_w(UINT32 addr, REG8 value) {

	UINT8	bit;

	addr = addr & 0x7ffff;
	vramex[addr] = value;
	bit = (addr & 0x40000)?2:1;
	vramupdate[LOW15(addr >> 3)] |= bit;
	gdcs.grphdisp |= bit;
//	TRACEOUT(("mem9821_w %x %x", addr, value));
}

REG8 MEMCALL mem9821_r(UINT32 addr) {

	return(vramex[addr & 0x7ffff]);
}

void MEMCALL mem9821_ww(UINT32 addr, REG16 value) {

	UINT8	bit;

	addr = addr & 0x7ffff;
	STOREINTELWORD(vramex + addr, value);
	bit = (addr & 0x40000)?2:1;
	vramupdate[LOW15(addr >> 3)] |= bit;
	vramupdate[LOW15((addr + 1) >> 3)] |= bit;
	gdcs.grphdisp |= bit;
//	TRACEOUT(("mem9821_ww %x %x", addr, value));
}

REG16 MEMCALL mem9821_rw(UINT32 addr) {

	addr = addr & 0x7ffff;
	return(LOADINTELWORD(vramex + addr));
}


// ----

void MEMCALL mem9821_b0w(UINT32 address, REG8 value) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.mio1[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	vramex[addr] = value;
	vramupdate[LOW15(addr >> 3)] |= (addr & 0x40000)?2:1;
	gdcs.grphdisp |= (addr & 0x40000)?2:1;
}

REG8 MEMCALL mem9821_b0r(UINT32 address) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.mio1[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	return(vramex[addr]);
}

void MEMCALL mem9821_b0ww(UINT32 address, REG16 value) {

	mem9821_b0w(address + 0, (REG8)value);
	mem9821_b0w(address + 1, (REG8)(value >> 8));
}

REG16 MEMCALL mem9821_b0rw(UINT32 address) {

	REG16	ret;

	ret = mem9821_b0r(address);
	ret |= mem9821_b0r(address + 1) << 8;
	return(ret);
}


void MEMCALL mem9821_b2w(UINT32 addr, REG8 value) {

	UINT	pos;

	addr -= 0xe0000;
	pos = addr - 0x0004;
	if (pos < 4) {
		vramop.mio1[pos] = value;
		return;
	}
	pos = addr - 0x0100;
	if (pos < 0x40) {
		vramop.mio2[pos] = value;
		TRACEOUT(("mem9821_b2w(%.5x, %.2x)", addr, value));
		return;
	}
}

REG8 MEMCALL mem9821_b2r(UINT32 addr) {

	UINT	pos;

	addr -= 0xe0000;
	pos = addr - 0x0004;
	if (pos < 4) {
		return(vramop.mio1[pos]);
	}
	pos = addr - 0x0100;
	if (pos < 0x40) {
		return(vramop.mio2[pos]);
	}
	return(0x00);
}

void MEMCALL mem9821_b2ww(UINT32 address, REG16 value) {

	mem9821_b2w(address + 0, (REG8)value);
	mem9821_b2w(address + 1, (REG8)(value >> 8));
}

REG16 MEMCALL mem9821_b2rw(UINT32 address) {

	REG16	ret;

	ret = mem9821_b2r(address);
	ret |= mem9821_b2r(address + 1) << 8;
	return(ret);
}
#endif

