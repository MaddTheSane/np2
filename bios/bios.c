#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"sxsibios.h"
#include	"lio.h"
#include	"vram.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"fdfmt.h"
#include	"keytable.res"
#include	"itfrom.res"
#include	"startup.res"
#include	"biosfd80.res"


#define	BIOS_SIMULATE

	BOOL	biosrom = FALSE;

static const char neccheck[] = "Copyright (C) 1983 by NEC Corporation";

typedef struct {
	UINT8	port;
	UINT8	data;
} IODATA;

static const IODATA iodata[] = {
			// DMA
				{0x29, 0x00}, {0x29, 0x01}, {0x29, 0x02}, {0x29, 0x03},
				{0x27, 0x00}, {0x21, 0x00}, {0x23, 0x00}, {0x25, 0x00},
				{0x1b, 0x00}, {0x11, 0x40},

			// PIT
				{0x77, 0x30}, {0x71, 0x00}, {0x71, 0x00},
				{0x77, 0x76}, {0x73, 0xcd}, {0x73, 0x04},
				{0x77, 0xb6},

			// PIC
				{0x00, 0x11}, {0x02, 0x08}, {0x02, 0x80}, {0x02, 0x1d},
				{0x08, 0x11}, {0x0a, 0x10}, {0x0a, 0x07}, {0x0a, 0x09},
				{0x02, 0x7d}, {0x0a, 0x71}};

static const UINT8 msw_default[8] =
							{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e};


static void bios_itfprepare(void) {

const IODATA	*p;
const IODATA	*pterm;

	crtc_biosreset();
	gdc_biosreset();

	p = iodata;
	pterm = iodata + (sizeof(iodata) / sizeof(IODATA));
	while(p < pterm) {
		iocore_out8(p->port, p->data);
		p++;
	}
}

static void bios_memclear(void) {

	ZeroMemory(mem, 0xa0000);
	ZeroMemory(mem + 0x100000, 0x10000);
	if (CPU_EXTMEM) {
		ZeroMemory(CPU_EXTMEM, CPU_EXTMEMSIZE);
	}
	bios0x18_16(0x20, 0xe1);
	ZeroMemory(mem + VRAM0_B, 0x18000);
	ZeroMemory(mem + VRAM0_E, 0x08000);
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
#if defined(SUPPORT_PC9821)
	ZeroMemory(vramex, sizeof(vramex));
#endif
}

static void bios_reinitbyswitch(void) {

	BYTE	prxcrt;
	BYTE	prxdupd;
	BYTE	biosflag;
	UINT8	boot;

	if (!(np2cfg.dipsw[2] & 0x80)) {
#if defined(CPUCORE_IA32)
		mem[MEMB_SYS_TYPE] = 0x03;		// 80386�`
#else
		mem[MEMB_SYS_TYPE] = 0x01;		// 80286
#endif
	}
	else {
		mem[MEMB_SYS_TYPE] = 0x00;		// V30
	}

	mem[MEMB_BIOS_FLAG0] = 0x01;
	prxcrt = 0x08;
	if (!(np2cfg.dipsw[0] & 0x01)) {			// dipsw1-1 on
		prxcrt |= 0x40;
	}
	if (gdc.display & (1 << GDCDISP_ANALOG)) {
		prxcrt |= 0x04;							// color16
	}
	if (!(np2cfg.dipsw[0] & 0x80)) {			// dipsw1-8 on
		prxcrt |= 0x01;
	}
	if (grcg.chip) {
		prxcrt |= 0x02;
	}
	mem[MEMB_PRXCRT] = prxcrt;

	prxdupd = 0x18;
	if (grcg.chip >= 3) {
		prxdupd |= 0x40;
	}
	if (!(np2cfg.dipsw[1] & 0x80)) {			// dipsw2-8 on
		prxdupd |= 0x20;
	}
	mem[MEMB_PRXDUPD] = prxdupd;

	biosflag = 0x20;
	if (pccore.cpumode & CPUMODE_8MHZ) {
		biosflag |= 0x80;
	}
	biosflag |= mem[0xa3fea] & 7;
	if (np2cfg.dipsw[2] & 0x80) {
		biosflag |= 0x40;
	}
	mem[MEMB_BIOS_FLAG1] = biosflag;
	mem[MEMB_EXPMMSZ] = (BYTE)(pccore.extmem << 3);
	mem[MEMB_CRT_RASTER] = 0x0f;

	// FDD initialize
	SETBIOSMEM32(MEMD_F2DD_POINTER, 0xfd801ad7);
	SETBIOSMEM32(MEMD_F2HD_POINTER, 0xfd801aaf);
	boot = mem[MEMB_MSW5] & 0xf0;
	if (boot != 0x20) {		// 1MB
		fddbios_equip(3, TRUE);
		mem[MEMB_BIOS_FLAG0] |= 0x02;
	}
	else {					// 640KB
		fddbios_equip(0, TRUE);
		mem[MEMB_BIOS_FLAG0] &= ~0x02;
	}
	mem[MEMB_F2DD_MODE] = 0xff;

#if defined(SUPPORT_CRT31KHZ)
	mem[MEMB_CRT_BIOS] = 0x80;
#endif
#if defined(SUPPORT_PC9821)
	mem[0x45c] = 0x40;
#endif

	// FDC
	if (fdc.support144) {
		mem[MEMB_F144_SUP] |= fdc.equip;
	}

	// IDE initialize
	if (pccore.hddif & PCHDD_IDE) {
		mem[MEMB_SYS_TYPE] |= 0x80;		// IDE
		CPU_AX = 0x8300;
		sasibios_operate();
	}
}

