#include	"compiler.h"

#ifndef NP2_MEMORY_ASM

#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"memtram.h"
#include	"memvram.h"
#include	"memegc.h"
#if defined(SUPPORT_PC9821)
#include	"memvga.h"
#endif
#include	"memems.h"
#include	"memepp.h"
#include	"vram.h"
#include	"font.h"


	UINT8	mem[0x200000];


// ---- MAIN

static REG8 MEMCALL memmain_rd8(UINT32 address) {

	return(mem[address & CPU_ADRSMASK]);
}

static REG16 MEMCALL memmain_rd16(UINT32 address) {

const UINT8	*ptr;

	ptr = mem + (address & CPU_ADRSMASK);
	return(LOADINTELWORD(ptr));
}

static void MEMCALL memmain_wr8(UINT32 address, REG8 value) {

	mem[address & CPU_ADRSMASK] = (UINT8)value;
}

static void MEMCALL memmain_wr16(UINT32 address, REG16 value) {

	UINT8	*ptr;

	ptr = mem + (address & CPU_ADRSMASK);
	STOREINTELWORD(ptr, value);
}


// ---- N/C

static REG8 MEMCALL memnc_rd8(UINT32 address) {

	(void)address;
	return(0xff);
}

static REG16 MEMCALL memnc_rd16(UINT32 address) {

	(void)address;
	return(0xffff);
}

static void MEMCALL memnc_wr8(UINT32 address, REG8 value) {

	(void)address;
	(void)value;
}

static void MEMCALL memnc_wr16(UINT32 address, REG16 value) {

	(void)address;
	(void)value;
}


// ---- table

typedef void (MEMCALL * MEM8WRITE)(UINT32 address, REG8 value);
typedef REG8 (MEMCALL * MEM8READ)(UINT32 address);
typedef void (MEMCALL * MEM16WRITE)(UINT32 address, REG16 value);
typedef REG16 (MEMCALL * MEM16READ)(UINT32 address);

typedef struct {
	MEM8READ	rd8[0x20];
	MEM8WRITE	wr8[0x20];
	MEM16READ	rd16[0x20];
	MEM16WRITE	wr16[0x20];
} MEMFN0;

typedef struct {
	MEM8READ	brd8;		// E8000-F7FFF byte read
	MEM8READ	ird8;		// F8000-FFFFF byte read
	MEM8WRITE	bwr8;		// E8000-FFFFF byte write
	MEM16READ	brd16;		// E8000-F7FFF word read
	MEM16READ	ird16;		// F8000-FFFFF word read
	MEM16WRITE	bwr16;		// F8000-FFFFF word write
} MMAPTBL;

typedef struct {
	MEM8READ	rd8;
	MEM8WRITE	wr8;
	MEM16READ	rd16;
	MEM16WRITE	wr16;
} VACCTBL;

