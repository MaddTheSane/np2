#include	"compiler.h"

#ifndef NP2_MEMORY_ASM

#include	"cpucore.h"
#include	"memory.h"
#include	"egcmem.h"
#include	"mem9821.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"font.h"


	BYTE	mem[0x200000];


#define	USE_HIMEM		0x110000

// ---- write byte

static void MEMCALL i286_wt(UINT32 address, REG8 value) {

	mem[address & CPU_ADRSMASK] = (BYTE)value;
}

static void MEMCALL tram_wt(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= MEMWAIT_TRAM;
	if (address < 0xa2000) {
		mem[address] = (BYTE)value;
		tramupdate[LOW12(address >> 1)] = 1;
		gdcs.textdisp |= 1;
	}
	else if (address < 0xa3fe0) {
		if (!(address & 1)) {
			mem[address] = (BYTE)value;
			tramupdate[LOW12(address >> 1)] = 1;
			gdcs.textdisp |= 1;
		}
	}
	else if (address < 0xa4000) {
		if (!(address & 1)) {
			if ((!(address & 2)) || (gdcs.msw_accessable)) {
				mem[address] = (BYTE)value;
				tramupdate[LOW12(address >> 1)] = 1;
				gdcs.textdisp |= 1;
			}
		}
	}
	else if (address < 0xa5000) {
		if ((address & 1) && (cgwindow.writable & 1)) {
			cgwindow.writable |= 0x80;
			fontrom[cgwindow.high + ((address >> 1) & 0x0f)] = (BYTE)value;
		}
	}
}

static void MEMCALL vram_w0(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	mem[address] = (BYTE)value;
	vramupdate[LOW15(address)] |= 1;
	gdcs.grphdisp |= 1;
}

static void MEMCALL vram_w1(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	mem[address + VRAM_STEP] = (BYTE)value;
	vramupdate[LOW15(address)] |= 2;
	gdcs.grphdisp |= 2;
}