static void bios_vectorset(void) {

	UINT	i;

	for (i=0; i<0x20; i++) {
		*(UINT16 *)(mem + (i*4)) = *(UINT16 *)(mem + BIOS_BASE + BIOS_TABLE + (i*2));
		SETBIOSMEM16((i * 4) + 2, BIOS_SEG);
	}
	SETBIOSMEM32(0x1e*4, 0xe8000000);
}

static void bios_screeninit(void) {

	REG8	al;

	al = 4;
	al += (np2cfg.dipsw[1] & 0x04) >> 1;
	al += (np2cfg.dipsw[1] & 0x08) >> 3;
	bios0x18_0a(al);
}


void bios_initialize(void) {

	char	path[MAX_PATH];
	FILEH	fh;
	UINT	i;
	UINT32	tmp;
	UINT	pos;

	biosrom = FALSE;
	getbiospath(path, str_biosrom, sizeof(path));
	fh = file_open_rb(path);
	if (fh != FILEH_INVALID) {
		biosrom = (file_read(fh, mem + 0x0e8000, 0x18000) == 0x18000);
		file_close(fh);
	}
	if (biosrom) {
		TRACEOUT(("load bios.rom"));
		// PnP BIOS��ׂ�
		for (i=0; i<0x10000; i+=0x10) {
			tmp = LOADINTELDWORD(mem + 0xf0000 + i);
			if (tmp == 0x506e5024) {
				TRACEOUT(("found PnP BIOS at %.5x", 0xf0000 + i));
				STOREINTELDWORD(mem + 0xf0000 + i, 0x32504e24);
			}
		}
	}
	else {
		CopyMemory(mem + 0x0e8000, nosyscode, sizeof(nosyscode));
	}

#if defined(SUPPORT_PC9821)
	getbiospath(path, "bios9821.rom", sizeof(path));
	fh = file_open_rb(path);
	if (fh != FILEH_INVALID) {
		if (file_read(fh, mem + 0x0d8000, 0x2000) == 0x2000) {
			// IDE BIOS��ׂ�
			TRACEOUT(("load bios9821.rom"));
			STOREINTELWORD(mem + 0x0d8009, 0);
		}
		file_close(fh);
	}
#endif

#if defined(BIOS_SIMULATE)
	CopyMemory(mem + BIOS_BASE, biosfd80, sizeof(biosfd80));
	if (!biosrom) {
		lio_initialize();
	}

	for (i=0; i<8; i+=2) {
		STOREINTELWORD(mem + 0xfd800 + 0x1aaf + i, 0x1ab7);
		STOREINTELWORD(mem + 0xfd800 + 0x1ad7 + i, 0x1adf);
		STOREINTELWORD(mem + 0xfd800 + 0x2361 + i, 0x1980);
	}
	CopyMemory(mem + 0xfd800 + 0x1ab7, fdfmt2hd, sizeof(fdfmt2hd));
	CopyMemory(mem + 0xfd800 + 0x1adf, fdfmt2dd, sizeof(fdfmt2dd));
	CopyMemory(mem + 0xfd800 + 0x1980, fdfmt144, sizeof(fdfmt144));

	SETBIOSMEM16(0xfffe8, 0xcb90);
	SETBIOSMEM16(0xfffec, 0xcb90);
	mem[0xffff0] = 0xea;
	STOREINTELDWORD(mem + 0xffff1, 0xfd800000);

	if ((!biosrom) && (!(pccore.model & PCMODEL_EPSON))) {
		CopyMemory(mem + 0xe8dd8, neccheck, 0x25);
		pos = LOADINTELWORD(itfrom + 2);
		CopyMemory(mem + 0xf538e, itfrom + pos, 0x27);
	}

	CopyMemory(mem + 0x0fd800 + 0x0e00, keytable[0], 0x300);

	CopyMemory(mem + ITF_ADRS, itfrom, sizeof(itfrom));
	mem[ITF_ADRS + 0x7ff0] = 0xea;
	STOREINTELDWORD(mem + ITF_ADRS + 0x7ff1, 0xf8000000);
	if (pccore.model & PCMODEL_EPSON) {
		mem[ITF_ADRS + 0x7ff1] = 0x04;
	}
	else if ((pccore.model & PCMODELMASK) == PCMODEL_VM) {
		mem[ITF_ADRS + 0x7ff1] = 0x08;
	}
#else
	fh = file_open_c("itf.rom");
	if (fh != FILEH_INVALID) {
		file_read(fh, mem + ITF_ADRS, 0x8000);
		file_close(fh);
		TRACEOUT(("load itf.rom"));
	}
#endif

	CopyMemory(mem + 0x1c0000, mem + ITF_ADRS, 0x08000);
	CopyMemory(mem + 0x1e8000, mem + 0x0e8000, 0x10000);
}