static MEMFN0 memfn0 = {
	   {memmain_rd8,	memmain_rd8,	memmain_rd8,	memmain_rd8,	// 00
		memmain_rd8,	memmain_rd8,	memmain_rd8,	memmain_rd8,	// 20
		memmain_rd8,	memmain_rd8,	memmain_rd8,	memmain_rd8,	// 40
		memmain_rd8,	memmain_rd8,	memmain_rd8,	memmain_rd8,	// 60
		memmain_rd8,	memmain_rd8,	memmain_rd8,	memmain_rd8,	// 80
		memtram_rd8,	memvram0_rd8,	memvram0_rd8,	memvram0_rd8,	// a0
		memems_rd8,		memems_rd8,		memmain_rd8,	memmain_rd8,	// c0
		memvram0_rd8,	memmain_rd8,	memmain_rd8,	memf800_rd8},	// e0

	   {memmain_wr8,	memmain_wr8,	memmain_wr8,	memmain_wr8,	// 00
		memmain_wr8,	memmain_wr8,	memmain_wr8,	memmain_wr8,	// 20
		memmain_wr8,	memmain_wr8,	memmain_wr8,	memmain_wr8,	// 40
		memmain_wr8,	memmain_wr8,	memmain_wr8,	memmain_wr8,	// 60
		memmain_wr8,	memmain_wr8,	memmain_wr8,	memmain_wr8,	// 80
		memtram_wr8,	memvram0_wr8,	memvram0_wr8,	memvram0_wr8,	// a0
		memems_wr8,		memems_wr8,		memd000_wr8,	memd000_wr8,	// c0
		memvram0_wr8,	memnc_wr8,		memnc_wr8,		memnc_wr8},		// e0

	   {memmain_rd16,	memmain_rd16,	memmain_rd16,	memmain_rd16,	// 00
		memmain_rd16,	memmain_rd16,	memmain_rd16,	memmain_rd16,	// 20
		memmain_rd16,	memmain_rd16,	memmain_rd16,	memmain_rd16,	// 40
		memmain_rd16,	memmain_rd16,	memmain_rd16,	memmain_rd16,	// 60
		memmain_rd16,	memmain_rd16,	memmain_rd16,	memmain_rd16,	// 80
		memtram_rd16,	memvram0_rd16,	memvram0_rd16,	memvram0_rd16,	// a0
		memems_rd16,	memems_rd16,	memmain_rd16,	memmain_rd16,	// c0
		memvram0_rd16,	memmain_rd16,	memmain_rd16,	memf800_rd16},	// e0

	   {memmain_wr16,	memmain_wr16,	memmain_wr16,	memmain_wr16,	// 00
		memmain_wr16,	memmain_wr16,	memmain_wr16,	memmain_wr16,	// 20
		memmain_wr16,	memmain_wr16,	memmain_wr16,	memmain_wr16,	// 40
		memmain_wr16,	memmain_wr16,	memmain_wr16,	memmain_wr16,	// 60
		memmain_wr16,	memmain_wr16,	memmain_wr16,	memmain_wr16,	// 80
		memtram_wr16,	memvram0_wr16,	memvram0_wr16,	memvram0_wr16,	// a0
		memems_wr16,	memems_wr16,	memd000_wr16,	memd000_wr16,	// c0
		memvram0_wr16,	memnc_wr16,		memnc_wr16,		memnc_wr16}};	// e0

static const MMAPTBL mmaptbl[2] = {
		   {memmain_rd8,	memf800_rd8,	memnc_wr8,
			memmain_rd16,	memf800_rd16,	memnc_wr16},
		   {memf800_rd8,	memf800_rd8,	memepson_wr8,
			memf800_rd16,	memf800_rd16,	memepson_wr16}};

static const VACCTBL vacctbl[0x10] = {
		{memvram0_rd8,	memvram0_wr8,	memvram0_rd16,	memvram0_wr16},	// 00
		{memvram1_rd8,	memvram1_wr8,	memvram1_rd16,	memvram1_wr16},
		{memvram0_rd8,	memvram0_wr8,	memvram0_rd16,	memvram0_wr16},
		{memvram1_rd8,	memvram1_wr8,	memvram1_rd16,	memvram1_wr16},
		{memvram0_rd8,	memvram0_wr8,	memvram0_rd16,	memvram0_wr16},	// 40
		{memvram1_rd8,	memvram1_wr8,	memvram1_rd16,	memvram1_wr16},
		{memvram0_rd8,	memvram0_wr8,	memvram0_rd16,	memvram0_wr16},
		{memvram1_rd8,	memvram1_wr8,	memvram1_rd16,	memvram1_wr16},
		{memtcr0_rd8,	memtdw0_wr8,	memtcr0_rd16,	memtdw0_wr16},	// 80
		{memtcr1_rd8,	memtdw1_wr8,	memtcr1_rd16,	memtdw1_wr16},
		{memegc_rd8,	memegc_wr8,		memegc_rd16,	memegc_wr16},
		{memegc_rd8,	memegc_wr8,		memegc_rd16,	memegc_wr16},
		{memvram0_rd8,	memrmw0_wr8,	memvram0_rd16,	memrmw0_wr16},	// c0
		{memvram1_rd8,	memrmw1_wr8,	memvram1_rd16,	memrmw1_wr16},
		{memegc_rd8,	memegc_wr8,		memegc_rd16,	memegc_wr16},
		{memegc_rd8,	memegc_wr8,		memegc_rd16,	memegc_wr16}};


