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





// ---- write byte

static void MEMCALL grcg_rmw0(UINT32 address, REG8 value) {		// VRAM

	REG8	mask;
	UINT8	*vram;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	mask = ~value;
	address = LOW15(address);
	vramupdate[address] |= 1;
	gdcs.grphdisp |= 1;
	vram = mem + address;
	if (!(grcg.modereg & 1)) {
		vram[VRAM0_B] &= mask;
		vram[VRAM0_B] |= (value & grcg.tile[0].b[0]);
	}
	if (!(grcg.modereg & 2)) {
		vram[VRAM0_R] &= mask;
		vram[VRAM0_R] |= (value & grcg.tile[1].b[0]);
	}
	if (!(grcg.modereg & 4)) {
		vram[VRAM0_G] &= mask;
		vram[VRAM0_G] |= (value & grcg.tile[2].b[0]);
	}
	if (!(grcg.modereg & 8)) {
		vram[VRAM0_E] &= mask;
		vram[VRAM0_E] |= (value & grcg.tile[3].b[0]);
	}
}

static void MEMCALL grcg_rmw1(UINT32 address, REG8 value) {		// VRAM

	REG8	mask;
	UINT8	*vram;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	mask = ~value;
	address = LOW15(address);
	vramupdate[address] |= 2;
	gdcs.grphdisp |= 2;
	vram = mem + address;
	if (!(grcg.modereg & 1)) {
		vram[VRAM1_B] &= mask;
		vram[VRAM1_B] |= (value & grcg.tile[0].b[0]);
	}
	if (!(grcg.modereg & 2)) {
		vram[VRAM1_R] &= mask;
		vram[VRAM1_R] |= (value & grcg.tile[1].b[0]);
	}
	if (!(grcg.modereg & 4)) {
		vram[VRAM1_G] &= mask;
		vram[VRAM1_G] |= (value & grcg.tile[2].b[0]);
	}
	if (!(grcg.modereg & 8)) {
		vram[VRAM1_E] &= mask;
		vram[VRAM1_E] |= (value & grcg.tile[3].b[0]);
	}
}

static void MEMCALL grcg_tdw0(UINT32 address, REG8 value) {		// VRAM

	UINT8	*vram;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	address = LOW15(address);
	vramupdate[address] |= 1;
	gdcs.grphdisp |= 1;
	vram = mem + address;
	if (!(grcg.modereg & 1)) {
		vram[VRAM0_B] = grcg.tile[0].b[0];
	}
	if (!(grcg.modereg & 2)) {
		vram[VRAM0_R] = grcg.tile[1].b[0];
	}
	if (!(grcg.modereg & 4)) {
		vram[VRAM0_G] = grcg.tile[2].b[0];
	}
	if (!(grcg.modereg & 8)) {
		vram[VRAM0_E] = grcg.tile[3].b[0];
	}
	(void)value;
}

static void MEMCALL grcg_tdw1(UINT32 address, REG8 value) {		// VRAM

	UINT8	*vram;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	address = LOW15(address);
	vramupdate[address] |= 2;
	gdcs.grphdisp |= 2;
	vram = mem + address;
	if (!(grcg.modereg & 1)) {
		vram[VRAM1_B] = grcg.tile[0].b[0];
	}
	if (!(grcg.modereg & 2)) {
		vram[VRAM1_R] = grcg.tile[1].b[0];
	}
	if (!(grcg.modereg & 4)) {
		vram[VRAM1_G] = grcg.tile[2].b[0];
	}
	if (!(grcg.modereg & 8)) {
		vram[VRAM1_E] = grcg.tile[3].b[0];
	}
	(void)value;
}

static void MEMCALL egc_wt(UINT32 address, REG8 value) {		// VRAM

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	memegc_wr8(address, value);
}


// ---- read byte

