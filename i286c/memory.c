#include	"compiler.h"
#include	"cpucore.h"
#include	"egcmem.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"font.h"


	BYTE	mem[0x200000];


// ---- write byte

static void MEMCALL i286_wt(UINT32 address, REG8 value) {		// MAIN

	mem[address & CPU_ADRSMASK] = (BYTE)value;
}

static void MEMCALL tram_wt(UINT32 address, REG8 value) {		// TRAM

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

static void MEMCALL vram_w0(UINT32 address, REG8 value) {		// VRAM

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	mem[address] = (BYTE)value;
	vramupdate[LOW15(address)] |= 1;
	gdcs.grphdisp |= 1;
}

static void MEMCALL vram_w1(UINT32 address, REG8 value) {		// VRAM

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	mem[address + VRAM_STEP] = (BYTE)value;
	vramupdate[LOW15(address)] |= 2;
	gdcs.grphdisp |= 2;
}

static void MEMCALL grcg_rmw0(UINT32 address, REG8 value) {		// VRAM

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

static void MEMCALL grcg_rmw1(UINT32 address, REG8 value) {		// VRAM

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

static void MEMCALL grcg_tdw0(UINT32 address, REG8 value) {		// VRAM

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

static void MEMCALL grcg_tdw1(UINT32 address, REG8 value) {		// VRAM

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

static void MEMCALL egc_wt(UINT32 address, REG8 value) {		// VRAM

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	egc_write(address, value);
}

static void MEMCALL emmc_wt(UINT32 address, REG8 value) {		// EMS

	CPU_EMSPTR[(address >> 14) & 3][LOW14(address)] = (BYTE)value;
}

static void MEMCALL i286_wd(UINT32 address, REG8 value) {		// D000Å`DFFF

	if (CPU_RAM_D000 & (1 << ((address >> 12) & 15))) {
		mem[address] = (BYTE)value;
	}
}

static void MEMCALL i286_wb(UINT32 address, REG8 value) {		// F800Å`FFFF

	mem[address + 0x1c8000 - 0xe8000] = (BYTE)value;
}

static void MEMCALL i286_wn(UINT32 address, REG8 value) {		// NONE

	(void)address;
	(void)value;
}


// ---- read byte

static REG8 MEMCALL i286_rd(UINT32 address) {					// MAIN

	return(mem[address & CPU_ADRSMASK]);
}

static REG8 MEMCALL tram_rd(UINT32 address) {					// TRAM

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

static REG8 MEMCALL vram_r0(UINT32 address) {					// VRAM

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(mem[address]);
}

static REG8 MEMCALL vram_r1(UINT32 address) {					// VRAM

	CPU_REMCLOCK -= MEMWAIT_VRAM;
	return(mem[address + VRAM_STEP]);
}

static REG8 MEMCALL grcg_tcr0(UINT32 address) {					// VRAM

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

static REG8 MEMCALL grcg_tcr1(UINT32 address) {					// VRAM

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

static REG8 MEMCALL egc_rd(UINT32 address) {					// VRAM

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	return(egc_read(address));
}

static REG8 MEMCALL emmc_rd(UINT32 address) {					// EMS

	return(CPU_EMSPTR[(address >> 14) & 3][LOW14(address)]);
}

static REG8 MEMCALL i286_rb(UINT32 address) {					// F800-FFFF

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
		if (!(address & 1)) {
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
	egc_write_w(address, value);
}

static void MEMCALL emmcw_wt(UINT32 address, REG16 value) {

	BYTE	*ptr;

	if ((address & 0x3fff) != 0x3fff) {
		ptr = CPU_EMSPTR[(address >> 14) & 3] + LOW14(address);
		STOREINTELWORD(ptr, value);
	}
	else {
		CPU_EMSPTR[(address >> 14) & 3][0x3fff] = (BYTE)value;
		CPU_EMSPTR[((address + 1) >> 14) & 3][0] = (BYTE)(value >> 8);
	}
}

static void MEMCALL i286w_wd(UINT32 address, REG16 value) {

	BYTE	*ptr;
	UINT16	bit;

	ptr = mem + address;
	bit = 1 << ((address >> 12) & 15);
	if ((address + 1) & 0xfff) {
		if (CPU_RAM_D000 & bit) {
			STOREINTELWORD(ptr, value);
		}
	}
	else {
		if (CPU_RAM_D000 & bit) {
			ptr[0] = (UINT8)value;
		}
		if (CPU_RAM_D000 & (bit << 1)) {
			ptr[1] = (UINT8)(value >> 8);
		}
	}
}

static void MEMCALL i286w_wb(UINT32 address, REG16 value) {

	BYTE	*ptr;

	ptr = mem + (address + 0x1c8000 - 0xe8000);
	STOREINTELWORD(ptr, value);
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

	CPU_REMCLOCK -= MEMWAIT_GRCG;
	return(egc_read_w(address));
}

static REG16 MEMCALL emmcw_rd(UINT32 address) {

const BYTE	*ptr;
	REG16	ret;

	if ((address & 0x3fff) != 0x3fff) {
		ptr = CPU_EMSPTR[(address >> 14) & 3] + LOW14(address);
		return(LOADINTELWORD(ptr));
	}
	else {
		ret = CPU_EMSPTR[(address >> 14) & 3][0x3fff];
		ret += CPU_EMSPTR[((address + 1) >> 14) & 3][0] << 8;
		return(ret);
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
		   {i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 00
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 20
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 40
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 60
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 80
			tram_rd,	vram_r0,	vram_r0,	vram_r0,		// a0
			emmc_rd,	emmc_rd,	i286_rd,	i286_rd,		// c0
			vram_r0,	i286_rd,	i286_rd,	i286_rb},		// e0

		   {i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 00
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 20
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 40
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 60
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 80
			tram_wt,	vram_w0,	vram_w0,	vram_w0,		// a0
			emmc_wt,	emmc_wt,	i286_wd,	i286_wd,		// c0
			vram_w0,	i286_wn,	i286_wn,	i286_wn},		// e0

		   {i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 00
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 20
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 40
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 60
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 80
			tramw_rd,	vramw_r0,	vramw_r0,	vramw_r0,		// a0
			emmcw_rd,	emmcw_rd,	i286w_rd,	i286w_rd,		// c0
			vramw_r0,	i286w_rd,	i286w_rd,	i286w_rb},		// e0

		   {i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 00
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 20
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 40
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 60
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 80
			tramw_wt,	vramw_w0,	vramw_w0,	vramw_w0,		// a0
			emmcw_wt,	emmcw_wt,	i286w_wd,	i286w_wd,		// c0
			vramw_w0,	i286w_wn,	i286w_wn,	i286w_wn}};		// e0

static const MMAPTBL mmaptbl[2] = {
		   {i286_rd,	i286_rb,	i286_wn,
			i286w_rd,	i286w_rb,	i286w_wn},
		   {i286_rb,	i286_rb,	i286_wb,
			i286w_rb,	i286w_rb,	i286w_wb}};

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
		memfn.wr8[0xe0000 >> 15] = i286_wn;
		memfn.wr16[0xe0000 >> 15] = i286w_wn;
		memfn.rd8[0xe0000 >> 15] = i286_nonram_r;
		memfn.rd16[0xe0000 >> 15] = i286_nonram_rw;
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
		mem[address] = (BYTE)value;
	}
#if defined(USE_HIMEM)
	else if (address >= USE_HIMEM) {
		address -= 0x100000;
		if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (BYTE)value;
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
			mem[address] = (BYTE)value;
		}
		else if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (BYTE)value;
		}
		address++;
		if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (BYTE)(value >> 8);
		}
	}
#endif
	else if ((address & 0x7fff) != 0x7fff) {
		memfn.wr16[(address >> 15) & 0x1f](address, value);
	}
	else {
		memfn.wr8[(address >> 15) & 0x1f](address, (BYTE)value);
		address++;
		memfn.wr8[(address >> 15) & 0x1f](address, (BYTE)(value >> 8));
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
		mem[address] = (BYTE)value;
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

	BYTE	*out;
	UINT32	adrs;
	UINT	size;

	out = (BYTE *)dat;
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

	BYTE	*out;
	UINT32	adrs;
	UINT	size;

	out = (BYTE *)dat;
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

void MEMCALL meml_write(UINT32 address, const void *dat, UINT leng) {

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