void MEMCALL i286_memorymap(UINT type) {

const MMAPTBL	*mm;

	mm = mmaptbl + (type & 1);

	memfn0.rd8[0xe8000 >> 15] = mm->brd8;
	memfn0.rd8[0xf0000 >> 15] = mm->brd8;
	memfn0.rd8[0xf8000 >> 15] = mm->ird8;
	memfn0.wr8[0xe8000 >> 15] = mm->bwr8;
	memfn0.wr8[0xf0000 >> 15] = mm->bwr8;
	memfn0.wr8[0xf8000 >> 15] = mm->bwr8;

	memfn0.rd16[0xe8000 >> 15] = mm->brd16;
	memfn0.rd16[0xf0000 >> 15] = mm->brd16;
	memfn0.rd16[0xf8000 >> 15] = mm->ird16;
	memfn0.wr16[0xe8000 >> 15] = mm->bwr16;
	memfn0.wr16[0xf0000 >> 15] = mm->bwr16;
	memfn0.wr16[0xf8000 >> 15] = mm->bwr16;
}

void MEMCALL i286_vram_dispatch(UINT func) {

const VACCTBL	*vacc;

#if defined(SUPPORT_PC9821)
	if (!(func & 0x20)) {
#endif
		vacc = vacctbl + (func & 0x0f);
		memfn0.rd8[0xa8000 >> 15] = vacc->rd8;
		memfn0.rd8[0xb0000 >> 15] = vacc->rd8;
		memfn0.rd8[0xb8000 >> 15] = vacc->rd8;
		memfn0.rd8[0xe0000 >> 15] = vacc->rd8;

		memfn0.wr8[0xa8000 >> 15] = vacc->wr8;
		memfn0.wr8[0xb0000 >> 15] = vacc->wr8;
		memfn0.wr8[0xb8000 >> 15] = vacc->wr8;
		memfn0.wr8[0xe0000 >> 15] = vacc->wr8;

		memfn0.rd16[0xa8000 >> 15] = vacc->rd16;
		memfn0.rd16[0xb0000 >> 15] = vacc->rd16;
		memfn0.rd16[0xb8000 >> 15] = vacc->rd16;
		memfn0.rd16[0xe0000 >> 15] = vacc->rd16;

		memfn0.wr16[0xa8000 >> 15] = vacc->wr16;
		memfn0.wr16[0xb0000 >> 15] = vacc->wr16;
		memfn0.wr16[0xb8000 >> 15] = vacc->wr16;
		memfn0.wr16[0xe0000 >> 15] = vacc->wr16;

		if (!(func & 0x10)) {							// digital
			memfn0.rd8[0xe0000 >> 15] = memnc_rd8;
			memfn0.wr8[0xe0000 >> 15] = memnc_wr8;
			memfn0.rd16[0xe0000 >> 15] = memnc_rd16;
			memfn0.wr16[0xe0000 >> 15] = memnc_wr16;
		}
#if defined(SUPPORT_PC9821)
	}
	else {
		memfn0.rd8[0xa8000 >> 15] = memvga0_rd8;
		memfn0.rd8[0xb0000 >> 15] = memvga0_rd8;
		memfn0.rd8[0xb8000 >> 15] = memnc_rd8;
		memfn0.rd8[0xe0000 >> 15] = memvgaio_rd8;

		memfn0.wr8[0xa8000 >> 15] = memvga0_wr8;
		memfn0.wr8[0xb0000 >> 15] = memvga0_wr8;
		memfn0.wr8[0xb8000 >> 15] = memnc_wr8;
		memfn0.wr8[0xe0000 >> 15] = memvgaio_wr8;

		memfn0.rd16[0xa8000 >> 15] = memvga0_rd16;
		memfn0.rd16[0xb0000 >> 15] = memvga0_rd16;
		memfn0.rd16[0xb8000 >> 15] = memnc_rd16;
		memfn0.rd16[0xe0000 >> 15] = memvgaio_rd16;

		memfn0.wr16[0xa8000 >> 15] = memvga0_wr16;
		memfn0.wr16[0xb0000 >> 15] = memvga0_wr16;
		memfn0.wr16[0xb8000 >> 15] = memnc_wr16;
		memfn0.wr16[0xe0000 >> 15] = memvgaio_wr16;
	}
#endif
}


