#include	"compiler.h"

#ifndef NP2_MEMORY_ASM

#include	"cpucore.h"
#include	"memory.h"
#include	"egcmem.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"font.h"


#define	USE_HIMEM

// ---- write byte

static void MEMCALL i286_wt(UINT32 address, REG8 value) {

	mem[address & CPU_ADRSMASK] = (BYTE)value;
}

static void MEMCALL tram_wt(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= vramop.tramwait;
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

	CPU_REMCLOCK -= vramop.vramwait;
	mem[address] = (BYTE)value;
	vramupdate[LOW15(address)] |= 1;
	gdcs.grphdisp |= 1;
}

static void MEMCALL vram_w1(UINT32 address, REG8 value) {

	CPU_REMCLOCK -= vramop.vramwait;
	mem[address + VRAM_STEP] = (BYTE)value;
	vramupdate[LOW15(address)] |= 2;
	gdcs.grphdisp |= 2;
}

static void MEMCALL grcg_rmw0(UINT32 address, REG8 value) {

	REG8	mask;
	BYTE	*vram;

	CPU_REMCLOCK -= vramop.grcgwait;
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

	CPU_REMCLOCK -= vramop.grcgwait;
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

	CPU_REMCLOCK -= vramop.grcgwait;
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

	CPU_REMCLOCK -= vramop.grcgwait;
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

	egc_write(address, value);
}

static void MEMCALL emmc_wt(UINT32 address, REG8 value) {

	extmem.pageptr[(address >> 14) & 3][LOW14(address)] = (BYTE)value;
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

	CPU_REMCLOCK -= vramop.tramwait;
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

	CPU_REMCLOCK -= vramop.vramwait;
	return(mem[address]);
}

static REG8 MEMCALL vram_r1(UINT32 address) {

	CPU_REMCLOCK -= vramop.vramwait;
	return(mem[address + VRAM_STEP]);
}

static REG8 MEMCALL grcg_tcr0(UINT32 address) {

const BYTE	*vram;
	REG8	ret;

	CPU_REMCLOCK -= vramop.grcgwait;
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

	CPU_REMCLOCK -= vramop.grcgwait;
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

	return(egc_read(address));
}

static REG8 MEMCALL emmc_rd(UINT32 address) {

	return(extmem.pageptr[(address >> 14) & 3][LOW14(address)]);
}