static REG8 MEMCALL grcg_tcr0(UINT32 address) {					// VRAM

const UINT8	*vram;
	REG8	ret;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	vram = mem + LOW15(address);
	ret = 0;
	if (!(grcg.modereg & 1)) {
		ret |= vram[VRAM0_B] ^ grcg.tile[0].b[0];
	}
	if (!(grcg.modereg & 2)) {
		ret |= vram[VRAM0_R] ^ grcg.tile[1].b[0];
	}
	if (!(grcg.modereg & 4)) {
		ret |= vram[VRAM0_G] ^ grcg.tile[2].b[0];
	}
	if (!(grcg.modereg & 8)) {
		ret |= vram[VRAM0_E] ^ grcg.tile[3].b[0];
	}
	return(ret ^ 0xff);
}

static REG8 MEMCALL grcg_tcr1(UINT32 address) {					// VRAM

const UINT8	*vram;
	REG8	ret;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	ret = 0;
	vram = mem + LOW15(address);
	if (!(grcg.modereg & 1)) {
		ret |= vram[VRAM1_B] ^ grcg.tile[0].b[0];
	}
	if (!(grcg.modereg & 2)) {
		ret |= vram[VRAM1_R] ^ grcg.tile[1].b[0];
	}
	if (!(grcg.modereg & 4)) {
		ret |= vram[VRAM1_G] ^ grcg.tile[2].b[0];
	}
	if (!(grcg.modereg & 8)) {
		ret |= vram[VRAM1_E] ^ grcg.tile[3].b[0];
	}
	return(ret ^ 0xff);
}

static REG8 MEMCALL egc_rd(UINT32 address) {					// VRAM

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	return(memegc_rd8(address));
}


// ---- write word

#define GRCGW_RMW(page) {											\
	UINT8	*vram;													\
	CPU_REMCLOCK -= MEMWAIT_GRCG;									\
	address = LOW15(address);										\
	vramupdate[address] |= (1 << page);								\
	vramupdate[address + 1] |= (1 << page);							\
	gdcs.grphdisp |= (1 << page);									\
	vram = mem + address + (VRAM_STEP * (page));					\
	if (!(grcg.modereg & 1)) {										\
		UINT8 tmp;													\
		tmp = (UINT8)value;											\
		vram[VRAM0_B+0] &= (~tmp);									\
		vram[VRAM0_B+0] |= (tmp & grcg.tile[0].b[0]);				\
		tmp = (UINT8)(value >> 8);									\
		vram[VRAM0_B+1] &= (~tmp);									\
		vram[VRAM0_B+1] |= (tmp & grcg.tile[0].b[0]);				\
	}																\
	if (!(grcg.modereg & 2)) {										\
		UINT8 tmp;													\
		tmp = (UINT8)value;											\
		vram[VRAM0_R+0] &= (~tmp);									\
		vram[VRAM0_R+0] |= (tmp & grcg.tile[1].b[0]);				\
		tmp = (UINT8)(value >> 8);									\
		vram[VRAM0_R+1] &= (~tmp);									\
		vram[VRAM0_R+1] |= (tmp & grcg.tile[1].b[0]);				\
	}																\
	if (!(grcg.modereg & 4)) {										\
		UINT8 tmp;													\
		tmp = (UINT8)value;											\
		vram[VRAM0_G+0] &= (~tmp);									\
		vram[VRAM0_G+0] |= (tmp & grcg.tile[2].b[0]);				\
		tmp = (UINT8)(value >> 8);									\
		vram[VRAM0_G+1] &= (~tmp);									\
		vram[VRAM0_G+1] |= (tmp & grcg.tile[2].b[0]);				\
	}																\
	if (!(grcg.modereg & 8)) {										\
		UINT8 tmp;													\
		tmp = (UINT8)value;											\
		vram[VRAM0_E+0] &= (~tmp);									\
		vram[VRAM0_E+0] |= (tmp & grcg.tile[3].b[0]);				\
		tmp = (UINT8)(value >> 8);									\
		vram[VRAM0_E+1] &= (~tmp);									\
		vram[VRAM0_E+1] |= (tmp & grcg.tile[3].b[0]);				\
	}																\
}

