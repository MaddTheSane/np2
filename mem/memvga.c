#include	"compiler.h"

#if defined(SUPPORT_PC9821)

#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"memvga.h"
#include	"vram.h"


// ---- flat

REG8 MEMCALL memvgaf_rd8(UINT32 address) {

	return(vramex[address & 0x7ffff]);
}

void MEMCALL memvgaf_wr8(UINT32 address, REG8 value) {

	UINT8	bit;

	address = address & 0x7ffff;
	vramex[address] = value;
	bit = (address & 0x40000)?2:1;
	vramupdate[LOW15(address >> 3)] |= bit;
	gdcs.grphdisp |= bit;
}

REG16 MEMCALL memvgaf_rd16(UINT32 address) {

	address = address & 0x7ffff;
	return(LOADINTELWORD(vramex + address));
}

void MEMCALL memvgaf_wr16(UINT32 address, REG16 value) {

	UINT8	bit;

	address = address & 0x7ffff;
	STOREINTELWORD(vramex + address, value);
	bit = (address & 0x40000)?2:1;
	vramupdate[LOW15((address + 0) >> 3)] |= bit;
	vramupdate[LOW15((address + 1) >> 3)] |= bit;
	gdcs.grphdisp |= bit;
}


// ---- 8086 bank memory

REG8 MEMCALL memvga0_rd8(UINT32 address) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.mio1[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	return(vramex[addr]);
}

void MEMCALL memvga0_wr8(UINT32 address, REG8 value) {

	UINT32	addr;

	address -= 0xa8000;
	addr = (vramop.mio1[((address >> 14) & 2)] & 15) << 15;
	addr += LOW15(address);
	vramex[addr] = value;
	vramupdate[LOW15(addr >> 3)] |= (addr & 0x40000)?2:1;
	gdcs.grphdisp |= (addr & 0x40000)?2:1;
}

REG16 MEMCALL memvga0_rd16(UINT32 address) {

	REG16	ret;

	ret = memvga0_rd8(address);
	ret |= memvga0_rd8(address + 1) << 8;
	return(ret);
}

void MEMCALL memvga0_wr16(UINT32 address, REG16 value) {

	memvga0_wr8(address + 0, (REG8)value);
	memvga0_wr8(address + 1, (REG8)(value >> 8));
}


// ---- 8086 bank I/O

REG8 MEMCALL memvgaio_rd8(UINT32 address) {

	UINT	pos;

	address -= 0xe0000;
	pos = address - 0x0004;
	if (pos < 4) {
		return(vramop.mio1[pos]);
	}
	pos = address - 0x0100;
	if (pos < 0x40) {
		return(vramop.mio2[pos]);
	}
	return(0x00);
}

void MEMCALL memvgaio_wr8(UINT32 address, REG8 value) {

	UINT	pos;

	address -= 0xe0000;
	pos = address - 0x0004;
	if (pos < 4) {
		vramop.mio1[pos] = value;
		return;
	}
	pos = address - 0x0100;
	if (pos < 0x40) {
		vramop.mio2[pos] = value;
		return;
	}
}

REG16 MEMCALL memvgaio_rd16(UINT32 address) {

	REG16	ret;

	ret = memvgaio_rd8(address);
	ret |= memvgaio_rd8(address + 1) << 8;
	return(ret);
}

void MEMCALL memvgaio_wr16(UINT32 address, REG16 value) {

	memvgaio_wr8(address + 0, (REG8)value);
	memvgaio_wr8(address + 1, (REG8)(value >> 8));
}

#endif

