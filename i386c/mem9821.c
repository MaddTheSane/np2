#include	"compiler.h"
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
	TRACEOUT(("mem9821_w %x %x", addr, value));
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
	TRACEOUT(("mem9821_ww %x %x", addr, value));
}

REG16 MEMCALL mem9821_rw(UINT32 addr) {

	addr = addr & 0x7ffff;
	return(LOADINTELWORD(vramex + addr));
}


// ----

void MEMCALL mem9821_b0w(UINT32 address, REG8 value) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.bank2[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	vramex[addr] = value;
	vramupdate[LOW15(addr >> 3)] |= (addr & 0x40000)?2:1;
	gdcs.grphdisp |= (addr & 0x40000)?2:1;
}

REG8 MEMCALL mem9821_b0r(UINT32 address) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.bank2[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	return(vramex[addr]);
}

void MEMCALL mem9821_b0ww(UINT32 address, REG16 value) {

	mem9821_b0w(address + 0, value);
	mem9821_b0w(address + 1, value >> 8);
}

REG16 MEMCALL mem9821_b0rw(UINT32 address) {

	REG16	ret;

	ret = mem9821_b0r(address);
	ret |= mem9821_b0r(address + 1) << 8;
	return(ret);
}


void MEMCALL mem9821_b2w(UINT32 address, REG8 value) {

	address -= 0xe0004;
	if (address < 4) {
		vramop.bank2[address] = value;
		TRACEOUT(("bank2[%d] = %.2x", address, value));
	}
}

REG8 MEMCALL mem9821_b2r(UINT32 address) {

	address -= 0xe0004;
	if (address < 4) {
		return(vramop.bank2[address]);
	}
	return(0xff);
}

void MEMCALL mem9821_b2ww(UINT32 address, REG16 value) {

	mem9821_b2w(address + 0, value);
	mem9821_b2w(address + 1, value >> 8);
}

REG16 MEMCALL mem9821_b2rw(UINT32 address) {

	REG16	ret;

	ret = mem9821_b2r(address);
	ret |= mem9821_b2r(address + 1) << 8;
	return(ret);
}