#define GRCGW_TDW(page) {											\
	UINT8	*vram;													\
	CPU_REMCLOCK -= MEMWAIT_GRCG;									\
	address = LOW15(address);										\
	vramupdate[address] |= (1 << page);								\
	vramupdate[address + 1] |= (1 << page);							\
	gdcs.grphdisp |= (1 << page);									\
	vram = mem + address + (VRAM_STEP * (page));					\
	if (!(grcg.modereg & 1)) {										\
		vram[VRAM0_B+0] = grcg.tile[0].b[0];						\
		vram[VRAM0_B+1] = grcg.tile[0].b[0];						\
	}																\
	if (!(grcg.modereg & 2)) {										\
		vram[VRAM0_R+0] = grcg.tile[1].b[0];						\
		vram[VRAM0_R+1] = grcg.tile[1].b[0];						\
	}																\
	if (!(grcg.modereg & 4)) {										\
		vram[VRAM0_G+0] = grcg.tile[2].b[0];						\
		vram[VRAM0_G+1] = grcg.tile[2].b[0];						\
	}																\
	if (!(grcg.modereg & 8)) {										\
		vram[VRAM0_E+0] = grcg.tile[3].b[0];						\
		vram[VRAM0_E+1] = grcg.tile[3].b[0];						\
	}																\
	(void)value;													\
}

static void MEMCALL grcgw_rmw0(UINT32 address, REG16 value) GRCGW_RMW(0)
static void MEMCALL grcgw_rmw1(UINT32 address, REG16 value) GRCGW_RMW(1)
static void MEMCALL grcgw_tdw0(UINT32 address, REG16 value) GRCGW_TDW(0)
static void MEMCALL grcgw_tdw1(UINT32 address, REG16 value) GRCGW_TDW(1)

static void MEMCALL egcw_wt(UINT32 address, REG16 value) {

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	memegc_wr16(address, value);
}


// ---- read word

static REG16 MEMCALL grcgw_tcr0(UINT32 address) {

	UINT8	*vram;
	REG16	ret;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	ret = 0;
	vram = mem + LOW15(address);
	if (!(grcg.modereg & 1)) {
		ret |= LOADINTELWORD(vram + VRAM0_B) ^ grcg.tile[0].w;
	}
	if (!(grcg.modereg & 2)) {
		ret |= LOADINTELWORD(vram + VRAM0_R) ^ grcg.tile[1].w;
	}
	if (!(grcg.modereg & 4)) {
		ret |= LOADINTELWORD(vram + VRAM0_G) ^ grcg.tile[2].w;
	}
	if (!(grcg.modereg & 8)) {
		ret |= LOADINTELWORD(vram + VRAM0_E) ^ grcg.tile[3].w;
	}
	return((UINT16)~ret);
}

static REG16 MEMCALL grcgw_tcr1(UINT32 address) {

	UINT8	*vram;
	REG16	ret;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	ret = 0;
	vram = mem + LOW15(address);
	if (!(grcg.modereg & 1)) {
		ret |= LOADINTELWORD(vram + VRAM1_B) ^ grcg.tile[0].w;
	}
	if (!(grcg.modereg & 2)) {
		ret |= LOADINTELWORD(vram + VRAM1_R) ^ grcg.tile[1].w;
	}
	if (!(grcg.modereg & 4)) {
		ret |= LOADINTELWORD(vram + VRAM1_G) ^ grcg.tile[2].w;
	}
	if (!(grcg.modereg & 8)) {
		ret |= LOADINTELWORD(vram + VRAM1_E) ^ grcg.tile[3].w;
	}
	return((UINT16)(~ret));
}

static REG16 MEMCALL egcw_rd(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	return(memegc_rd16(address));
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
} MEMFN;

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