static void MEMCALL grcg_rmw0(UINT32 address, REG8 value) {

	REG8	mask;
	BYTE	*vram;

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

static void MEMCALL grcg_rmw1(UINT32 address, REG8 value) {

	REG8	mask;
	BYTE	*vram;

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

static void MEMCALL grcg_tdw0(UINT32 address, REG8 value) {

	BYTE	*vram;

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

static void MEMCALL grcg_tdw1(UINT32 address, REG8 value) {

	BYTE	*vram;

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

static void MEMCALL egc_wt(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	egc_write(address, value);
}

static void MEMCALL i286_wb(UINT32 address, REG8 value) {

	mem[address + 0x1c8000 - 0xe8000] = (BYTE)value;
}

static void MEMCALL i286_wn(UINT32 address, REG8 value) {

	(void)address;
	(void)value;
}


// ---- read byte

static REG8 MEMCALL i286_rd(UINT32 address) {

	return(mem[address & CPU_ADRSMASK]);
}

static REG8 MEMCALL tram_rd(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_TRAM;
	if (address < 0xa4000) {
		return(mem[address]);
	}
	else if (address < 0xa5000) {
		if (address & 1) {
			return(fontrom[cgwindow.high + ((address >> 1) & 0x0f)]);
		}
		else {
			return(fontrom[cgwindow.low + ((address >> 1) & 0x0f)]);
		}
	}
	return(mem[address]);
}

static REG8 MEMCALL vram_r0(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(mem[address]);
}

static REG8 MEMCALL vram_r1(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(mem[address + VRAM_STEP]);
}

static REG8 MEMCALL grcg_tcr0(UINT32 address) {

const BYTE	*vram;
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

static REG8 MEMCALL grcg_tcr1(UINT32 address) {

const BYTE	*vram;
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

static REG8 MEMCALL egc_rd(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	return(egc_read(address));
}

static REG8 MEMCALL i286_rb(UINT32 address) {

	if (CPU_ITFBANK) {
		address += VRAM_STEP;
	}
	return(mem[address]);
}


// ---- write word

static void MEMCALL i286w_wt(UINT32 address, REG16 value) {

	BYTE	*ptr;

	ptr = mem + (address & CPU_ADRSMASK);
	STOREINTELWORD(ptr, value);
}

static void MEMCALL tramw_wt(UINT32 address, REG16 value) {

	CPU_REMCLOCK -= MEMWAIT_TRAM;
	if (address < 0xa1fff) {
		STOREINTELWORD(mem + address, value);
		tramupdate[LOW12(address >> 1)] = 1;
		tramupdate[LOW12((address + 1) >> 1)] = 1;
		gdcs.textdisp |= 1;
	}
	else if (address == 0xa1fff) {
		STOREINTELWORD(mem + address, value);
		tramupdate[0] = 1;
		tramupdate[0xfff] = 1;
		gdcs.textdisp |= 1;
	}
	else if (address < 0xa3fe0) {
		if (address & 1) {
			address++;
			value >>= 8;
		}
		mem[address] = (BYTE)value;
		tramupdate[LOW12(address >> 1)] = 1;
		gdcs.textdisp |= 1;
	}
	else if (address < 0xa3fff) {
		if (address & 1) {
			address++;
			value >>= 8;
		}
		if ((!(address & 2)) || (gdcs.msw_accessable)) {
			mem[address] = (BYTE)value;
			tramupdate[LOW12(address >> 1)] = 1;
			gdcs.textdisp |= 1;
		}
	}
	else if (address < 0xa5000) {
		if (address & 1) {
			value >>= 8;
		}
		if (cgwindow.writable & 1) {
			cgwindow.writable |= 0x80;
			fontrom[cgwindow.high + ((address >> 1) & 0x0f)] = (BYTE)value;
		}
	}
}


#define GRCGW_NON(page) {											\
	CPU_REMCLOCK -= MEMWAIT_VRAM;									\
	STOREINTELWORD(mem + address + VRAM_STEP*(page), value);		\
	vramupdate[LOW15(address)] |= (1 << page);						\
	vramupdate[LOW15(address + 1)] |= (1 << page);					\
	gdcs.grphdisp |= (1 << page);									\
}

#define GRCGW_RMW(page) {											\
	BYTE	*vram;													\
	CPU_REMCLOCK -= MEMWAIT_GRCG;									\
	address = LOW15(address);										\
	vramupdate[address] |= (1 << page);								\
	vramupdate[address + 1] |= (1 << page);							\
	gdcs.grphdisp |= (1 << page);									\
	vram = mem + address + (VRAM_STEP * (page));					\
	if (!(grcg.modereg & 1)) {										\
		BYTE tmp;													\
		tmp = (BYTE)value;											\
		vram[VRAM0_B+0] &= (~tmp);									\
		vram[VRAM0_B+0] |= (tmp & grcg.tile[0].b[0]);				\
		tmp = (BYTE)(value >> 8);									\
		vram[VRAM0_B+1] &= (~tmp);									\
		vram[VRAM0_B+1] |= (tmp & grcg.tile[0].b[0]);				\
	}																\
	if (!(grcg.modereg & 2)) {										\
		BYTE tmp;													\
		tmp = (BYTE)value;											\
		vram[VRAM0_R+0] &= (~tmp);									\
		vram[VRAM0_R+0] |= (tmp & grcg.tile[1].b[0]);				\
		tmp = (BYTE)(value >> 8);									\
		vram[VRAM0_R+1] &= (~tmp);									\
		vram[VRAM0_R+1] |= (tmp & grcg.tile[1].b[0]);				\
	}																\
	if (!(grcg.modereg & 4)) {										\
		BYTE tmp;													\
		tmp = (BYTE)value;											\
		vram[VRAM0_G+0] &= (~tmp);									\
		vram[VRAM0_G+0] |= (tmp & grcg.tile[2].b[0]);				\
		tmp = (BYTE)(value >> 8);									\
		vram[VRAM0_G+1] &= (~tmp);									\
		vram[VRAM0_G+1] |= (tmp & grcg.tile[2].b[0]);				\
	}																\
	if (!(grcg.modereg & 8)) {										\
		BYTE tmp;													\
		tmp = (BYTE)value;											\
		vram[VRAM0_E+0] &= (~tmp);									\
		vram[VRAM0_E+0] |= (tmp & grcg.tile[3].b[0]);				\
		tmp = (BYTE)(value >> 8);									\
		vram[VRAM0_E+1] &= (~tmp);									\
		vram[VRAM0_E+1] |= (tmp & grcg.tile[3].b[0]);				\
	}																\
}

#define GRCGW_TDW(page) {											\
	BYTE	*vram;													\
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

static void MEMCALL vramw_w0(UINT32 address, REG16 value) GRCGW_NON(0)
static void MEMCALL vramw_w1(UINT32 address, REG16 value) GRCGW_NON(1)
static void MEMCALL grcgw_rmw0(UINT32 address, REG16 value) GRCGW_RMW(0)
static void MEMCALL grcgw_rmw1(UINT32 address, REG16 value) GRCGW_RMW(1)
static void MEMCALL grcgw_tdw0(UINT32 address, REG16 value) GRCGW_TDW(0)
static void MEMCALL grcgw_tdw1(UINT32 address, REG16 value) GRCGW_TDW(1)

static void MEMCALL egcw_wt(UINT32 address, REG16 value) {

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	if (!(address & 1)) {
		egc_write_w(address, value);
	}
	else {
		if (!(egc.sft & 0x1000)) {
			egc_write(address, (REG8)value);
			egc_write(address + 1, (REG8)(value >> 8));
		}
		else {
			egc_write(address + 1, (REG8)(value >> 8));
			egc_write(address, (REG8)value);
		}
	}
}

static void MEMCALL i286w_wb(UINT32 address, REG16 value) {

	mem[address + 0x1c8000 - 0xe8000] = (BYTE)value;
	mem[address + 0x1c8001 - 0xe8000] = (BYTE)(value >> 8);
}

static void MEMCALL i286w_wn(UINT32 address, REG16 value) {

	(void)address;
	(void)value;
}


// ---- read word

static REG16 MEMCALL i286w_rd(UINT32 address) {

	BYTE	*ptr;

	ptr = mem + (address & CPU_ADRSMASK);
	return(LOADINTELWORD(ptr));
}

static REG16 MEMCALL tramw_rd(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_TRAM;
	if (address < (0xa4000 - 1)) {
		return(LOADINTELWORD(mem + address));
	}
	else if (address == 0xa3fff) {
		return(mem[address] + (fontrom[cgwindow.low] << 8));
	}
	else if (address < 0xa4fff) {
		if (address & 1) {
			REG16 ret;
			ret = fontrom[cgwindow.high + ((address >> 1) & 0x0f)];
			ret += fontrom[cgwindow.low + (((address + 1) >> 1) & 0x0f)] << 8;
			return(ret);
		}
		else {
			REG16 ret;
			ret = fontrom[cgwindow.low + ((address >> 1) & 0x0f)];
			ret += fontrom[cgwindow.high + ((address >> 1) & 0x0f)] << 8;
			return(ret);
		}
	}
	else if (address == 0xa4fff) {
		return((mem[0xa5000] << 8) | fontrom[cgwindow.high + 15]);
	}
	return(LOADINTELWORD(mem + address));
}

static REG16 MEMCALL vramw_r0(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(LOADINTELWORD(mem + address));
}

static REG16 MEMCALL vramw_r1(UINT32 address) {

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(LOADINTELWORD(mem + address + VRAM_STEP));
}

static REG16 MEMCALL grcgw_tcr0(UINT32 address) {

	BYTE	*vram;
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

	BYTE	*vram;
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

	REG16	ret;

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	if (!(address & 1)) {
		return(egc_read_w(address));
	}
	else {
		if (!(egc.sft & 0x1000)) {
			ret = egc_read(address);
			ret += egc_read(address + 1) << 8;
			return(ret);
		}
		else {
			ret = egc_read(address + 1) << 8;
			ret += egc_read(address);
			return(ret);
		}
	}
}

static REG16 MEMCALL i286w_rb(UINT32 address) {

	if (CPU_ITFBANK) {
		address += VRAM_STEP;
	}
	return(LOADINTELWORD(mem + address));
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
	MEM8READ	brd8;
	MEM8READ	ird8;
	MEM8WRITE	ewr8;
	MEM8WRITE	bwr8;
	MEM16READ	brd16;
	MEM16READ	ird16;
	MEM16WRITE	ewr16;
	MEM16WRITE	bwr16;
} MMAPTBL;

typedef struct {
	MEM8READ	rd8;
	MEM8WRITE	wr8;
	MEM16READ	rd16;
	MEM16WRITE	wr16;
} VACCTBL;

static MEMFN memfn = {
		   {i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 00
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 20
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 40
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 60
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 80
			tram_rd,	vram_r0,	vram_r0,	vram_r0,		// a0
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// c0
			vram_r0,	i286_rd,	i286_rd,	i286_rb},		// e0

		   {i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 00
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 20
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 40
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 60
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 80
			tram_wt,	vram_w0,	vram_w0,	vram_w0,		// a0
			i286_wn,	i286_wn,	i286_wn,	i286_wn,		// c0
			vram_w0,	i286_wn,	i286_wn,	i286_wn},		// e0

		   {i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 00
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 20
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 40
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 60
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 80
			tramw_rd,	vramw_r0,	vramw_r0,	vramw_r0,		// a0
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// c0
			vramw_r0,	i286w_rd,	i286w_rd,	i286w_rb},		// e0

		   {i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 00
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 20
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 40
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 60
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 80
			tramw_wt,	vramw_w0,	vramw_w0,	vramw_w0,		// a0
			i286w_wn,	i286w_wn,	i286w_wn,	i286w_wn,		// c0
			vramw_w0,	i286w_wn,	i286w_wn,	i286w_wn}};		// e0

static const MMAPTBL mmaptbl[2] = {
		   {i286_rd,	i286_rb,	i286_wn,	i286_wn,
			i286w_rd,	i286w_rb,	i286w_wn,	i286w_wn},
		   {i286_rb,	i286_rb,	i286_wt,	i286_wb,
			i286w_rb,	i286w_rb,	i286w_wt,	i286w_wb}};

static const VACCTBL vacctbl[0x10] = {
			{vram_r0,	vram_w0,	vramw_r0,	vramw_w0},		// 00
			{vram_r1,	vram_w1,	vramw_r1,	vramw_w1},
			{vram_r0,	vram_w0,	vramw_r0,	vramw_w0},
			{vram_r1,	vram_w1,	vramw_r1,	vramw_w1},
			{vram_r0,	vram_w0,	vramw_r0,	vramw_w0},		// 40
			{vram_r1,	vram_w1,	vramw_r1,	vramw_w1},
			{vram_r0,	vram_w0,	vramw_r0,	vramw_w0},
			{vram_r1,	vram_w1,	vramw_r1,	vramw_w1},
			{grcg_tcr0,	grcg_tdw0,	grcgw_tcr0,	grcgw_tdw0},	// 80 tdw/tcr
			{grcg_tcr1,	grcg_tdw1,	grcgw_tcr1,	grcgw_tdw1},
			{egc_rd,	egc_wt,		egcw_rd,	egcw_wt},
			{egc_rd,	egc_wt,		egcw_rd,	egcw_wt},
			{vram_r0,	grcg_rmw0,	vramw_r0,	grcgw_rmw0},	// c0 rmw
			{vram_r1,	grcg_rmw1,	vramw_r1,	grcgw_rmw1},
			{egc_rd,	egc_wt,		egcw_rd,	egcw_wt},
			{egc_rd,	egc_wt,		egcw_rd,	egcw_wt}};


static REG8 MEMCALL i286_nonram_r(UINT32 address) {

	(void)address;
	return(0xff);
}

static REG16 MEMCALL i286_nonram_rw(UINT32 address) {

	(void)address;
	return(0xffff);
}


void MEMCALL i286_memorymap(UINT type) {

const MMAPTBL	*mm;

	mm = mmaptbl + (type & 1);

	memfn.rd8[0xe8000 >> 15] = mm->brd8;
	memfn.rd8[0xf0000 >> 15] = mm->brd8;
	memfn.rd8[0xf8000 >> 15] = mm->ird8;

	memfn.wr8[0xd0000 >> 15] = mm->ewr8;
	memfn.wr8[0xd8000 >> 15] = mm->ewr8;
	memfn.wr8[0xe8000 >> 15] = mm->bwr8;
	memfn.wr8[0xf0000 >> 15] = mm->bwr8;
	memfn.wr8[0xf8000 >> 15] = mm->bwr8;

	memfn.rd16[0xe8000 >> 15] = mm->brd16;
	memfn.rd16[0xf0000 >> 15] = mm->brd16;
	memfn.rd16[0xf8000 >> 15] = mm->ird16;

	memfn.wr16[0xd0000 >> 15] = mm->ewr16;
	memfn.wr16[0xd8000 >> 15] = mm->ewr16;
	memfn.wr16[0xe8000 >> 15] = mm->bwr16;
	memfn.wr16[0xf0000 >> 15] = mm->bwr16;
	memfn.wr16[0xf8000 >> 15] = mm->bwr16;
}

void MEMCALL i286_vram_dispatch(UINT func) {

const VACCTBL	*vacc;

	vacc = vacctbl + (func & 0x0f);
#if defined(SUPPORT_PC9821)
	if (!(func & 0x20)) {
#endif
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
			memfn.wr8[0xe0000 >> 15] = i286_wn;
			memfn.wr16[0xe0000 >> 15] = i286w_wn;
			memfn.rd8[0xe0000 >> 15] = i286_nonram_r;
			memfn.rd16[0xe0000 >> 15] = i286_nonram_rw;
		}
#if defined(SUPPORT_PC9821)
	}
	else {
		memfn.rd8[0xa8000 >> 15] = mem9821_b0r;
		memfn.rd8[0xb0000 >> 15] = mem9821_b0r;
		memfn.rd8[0xb8000 >> 15] = vacc->rd8;
		memfn.rd8[0xe0000 >> 15] = mem9821_b2r;

		memfn.wr8[0xa8000 >> 15] = mem9821_b0w;
		memfn.wr8[0xb0000 >> 15] = mem9821_b0w;
		memfn.wr8[0xb8000 >> 15] = vacc->wr8;
		memfn.wr8[0xe0000 >> 15] = mem9821_b2w;

		memfn.rd16[0xa8000 >> 15] = mem9821_b0rw;
		memfn.rd16[0xb0000 >> 15] = mem9821_b0rw;
		memfn.rd16[0xb8000 >> 15] = vacc->rd16;
		memfn.rd16[0xe0000 >> 15] = mem9821_b2rw;

		memfn.wr16[0xa8000 >> 15] = mem9821_b0ww;
		memfn.wr16[0xb0000 >> 15] = mem9821_b0ww;
		memfn.wr16[0xb8000 >> 15] = vacc->wr16;
		memfn.wr16[0xe0000 >> 15] = mem9821_b2ww;
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
			return(memfn.rd8[(addr >> 15) & 0x1f](addr - 0x00f00000));
		}
#if defined(SUPPORT_PC9821)
		else if (addr >= 0xfff00000) {
			return(mem9821_r(addr));
		}
#endif
		else {
			return(0xff);
		}
	}
	else {
		return(memfn.rd8[(addr >> 15) & 0x1f](addr));
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
				return(memfn.rd16[(addr >> 15) & 0x1f](addr - 0x00f00000));
			}
#if defined(SUPPORT_PC9821)
			else if (addr >= 0xfff00000) {
				return(mem9821_rw(addr));
			}
#endif
			else {
				return(0xffff);
			}
		}
		return(memfn.rd16[(addr >> 15) & 0x1f](addr));
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
		mem[addr] = (BYTE)value;
	}
	else if (addr >= USE_HIMEM) {
		pos = (addr & CPU_ADRSMASK) - 0x100000;
		if (pos < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[pos] = (BYTE)value;
		}
		else if ((addr >= 0x00fa0000) && (addr < 0x01000000)) {
			memfn.wr8[(addr >> 15) & 0x1f](addr - 0x00f00000, value);
		}
#if defined(SUPPORT_PC9821)
		else if (addr >= 0xfff00000) {
			mem9821_w(addr, value);
		}
#endif
	}
	else {
		memfn.wr8[(addr >> 15) & 0x1f](addr, value);
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
				memfn.wr16[(addr >> 15) & 0x1f](addr - 0x00f00000, value);
			}
#if defined(SUPPORT_PC9821)
			else if (addr >= 0xfff00000) {
				mem9821_ww(addr, value);
			}
#endif
		}
		else {
			memfn.wr16[(addr >> 15) & 0x1f](addr, value);
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
		mem[address] = (BYTE)value;
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

	BYTE *out = (BYTE *)dat;
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

	const BYTE *out = (BYTE *)dat;
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

static UINT32 physicaladdr(UINT32 addr) {

	UINT32	pde;
	UINT32	pte;

	if (CPU_STAT_PAGING) {
		pde = i286_memoryread_d(CPU_STAT_PDE_BASE + ((addr >> 20) & 0xffc));
		if (!(pde & CPU_PDE_PRESENT)) {
			goto retdummy;
		}
		pte = cpu_memoryread_d((pde & CPU_PDE_BASEADDR_MASK) + ((addr >> 10) & 0xffc));
		if (!(pte & CPU_PTE_PRESENT)) {
			goto retdummy;
		}
		addr = (pte & CPU_PTE_BASEADDR_MASK) + (addr & 0x00000fff);
	}
	return(addr);

retdummy:
	return(0x01000000);		// ÇƒÇ´Ç∆Å[Ç…ÉÅÉÇÉäÇ™ë∂ç›ÇµÇ»Ç¢èÍèä
}


REG8 MEMCALL meml_read8(UINT seg, UINT off) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (CPU_STAT_PAGING) {
		addr = physicaladdr(addr);
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
		return(i286_memoryread_w(physicaladdr(addr)));
	}
	return(meml_read8(seg, off) + (meml_read8(seg, off + 1) << 8));
}