REG8 MEMCALL i286_memoryread(UINT32 addr) {

	UINT32	pos;

	if (addr < I286_MEMREADMAX) {
		return(mem[addr]);
	}
	else if (addr >= USE_HIMEM) {
		pos = (addr & CPU_ADRSMASK) - 0x100000;
		if (pos < CPU_EXTMEMSIZE) {
			return(CPU_EXTMEM[pos]);
		}
		else if ((addr >= 0x00fa0000) && (addr < 0x01000000)) {
			return(memfn0.rd8[(addr >> 15) & 0x1f](addr - 0x00f00000));
		}
#if defined(SUPPORT_PC9821)
		else if ((addr >= 0x00f00000) && (addr < 0x00f80000)) {
			return(memvgaf_rd8(addr));
		}
		else if ((addr >= 0xfff00000) && (addr < 0xfff80000)) {
			return(memvgaf_rd8(addr));
		}
#endif
		else {
//			TRACEOUT(("out of mem (read8): %x", addr));
			return(0xff);
		}
	}
	else {
		return(memfn0.rd8[(addr >> 15) & 0x1f](addr));
	}
}

REG16 MEMCALL i286_memoryread_w(UINT32 addr) {

	UINT32	pos;
	REG16	ret;

	if (addr < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + addr));
	}
	else if ((addr + 1) & 0x7fff) {				// non 32kb boundary
		if (addr >= USE_HIMEM) {
			pos = (addr & CPU_ADRSMASK) - 0x100000;
			if (pos < CPU_EXTMEMSIZE) {
				return(LOADINTELWORD(CPU_EXTMEM + pos));
			}
			else if ((addr >= 0x00fa0000) && (addr < 0x01000000)) {
				return(memfn0.rd16[(addr >> 15) & 0x1f](addr - 0x00f00000));
			}
#if defined(SUPPORT_PC9821)
			else if ((addr >= 0x00f00000) && (addr < 0x00f80000)) {
				return(memvgaf_rd16(addr));
			}
			else if ((addr >= 0xfff00000) && (addr < 0xfff80000)) {
				return(memvgaf_rd16(addr));
			}
#endif
			else {
//				TRACEOUT(("out of mem (read16): %x", addr));
				return(0xffff);
			}
		}
		return(memfn0.rd16[(addr >> 15) & 0x1f](addr));
	}
	else {
		ret = i286_memoryread(addr);
		ret += (REG16)(i286_memoryread(addr + 1) << 8);
		return(ret);
	}
}

