#include	"compiler.h"
#include	"cpucore.h"
#include	"egcmem.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"font.h"

#define	USE_HIMEM		0x110000


REG8 MEMCALL i286_membyte_read(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + off;
	if (address < I286_MEMREADMAX) {
		return(mem[address]);
	}
	else {
		return(i286_memoryread(address));
	}
}

REG16 MEMCALL i286_memword_read(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + off;
	if (address < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + address));
	}
	else {
		return(i286_memoryread_w(address));
	}
}

void MEMCALL i286_membyte_write(UINT seg, UINT off, REG8 value) {

	UINT32	address;

	address = (seg << 4) + off;
	if (address < I286_MEMWRITEMAX) {
		mem[address] = (BYTE)value;
	}
	else {
		i286_memorywrite(address, value);
	}
}

void MEMCALL i286_memword_write(UINT seg, UINT off, REG16 value) {

	UINT32	address;

	address = (seg << 4) + off;
	if (address < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + address, value);
	}
	else {
		i286_memorywrite_w(address, value);
	}
}

void MEMCALL i286_memstr_read(UINT seg, UINT off, void *dat, UINT leng) {

	BYTE	*out;
	UINT32	adrs;
	UINT	size;

	out = (BYTE *)dat;
	adrs = seg << 4;
	if ((I286_MEMREADMAX >= 0x10000) &&
		(adrs < (I286_MEMREADMAX - 0x10000))) {
		if (leng) {
			size = 0x10000 - off;
			if (size >= leng) {
				CopyMemory(out, mem + adrs + off, leng);
				return;
			}
			CopyMemory(out, mem + adrs + off, size);
			out += size;
			leng -= size;
		}
		while(leng >= 0x10000) {
			CopyMemory(out, mem + adrs, 0x10000);
			out += 0x10000;
			leng -= 0x10000;
		}
		if (leng) {
			CopyMemory(out, mem + adrs, leng);
		}
	}
	else {
		while(leng--) {
			*out++ = i286_memoryread(adrs + off);
			off = LOW16(off + 1);
		}
	}
}

void MEMCALL i286_memstr_write(UINT seg, UINT off,
												const void *dat, UINT leng) {

	BYTE	*out;
	UINT32	adrs;
	UINT	size;

	out = (BYTE *)dat;
	adrs = seg << 4;
	if ((I286_MEMWRITEMAX >= 0x10000) &&
		(adrs < (I286_MEMWRITEMAX - 0x10000))) {
		if (leng) {
			size = 0x10000 - off;
			if (size >= leng) {
				CopyMemory(mem + adrs + off, out, leng);
				return;
			}
			CopyMemory(mem + adrs + off, out, size);
			out += size;
			leng -= size;
		}
		while(leng >= 0x10000) {
			CopyMemory(mem + adrs, out, 0x10000);
			out += 0x10000;
			leng -= 0x10000;
		}
		if (leng) {
			CopyMemory(mem + adrs, out, leng);
		}
	}
	else {
		while(leng--) {
			i286_memorywrite(adrs + off, *out++);
			off = LOW16(off + 1);
		}
	}
}

void MEMCALL i286_memx_read(UINT32 address, void *dat, UINT leng) {

	if ((address + leng) < I286_MEMREADMAX) {
		CopyMemory(dat, mem + address, leng);
	}
	else {
		BYTE *out = (BYTE *)dat;
		if (address < I286_MEMREADMAX) {
			CopyMemory(out, mem + address, I286_MEMREADMAX - address);
			out += I286_MEMREADMAX - address;
			leng -= I286_MEMREADMAX - address;
			address = I286_MEMREADMAX;
		}
		while(leng--) {
			*out++ = i286_memoryread(address++);
		}
	}
}

void MEMCALL i286_memx_write(UINT32 address, const void *dat, UINT leng) {

const BYTE	*out;

	if ((address + leng) < I286_MEMWRITEMAX) {
		CopyMemory(mem + address, dat, leng);
	}
	else {
		out = (BYTE *)dat;
		if (address < I286_MEMWRITEMAX) {
			CopyMemory(mem + address, out, I286_MEMWRITEMAX - address);
			out += I286_MEMWRITEMAX - address;
			leng -= I286_MEMWRITEMAX - address;
			address = I286_MEMWRITEMAX;
		}
		while(leng--) {
			i286_memorywrite(address++, *out++);
		}
	}
}