void MEMCALL meml_write8(UINT seg, UINT off, REG8 dat) {

	UINT32	addr;

	addr = (seg << 4) + LOW16(off);
	if (CPU_STAT_PAGING) {
		addr = physicaladdr(addr);
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
		i286_memorywrite_w(physicaladdr(addr), dat);
	}
	else {
		meml_write8(seg, off, (REG8)dat);
		meml_write8(seg, off + 1, (REG8)(dat >> 8));
	}
}

void MEMCALL meml_readstr(UINT seg, UINT off, void *dat, UINT leng) {

	UINT32	adrs;
	UINT	size;

	while(leng) {
		off = LOW16(off);
		adrs = (seg << 4) + off;
		size = 0x1000 - (adrs & 0xfff);
		size = min(size, leng);
		size = min(size, 0x10000 - off);
		memp_read(physicaladdr(adrs), dat, size);
		off += size;
		dat = ((BYTE *)dat) + size;
		leng -= size;
	}
}

void MEMCALL meml_writestr(UINT seg, UINT off, const void *dat, UINT leng) {

	UINT32	adrs;
	UINT	size;

	while(leng) {
		off = LOW16(off);
		adrs = (seg << 4) + off;
		size = 0x1000 - (adrs & 0xfff);
		size = min(size, leng);
		size = min(size, 0x10000 - off);
		memp_write(physicaladdr(adrs), dat, size);
		off += size;
		dat = ((BYTE *)dat) + size;
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
			memp_read(physicaladdr(address), dat, size);
			address += size;
			dat = ((BYTE *)dat) + size;
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
			memp_write(physicaladdr(address), dat, size);
			address += size;
			dat = ((BYTE *)dat) + size;
			leng -= size;
		}
	}
}
#endif