static MEMFN memfn = {
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
		{grcg_tcr0,		grcg_tdw0,		grcgw_tcr0,		grcgw_tdw0},	// 80
		{grcg_tcr1,		grcg_tdw1,		grcgw_tcr1,		grcgw_tdw1},
		{egc_rd,		egc_wt,			egcw_rd,		egcw_wt},
		{egc_rd,		egc_wt,			egcw_rd,		egcw_wt},
		{memvram0_rd8,	grcg_rmw0,		memvram0_rd16,	grcgw_rmw0},	// c0
		{memvram1_rd8,	grcg_rmw1,		memvram1_rd16,	grcgw_rmw1},
		{egc_rd,		egc_wt,			egcw_rd,		egcw_wt},
		{egc_rd,		egc_wt,			egcw_rd,		egcw_wt}};


void MEMCALL i286_memorymap(UINT type) {

const MMAPTBL	*mm;

	mm = mmaptbl + (type & 1);

	memfn.rd8[0xe8000 >> 15] = mm->brd8;
	memfn.rd8[0xf0000 >> 15] = mm->brd8;
	memfn.rd8[0xf8000 >> 15] = mm->ird8;
	memfn.wr8[0xe8000 >> 15] = mm->bwr8;
	memfn.wr8[0xf0000 >> 15] = mm->bwr8;
	memfn.wr8[0xf8000 >> 15] = mm->bwr8;

	memfn.rd16[0xe8000 >> 15] = mm->brd16;
	memfn.rd16[0xf0000 >> 15] = mm->brd16;
	memfn.rd16[0xf8000 >> 15] = mm->ird16;
	memfn.wr16[0xe8000 >> 15] = mm->bwr16;
	memfn.wr16[0xf0000 >> 15] = mm->bwr16;
	memfn.wr16[0xf8000 >> 15] = mm->bwr16;
}

void MEMCALL i286_vram_dispatch(UINT func) {

const VACCTBL	*vacc;

	vacc = vacctbl + (func & 0x0f);

	memfn.rd8[0xa8000 >> 15] = vacc->rd8;
	memfn.rd8[0xb0000 >> 15] = vacc->rd8;
	memfn.rd8[0xb8000 >> 15] = vacc->rd8;
	memfn.rd8[0xe0000 >> 15] = vacc->rd8;

	memfn.wr8[0xa8000 >> 15] = vacc->wr8;
	memfn.wr8[0xb0000 >> 15] = vacc->wr8;
	memfn.wr8[0xb8000 >> 15] = vacc->wr8;
	memfn.wr8[0xe0000 >> 15] = vacc->wr8;

	memfn.rd16[0xa8000 >> 15] = vacc->rd16;
	memfn.rd16[0xb0000 >> 15] = vacc->rd16;
	memfn.rd16[0xb8000 >> 15] = vacc->rd16;
	memfn.rd16[0xe0000 >> 15] = vacc->rd16;

	memfn.wr16[0xa8000 >> 15] = vacc->wr16;
	memfn.wr16[0xb0000 >> 15] = vacc->wr16;
	memfn.wr16[0xb8000 >> 15] = vacc->wr16;
	memfn.wr16[0xe0000 >> 15] = vacc->wr16;

	if (!(func & 0x10)) {							// digital
		memfn.rd8[0xe0000 >> 15] = memnc_rd8;
		memfn.wr8[0xe0000 >> 15] = memnc_wr8;
		memfn.rd16[0xe0000 >> 15] = memnc_rd16;
		memfn.wr16[0xe0000 >> 15] = memnc_wr16;
	}
}

REG8 MEMCALL i286_memoryread(UINT32 address) {

	if (address < I286_MEMREADMAX) {
		return(mem[address]);
	}
#if defined(USE_HIMEM)
	else if (address >= USE_HIMEM) {
		address -= 0x100000;
		if (address < CPU_EXTMEMSIZE) {
			return(CPU_EXTMEM[address]);
		}
		else {
			return(0xff);
		}
	}
#endif
	else {
		return(memfn.rd8[(address >> 15) & 0x1f](address));
	}
}

REG16 MEMCALL i286_memoryread_w(UINT32 address) {

	REG16	ret;

	if (address < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + address));
	}