static void bios_itfcall(void) {

	int		i;

	bios_itfprepare();
	bios_memclear();
	bios_vectorset();
	bios0x09_init();
	bios_reinitbyswitch();
	bios0x18_0c();

	if (!np2cfg.ITF_WORK) {
		for (i=0; i<8; i++) {
			mem[MEMB_MSW + (i*4)] = msw_default[i];
		}
		CPU_FLAGL |= C_FLAG;
	}
	else {
		CPU_DX = 0x43d;
		CPU_AL = 0x10;
		mem[0x004f8] = 0xee;		// out	dx, al
		mem[0x004f9] = 0xea;		// call	far
		SETBIOSMEM16(0x004fa, 0x0000);
		SETBIOSMEM16(0x004fc, 0xffff);
		CPU_FLAGL &= ~C_FLAG;
	}
}


UINT MEMCALL biosfunc(UINT32 adrs) {

	UINT16	bootseg;

	if ((CPU_ITFBANK) && (adrs >= 0xf8000) && (adrs < 0x100000)) {
		// for epson ITF
		return(0);
	}

//	TRACEOUT(("biosfunc(%x)", adrs));
#if defined(CPUCORE_IA32) && defined(TRACE)
	if (CPU_STAT_PAGING) {
		UINT32 pde = i286_memoryread_d(CPU_STAT_PDE_BASE);
		if (!(pde & CPU_PDE_PRESENT)) {
			TRACEOUT(("page0: PTE not present"));
		}
		else {
			UINT32 pte = i286_memoryread_d(pde & CPU_PDE_BASEADDR_MASK);
			if (!(pte & CPU_PTE_PRESENT)) {
				TRACEOUT(("page0: not present"));
			}
			else if (pte & CPU_PTE_BASEADDR_MASK) {
				TRACEOUT(("page0: physical address != 0 (pte = %.8x)", pte));
			}
		}
	}
#endif

	switch(adrs) {
		case BIOS_BASE + BIOSOFST_ITF:		// ���Z�b�g
			bios_itfcall();
			return(1);

		case BIOS_BASE + BIOSOFST_INIT:		// �u�[�g
#if 1		// for RanceII
			bios_memclear();
#endif
			bios_vectorset();
#if 1
			bios0x09_init();
#endif
			bios_reinitbyswitch();
			bios_vectorset();
			bios_screeninit();
			if (((pccore.model & PCMODELMASK) >= PCMODEL_VX) &&
				(pccore.sound & 0x7e)) {
				iocore_out8(0x188, 0x27);
				iocore_out8(0x18a, 0x3f);
			}
			return(1);

		case BIOS_BASE + BIOSOFST_09:
			CPU_REMCLOCK -= 500;
			bios0x09();
			return(1);

		case BIOS_BASE + BIOSOFST_0c:
			CPU_REMCLOCK -= 500;
			bios0x0c();
			return(1);

		case BIOS_BASE + BIOSOFST_12:
			CPU_REMCLOCK -= 500;
			bios0x12();
			return(1);

		case BIOS_BASE + BIOSOFST_13:
			CPU_REMCLOCK -= 500;
			bios0x13();
			return(1);

		case BIOS_BASE + BIOSOFST_18:
			CPU_REMCLOCK -= 200;
			bios0x18();
			return(1);

		case BIOS_BASE + BIOSOFST_19:
			CPU_REMCLOCK -= 200;
			bios0x19();
			return(1);

		case BIOS_BASE + BIOSOFST_CMT:
			CPU_REMCLOCK -= 200;
			bios0x1a_cmt();
			return(1);

		case BIOS_BASE + BIOSOFST_PRT:
			CPU_REMCLOCK -= 200;
			bios0x1a_prt();
			return(1);

		case BIOS_BASE + BIOSOFST_1b:
			CPU_STI;
			CPU_REMCLOCK -= 200;
			bios0x1b();
			return(1);

		case BIOS_BASE + BIOSOFST_1c:
			CPU_REMCLOCK -= 200;
			bios0x1c();
			return(1);

		case BIOS_BASE + BIOSOFST_1f:
			CPU_REMCLOCK -= 200;
			bios0x1f();
			return(1);

		case BIOS_BASE + BIOSOFST_WAIT:
			CPU_STI;
			if (fddmtr.busy) {
				CPU_IP--;
				CPU_REMCLOCK = -1;
			}
			else {
				if (fdc.chgreg & 1) {
					if (!(mem[0x0055e] & (0x01 << fdc.us))) {
						CPU_IP--;
						CPU_REMCLOCK -= 1000;
					}
				}
				else {
					if (!(mem[0x0055f] & (0x10 << fdc.us))) {
						CPU_IP--;
						CPU_REMCLOCK -= 1000;
					}
				}
			}
			return(1);

		case 0xfffe8:					// �u�[�g�X�g���b�v���[�h
			CPU_REMCLOCK -= 2000;
			bootseg = bootstrapload();
			if (bootseg) {
				CPU_STI;
				CPU_CS = bootseg;
				CPU_IP = 0x0000;
				return(1);
			}
			return(0);

		case 0xfffec:
			CPU_REMCLOCK -= 2000;
			bootstrapload();
			return(0);
	}

	if ((adrs >= 0xf9a00) && (adrs < 0x0f9a44)) {
		if (!(adrs & 3)) {
			CPU_REMCLOCK -= 500;
			bios_lio((REG8)((adrs - 0xf9a00) >> 2));
		}
		else {
			if (nevent_iswork(NEVENT_GDCSLAVE)) {
				CPU_IP--;
				CPU_REMCLOCK = -1;
				return(1);
			}
		}
		return(0);
	}
	return(0);
}