UINT32 MEMCALL i286_memoryread_d(UINT32 addr) {

	UINT32	pos;
	UINT32	ret;

	if (addr < (I286_MEMREADMAX - 3)) {
		return(LOADINTELDWORD(mem + addr));
	}
	else if (addr >= USE_HIMEM) {
		pos = (addr & CPU_ADRSMASK) - 0x100000;
		if ((pos + 3) < CPU_EXTMEMSIZE) {
			return(LOADINTELDWORD(CPU_EXTMEM + pos));
		}
	}
	if (!(addr & 1)) {
		ret = i286_memoryread_w(addr);
		ret += (UINT32)i286_memoryread_w(addr + 2) << 16;
	}
	else {
		ret = i286_memoryread(addr);
		ret += (UINT32)i286_memoryread_w(addr + 1) << 8;
		ret += (UINT32)i286_memoryread(addr + 3) << 24;
	}
	return(ret);
}

void MEMCALL i286_memorywrite(UINT32 addr, REG8 value) {

	UINT32	pos;

	if (addr < I286_MEMWRITEMAX) {
		mem[addr] = (UINT8)value;
	}
	else if (addr >= USE_HIMEM) {
		pos = (addr & CPU_ADRSMASK) - 0x100000;
		if (pos < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[pos] = (UINT8)value;
		}
		else if ((addr >= 0x00fa0000) && (addr < 0x01000000)) {
			memfn0.wr8[(addr >> 15) & 0x1f](addr - 0x00f00000, value);
		}
#if defined(SUPPORT_PC9821)
		else if ((addr >= 0x00f00000) && (addr < 0x00f80000)) {
			memvgaf_wr8(addr, value);
		}
		else if ((addr >= 0xfff00000) && (addr < 0xfff80000)) {
			memvgaf_wr8(addr, value);
		}
#endif
		else {
//			TRACEOUT(("out of mem (write8): %x", addr));
		}
	}
	else {
		memfn0.wr8[(addr >> 15) & 0x1f](addr, value);
	}
}

void MEMCALL i286_memorywrite_w(UINT32 addr, REG16 value) {

	UINT32	pos;

	if (addr < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + addr, value);
	}
	else if ((addr + 1) & 0x7fff) {				// non 32kb boundary
		if (addr >= USE_HIMEM) {
			pos = (addr & CPU_ADRSMASK) - 0x100000;
			if (pos < CPU_EXTMEMSIZE) {
				STOREINTELWORD(CPU_EXTMEM + pos, value);
			}
			else if ((addr >= 0x00fa0000) && (addr < 0x01000000)) {
				memfn0.wr16[(addr >> 15) & 0x1f](addr - 0x00f00000, value);
			}
#if defined(SUPPORT_PC9821)
			else if ((addr >= 0x00f00000) && (addr < 0x00f80000)) {
				memvgaf_wr16(addr, value);
			}
			else if ((addr >= 0xfff00000) && (addr < 0xfff80000)) {
				memvgaf_wr16(addr, value);
			}
#endif
			else {
//				TRACEOUT(("out of mem (write16): %x", addr));
			}
		}
		else {
			memfn0.wr16[(addr >> 15) & 0x1f](addr, value);
		}
	}
	else {
		i286_memorywrite(addr, (UINT8)value);
		i286_memorywrite(addr + 1, (UINT8)(value >> 8));
	}
}

void MEMCALL i286_memorywrite_d(UINT32 addr, UINT32 value) {

	UINT32	pos;

	if (addr < (I286_MEMWRITEMAX - 3)) {
		STOREINTELDWORD(mem + addr, value);
		return;
	}
	else if (addr >= USE_HIMEM) {
		pos = (addr & CPU_ADRSMASK) - 0x100000;
		if ((pos + 3) < CPU_EXTMEMSIZE) {
			STOREINTELDWORD(CPU_EXTMEM + pos, value);
			return;
		}
	}
	if (!(addr & 1)) {
		i286_memorywrite_w(addr, (UINT16)value);
		i286_memorywrite_w(addr + 2, (UINT16)(value >> 16));
	}
	else {
		i286_memorywrite(addr, (UINT8)value);
		i286_memorywrite_w(addr + 1, (UINT16)(value >> 8));
		i286_memorywrite(addr + 3, (UINT8)(value >> 24));
	}
}