static REG8 MEMCALL i286_itf(UINT32 address) {

	if (CPU_ITFBANK) {
		address = ITF_ADRS + LOW15(address);
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
	CPU_REMCLOCK -= vramop.vramwait;								\
	STOREINTELWORD(mem + address + VRAM_STEP*(page), value);		\
	vramupdate[LOW15(address)] |= (1 << page);						\
	vramupdate[LOW15(address + 1)] |= (1 << page);					\
	gdcs.grphdisp |= (1 << page);									\
}

#define GRCGW_RMW(page) {											\
	BYTE	*vram;													\
	CPU_REMCLOCK -= vramop.grcgwait;								\
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
	CPU_REMCLOCK -= vramop.grcgwait;								\
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

static void MEMCALL emmcw_wt(UINT32 address, REG16 value) {

	BYTE	*ptr;

	if ((address & 0x3fff) != 0x3fff) {
		ptr = extmem.pageptr[(address >> 14) & 3] + LOW14(address);
		STOREINTELWORD(ptr, value);
	}
	else {
		extmem.pageptr[(address >> 14) & 3][0x3fff] = (BYTE)value;
		extmem.pageptr[((address + 1) >> 14) & 3][0] = (BYTE)(value >> 8);
	}
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

	CPU_REMCLOCK -= vramop.tramwait;
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

	CPU_REMCLOCK -= vramop.vramwait;
	return(LOADINTELWORD(mem + address));
}

static REG16 MEMCALL vramw_r1(UINT32 address) {

	CPU_REMCLOCK -= vramop.vramwait;
	return(LOADINTELWORD(mem + address + VRAM_STEP));
}

static REG16 MEMCALL grcgw_tcr0(UINT32 address) {

	BYTE	*vram;
	REG16	ret;

	CPU_REMCLOCK -= vramop.grcgwait;
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

	CPU_REMCLOCK -= vramop.grcgwait;
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

static REG16 MEMCALL emmcw_rd(UINT32 address) {

const BYTE	*ptr;
	REG16	ret;

	if ((address & 0x3fff) != 0x3fff) {
		ptr = extmem.pageptr[(address >> 14) & 3] + LOW14(address);
		return(LOADINTELWORD(ptr));
	}
	else {
		ret = extmem.pageptr[(address >> 14) & 3][0x3fff];
		ret += extmem.pageptr[((address + 1) >> 14) & 3][0] << 8;
		return(ret);
	}
}

static REG16 MEMCALL i286w_itf(UINT32 address) {

	if (CPU_ITFBANK) {
		address = ITF_ADRS + LOW15(address);
	}
	return(LOADINTELWORD(mem + address));
}


// ---- table

typedef void (MEMCALL * MEM8WRITE)(UINT32 address, REG8 value);
typedef REG8 (MEMCALL * MEM8READ)(UINT32 address);
typedef void (MEMCALL * MEM16WRITE)(UINT32 address, REG16 value);
typedef REG16 (MEMCALL * MEM16READ)(UINT32 address);

static MEM8WRITE memory_write[] = {
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 00
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 20
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 40
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 60
			i286_wt,	i286_wt,	i286_wt,	i286_wt,		// 80
			tram_wt,	vram_w0,	vram_w0,	vram_w0,		// a0
			emmc_wt,	emmc_wt,	i286_wn,	i286_wn,		// c0
			vram_w0,	i286_wn,	i286_wn,	i286_wn};		// e0

static MEM8READ memory_read[] = {
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 00
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 20
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 40
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 60
			i286_rd,	i286_rd,	i286_rd,	i286_rd,		// 80
			tram_rd,	vram_r0,	vram_r0,	vram_r0,		// a0
			emmc_rd,	emmc_rd,	i286_rd,	i286_rd,		// c0
			vram_r0,	i286_rd,	i286_rd,	i286_itf};		// f0

static MEM16WRITE memword_write[] = {
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 00
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 20
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 40
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 60
			i286w_wt,	i286w_wt,	i286w_wt,	i286w_wt,		// 80
			tramw_wt,	vramw_w0,	vramw_w0,	vramw_w0,		// a0
			emmcw_wt,	emmcw_wt,	i286w_wn,	i286w_wn,		// c0
			vramw_w0,	i286w_wn,	i286w_wn,	i286w_wn};		// e0

static MEM16READ memword_read[] = {
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 00
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 20
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 40
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 60
			i286w_rd,	i286w_rd,	i286w_rd,	i286w_rd,		// 80
			tramw_rd,	vramw_r0,	vramw_r0,	vramw_r0,		// a0
			emmcw_rd,	emmcw_rd,	i286w_rd,	i286w_rd,		// c0
			vramw_r0,	i286w_rd,	i286w_rd,	i286w_itf};		// e0

static const MEM8WRITE vram_write[] = {
			vram_w0,	vram_w1,	vram_w0,	vram_w1,		// 00
			vram_w0,	vram_w1,	vram_w0,	vram_w1,		// 40
			grcg_tdw0,	grcg_tdw1,	egc_wt,		egc_wt,			// 80 tdw/tcr
			grcg_rmw0,	grcg_rmw1,	egc_wt,		egc_wt};		// c0 rmw

static const MEM8READ vram_read[] = {
			vram_r0,	vram_r1,	vram_r0,	vram_r1,		// 00
			vram_r0,	vram_r1,	vram_r0,	vram_r1,		// 40
			grcg_tcr0,	grcg_tcr1,	egc_rd,		egc_rd,			// 80 tdw/tcr
			vram_r0,	vram_r1,	egc_rd,		egc_rd};		// c0 rmw

static const MEM16WRITE vramw_write[] = {
			vramw_w0,	vramw_w1,	vramw_w0,	vramw_w1,		// 00
			vramw_w0,	vramw_w1,	vramw_w0,	vramw_w1,		// 40
			grcgw_tdw0,	grcgw_tdw1,	egcw_wt,	egcw_wt,		// 80 tdw/tcr
			grcgw_rmw0,	grcgw_rmw1,	egcw_wt,	egcw_wt};		// c0 rmw

static const MEM16READ vramw_read[] = {
			vramw_r0,	vramw_r1,	vramw_r0,	vramw_r1,		// 00
			vramw_r0,	vramw_r1,	vramw_r0,	vramw_r1,		// 40
			grcgw_tcr0,	grcgw_tcr1,	egcw_rd,	egcw_rd,		// 80 tdw/tcr
			vramw_r0,	vramw_r1,	egcw_rd,	egcw_rd};		// c0 rmw


static REG8 MEMCALL i286_nonram_r(UINT32 address) {

	(void)address;
	return(0xff);
}

static REG16 MEMCALL i286_nonram_rw(UINT32 address) {

	(void)address;
	return(0xffff);
}

void MEMCALL i286_vram_dispatch(UINT func) {

	UINT	proc;

	proc = func & 0x0f;
	memory_write[0xa8000 >> 15] = vram_write[proc];
	memory_write[0xb0000 >> 15] = vram_write[proc];
	memory_write[0xb8000 >> 15] = vram_write[proc];
	memory_write[0xe0000 >> 15] = vram_write[proc];

	memory_read[0xa8000 >> 15] = vram_read[proc];
	memory_read[0xb0000 >> 15] = vram_read[proc];
	memory_read[0xb8000 >> 15] = vram_read[proc];
	memory_read[0xe0000 >> 15] = vram_read[proc];

	memword_write[0xa8000 >> 15] = vramw_write[proc];
	memword_write[0xb0000 >> 15] = vramw_write[proc];
	memword_write[0xb8000 >> 15] = vramw_write[proc];
	memword_write[0xe0000 >> 15] = vramw_write[proc];

	memword_read[0xa8000 >> 15] = vramw_read[proc];
	memword_read[0xb0000 >> 15] = vramw_read[proc];
	memword_read[0xb8000 >> 15] = vramw_read[proc];
	memword_read[0xe0000 >> 15] = vramw_read[proc];

	if (!(func & 0x10)) {							// degital
		memory_write[0xe0000 >> 15] = i286_wn;
		memword_write[0xe0000 >> 15] = i286w_wn;
		memory_read[0xe0000 >> 15] = i286_nonram_r;
		memword_read[0xe0000 >> 15] = i286_nonram_rw;
	}
#if defined(USE_ASM)
	i286a_vram_dispatch(func);
#endif
}

REG8 MEMCALL __i286_memoryread(UINT32 address) {

	if (address < I286_MEMREADMAX) {
		return(mem[address]);
	}
#if defined(USE_HIMEM)
	else if (address >= 0x10fff0) {
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
		return(memory_read[(address >> 15) & 0x1f](address));
	}
}

REG16 MEMCALL __i286_memoryread_w(UINT32 address) {

	REG16	ret;

	if (address < (I286_MEMREADMAX - 1)) {
		return(LOADINTELWORD(mem + address));
	}
#if defined(USE_HIMEM)
	else if (address >= (0x10fff0 - 1)) {
		address -= 0x100000;
		if (address == (0x00fff0 - 1)) {
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
		return(memword_read[(address >> 15) & 0x1f](address));
	}
	else {
		ret = memory_read[(address >> 15) & 0x1f](address);
		address++;
		ret += memory_read[(address >> 15) & 0x1f](address) << 8;
		return(ret);
	}
}

UINT32 MEMCALL __i286_memoryread_d(UINT32 address) {

	UINT32 ret;

	ret = __i286_memoryread_w(address);
	ret |= (UINT32)__i286_memoryread_w(address + 2) << 16;

	return ret;
}

void MEMCALL __i286_memorywrite(UINT32 address, REG8 value) {

	if (address < I286_MEMWRITEMAX) {
		mem[address] = (BYTE)value;
	}
#if defined(USE_HIMEM)
	else if (address >= 0x10fff0) {
		address -= 0x100000;
		if (address < CPU_EXTMEMSIZE) {
			CPU_EXTMEM[address] = (BYTE)value;
		}
	}
#endif
	else {
		memory_write[(address >> 15) & 0x1f](address, value);
	}
}

void MEMCALL __i286_memorywrite_w(UINT32 address, REG16 value) {

	if (address < (I286_MEMWRITEMAX - 1)) {
		STOREINTELWORD(mem + address, value);
	}
#if defined(USE_HIMEM)
	else if (address >= (0x10fff0 - 1)) {
		address -= 0x100000;
		if (address == (0x00fff0 - 1)) {
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
		memword_write[(address >> 15) & 0x1f](address, value);
	}
	else {
		memory_write[(address >> 15) & 0x1f](address, (BYTE)value);
		address++;
		memory_write[(address >> 15) & 0x1f](address, (BYTE)(value >> 8));
	}
}

void MEMCALL __i286_memorywrite_d(UINT32 address, UINT32 value) {

	__i286_memorywrite_w(address, value & 0xffff);
	__i286_memorywrite_w(address + 2, (WORD)(value >> 16));
}

#if 0
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
#endif

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

#endif