#if defined(USE_HIMEM)
	else if (address >= (USE_HIMEM - 1)) {
		address -= 0x100000;
		if (address == (USE_HIMEM - 0x100000 - 1)) {
			ret = mem[0x100000 + address];
		}
		else if (address < CPU_EXTMEMSIZE) {
			ret = CPU_EXTMEM[address];
		}
		else {
			ret = 0xff;
		}
		address++;
		if (address < CPU_EXTMEMSIZE) {
			ret += CPU_EXTMEM[address] << 8;
		}
		else {
			ret += 0xff00;
		}
		return(ret);
	}
#endif
	else if ((address & 0x7fff) != 0x7fff) {
		return(memfn.rd16[(address >> 15) & 0x1f](address));
	}
	else {
		ret = memfn.rd8[(address >> 15) & 0x1f](address);
		address++;
		ret += memfn.rd8[(address >> 15) & 0x1f](address) << 8;
		return(ret);
	}
}

void MEMCALL i286_memorywrite(UINT32 address, REG8 value) {

	if (address < I286_MEMWRITEMAX) {
		mem[address] = (UINT8)value;
	}
#if defined(USE_HIMEM)
	else if (address >= USE_HIMEM) {
		address -= 0x100000;
		if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (UINT8)value;
		}
	}
#endif
	else {
		memfn.wr8[(address >> 15) & 0x1f](address, value);
	}
}

void MEMCALL i286_memorywrite_w(UINT32 address, REG16 value) {

	if (address < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + address, value);
	}
#if defined(USE_HIMEM)
	else if (address >= (USE_HIMEM - 1)) {
		address -= 0x100000;
		if (address == (USE_HIMEM - 0x100000 - 1)) {
			mem[address] = (UINT8)value;
		}
		else if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (UINT8)value;
		}
		address++;
		if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (UINT8)(value >> 8);
		}
	}
#endif
	else if ((address & 0x7fff) != 0x7fff) {
		memfn.wr16[(address >> 15) & 0x1f](address, value);
	}
	else {
		memfn.wr8[(address >> 15) & 0x1f](address, (UINT8)value);
		address++;
		memfn.wr8[(address >> 15) & 0x1f](address, (UINT8)(value >> 8));
	}
}

REG8 MEMCALL meml_read8(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < I286_MEMREADMAX) {
		return(mem[address]);
	}
	else {
		return(i286_memoryread(address));
	}
}

REG16 MEMCALL meml_read16(UINT seg, UINT off) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + address));
	}
	else {
		return(i286_memoryread_w(address));
	}
}

void MEMCALL meml_write8(UINT seg, UINT off, REG8 value) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < I286_MEMWRITEMAX) {
		mem[address] = (UINT8)value;
	}
	else {
		i286_memorywrite(address, value);
	}
}

void MEMCALL meml_write16(UINT seg, UINT off, REG16 value) {

	UINT32	address;

	address = (seg << 4) + LOW16(off);
	if (address < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + address, value);
	}
	else {
		i286_memorywrite_w(address, value);
	}
}

void MEMCALL meml_readstr(UINT seg, UINT off, void *dat, UINT leng) {

	UINT8	*out;
	UINT32	adrs;
	UINT	size;

	out = (UINT8 *)dat;
	adrs = seg << 4;
	off = LOW16(off);
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

void MEMCALL meml_writestr(UINT seg, UINT off, const void *dat, UINT leng) {

	UINT8	*out;
	UINT32	adrs;
	UINT	size;

	out = (UINT8 *)dat;
	adrs = seg << 4;
	off = LOW16(off);
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

void MEMCALL meml_read(UINT32 address, void *dat, UINT leng) {

	if ((address + leng) < I286_MEMREADMAX) {
		CopyMemory(dat, mem + address, leng);
	}
	else {
		UINT8 *out = (UINT8 *)dat;
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

void MEMCALL meml_write(UINT32 address, const void *dat, UINT leng) {

const UINT8	*out;

	if ((address + leng) < I286_MEMWRITEMAX) {
		CopyMemory(mem + address, dat, leng);
	}
	else {
		out = (UINT8 *)dat;
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

#endif