#if 0
REG8 MEMCALL i286_membyte_read(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < I286_MEMREADMAX) {
		return(mem[address]);
	}
	else {
		return(i286_memoryread(address));
	}
}

REG16 MEMCALL i286_memword_read(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + address));
	}
	else {
		return(i286_memoryread_w(address));
	}
}

void MEMCALL i286_membyte_write(UINT seg, UINT off, REG8 value) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < I286_MEMWRITEMAX) {
		mem[address] = (UINT8)value;
	}
	else {
		i286_memorywrite(address, value);
	}
}

void MEMCALL i286_memword_write(UINT seg, UINT off, REG16 value) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + address, value);
	}
	else {
		i286_memorywrite_w(address, value);
	}
}
#endif

void MEMCALL memp_read(UINT32 address, void *dat, UINT leng) {

	UINT8 *out = (UINT8 *)dat;
	UINT pos;
	UINT diff;

	/* fast memory access */
	if (address + leng < I286_MEMREADMAX) {
		CopyMemory(dat, mem + address, leng);
		return;
	} else if (address >= USE_HIMEM) {
		pos = (address & CPU_ADRSMASK) - 0x100000;
		if (pos + leng < CPU_EXTMEMSIZE) {
			CopyMemory(dat, CPU_EXTMEM + pos, leng);
			return;
		}
		if (pos < CPU_EXTMEMSIZE) {
			diff = CPU_EXTMEMSIZE - pos;
			CopyMemory(out, CPU_EXTMEM + pos, diff);
			out += diff;
			leng -= diff;
			address += diff;
		}
	}

	/* slow memory access */
	while (leng-- > 0) {
		*out++ = i286_memoryread(address++);
	}
}

void MEMCALL memp_write(UINT32 address, const void *dat, UINT leng) {

	const UINT8 *out = (UINT8 *)dat;
	UINT pos;
	UINT diff;

	/* fast memory access */
	if (address + leng < I286_MEMREADMAX) {
		CopyMemory(mem + address, dat, leng);
		return;
	} else if (address >= USE_HIMEM) {
		pos = (address & CPU_ADRSMASK) - 0x100000;
		if (pos + leng < CPU_EXTMEMSIZE) {
			CopyMemory(CPU_EXTMEM + pos, dat, leng);
			return;
		}
		if (pos < CPU_EXTMEMSIZE) {
			diff = CPU_EXTMEMSIZE - pos;
			CopyMemory(CPU_EXTMEM + pos, dat, diff);
			out += diff;
			leng -= diff;
			address += diff;
		}
	}

	/* slow memory access */
	while (leng-- > 0) {
		i286_memorywrite(address++, *out++);
	}
}


// ---- Logical Space (BIOS)

static UINT32 physicaladdr(UINT32 addr, BOOL wr) {

	UINT32	a;
	UINT32	pde;
	UINT32	pte;

	a = CPU_STAT_PDE_BASE + ((addr >> 20) & 0xffc);
	pde = i286_memoryread_d(a);
	if (!(pde & CPU_PDE_PRESENT)) {
		goto retdummy;
	}
	if (!(pde & CPU_PDE_ACCESS)) {
		i286_memorywrite(a, (UINT8)(pde | CPU_PDE_ACCESS));
	}
	a = (pde & CPU_PDE_BASEADDR_MASK) + ((addr >> 10) & 0xffc);
	pte = cpu_memoryread_d(a);
	if (!(pte & CPU_PTE_PRESENT)) {
		goto retdummy;
	}
	if (!(pte & CPU_PTE_ACCESS)) {
		i286_memorywrite(a, (UINT8)(pte | CPU_PTE_ACCESS));
	}
	if ((wr) && (!(pte & CPU_PTE_DIRTY))) {
		i286_memorywrite(a, (UINT8)(pte | CPU_PTE_DIRTY));
	}
	addr = (pte & CPU_PTE_BASEADDR_MASK) + (addr & 0x00000fff);
	return(addr);

retdummy:
	return(0x01000000);		// ÇƒÇ´Ç∆Å[Ç…ÉÅÉÇÉäÇ™ë∂ç›ÇµÇ»Ç¢èÍèä
}


REG8 MEMCALL meml_read8(UINT seg, UINT off) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (CPU_STAT_PAGING) {
		addr = physicaladdr(addr, FALSE);
	}
	return(i286_memoryread(addr));
}

REG16 MEMCALL meml_read16(UINT seg, UINT off) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (!CPU_STAT_PAGING) {
		return(i286_memoryread_w(addr));
	}
	else if ((addr + 1) & 0xfff) {
		return(i286_memoryread_w(physicaladdr(addr, FALSE)));
	}
	return(meml_read8(seg, off) + (meml_read8(seg, off + 1) << 8));
}

void MEMCALL meml_write8(UINT seg, UINT off, REG8 dat) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (CPU_STAT_PAGING) {
		addr = physicaladdr(addr, TRUE);
	}
	i286_memorywrite(addr, dat);
}

void MEMCALL meml_write16(UINT seg, UINT off, REG16 dat) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (!CPU_STAT_PAGING) {
		i286_memorywrite_w(addr, dat);
	}
	else if ((addr + 1) & 0xfff) {
		i286_memorywrite_w(physicaladdr(addr, TRUE), dat);
	}
	else {
		meml_write8(seg, off, (REG8)dat);
		meml_write8(seg, off + 1, (REG8)(dat >> 8));
	}
}

void MEMCALL meml_readstr(UINT seg, UINT off, void *dat, UINT leng) {

	UINT32	addr;
	UINT	rem;
	UINT	size;

	while(leng) {
		off = LOW16(off);
		addr = (seg << 4) + off;
		rem = 0x10000 - off;
		size = min(leng, rem);
		if (CPU_STAT_PAGING) {
			rem = 0x1000 - (addr & 0xfff);
			size = min(size, rem);
			addr = physicaladdr(addr, FALSE);
		}
		memp_read(addr, dat, size);
		off += size;
		dat = ((UINT8 *)dat) + size;
		leng -= size;
	}
}

void MEMCALL meml_writestr(UINT seg, UINT off, const void *dat, UINT leng) {

	UINT32	addr;
	UINT	rem;
	UINT	size;

	while(leng) {
		off = LOW16(off);
		addr = (seg << 4) + off;
		rem = 0x10000 - off;
		size = min(leng, rem);
		if (CPU_STAT_PAGING) {
			rem = 0x1000 - (addr & 0xfff);
			size = min(size, rem);
			addr = physicaladdr(addr, TRUE);
		}
		memp_write(addr, dat, size);
		off += size;
		dat = ((UINT8 *)dat) + size;
		leng -= size;
	}
}

void MEMCALL meml_read(UINT32 address, void *dat, UINT leng) {

	UINT	size;

	if (!CPU_STAT_PAGING) {
		memp_read(address, dat, leng);
	}
	else {
		while(leng) {
			size = 0x1000 - (address & 0xfff);
			size = min(size, leng);
			memp_read(physicaladdr(address, FALSE), dat, size);
			address += size;
			dat = ((UINT8 *)dat) + size;
			leng -= size;
		}
	}
}

void MEMCALL meml_write(UINT32 address, const void *dat, UINT leng) {

	UINT	size;

	if (!CPU_STAT_PAGING) {
		memp_write(address, dat, leng);
	}
	else {
		while(leng) {
			size = 0x1000 - (address & 0xfff);
			size = min(size, leng);
			memp_write(physicaladdr(address, TRUE), dat, size);
			address += size;
			dat = ((UINT8 *)dat) + size;
			leng -= size;
		}
	}
}

#endif

