#include	"compiler.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"sxsibios.h"
#include	"lio.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"fdfmt.h"
#include	"keytable.res"
#include	"itfrom.res"
#include	"startup.res"
#include	"biosboot.res"


#define	BIOS_SIMULATE

	BOOL	biosrom = FALSE;

static const char file_biosrom[] = "bios.rom";

static const char neccheck[] = "Copyright (C) 1983 by NEC Corporation";

						// 00/05/18 MS-DOS6.2 on PC-9801VX calling proc
static const BYTE printmain[] = {0x90, 0x5a, 0x1f, 0xcf};


static void bios_reinitbyswitch(void) {

	BYTE	prxcrt;
	BYTE	prxdupd;
	BYTE	biosflag;
	UINT8	boot;

#if defined(CPUCORE_IA32)
	UINT16	org_cs;
	UINT16	org_ip;

	org_cs = CPU_CS;
	org_ip = CPU_IP;
	CPU_SHUT();
	CPU_CS = org_cs;
	CPU_IP = org_ip;
	SETBIOSMEM16(0x00486, CPU_DX);
#endif

	if (!(np2cfg.dipsw[2] & 0x80)) {
#if defined(CPUCORE_IA32)
		mem[MEMB_SYS_TYPE] = 0x03;		// 80386～
#else
		mem[MEMB_SYS_TYPE] = 0x01;		// 80286
#endif
	}
	else {
		mem[MEMB_SYS_TYPE] = 0x00;		// V30
	}

	mem[MEMB_BIOS_FLAG0] = 0x01;
	prxcrt = 0x48;								// ver0.74
	if (gdc.display & 2) {
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
	if (pccore.cpumode & CPUMODE_8MHz) {
		biosflag |= 0x80;
	}
	biosflag |= mem[0xa3fea] & 7;
	if (np2cfg.dipsw[2] & 0x80) {
		biosflag |= 0x40;
	}
	mem[MEMB_BIOS_FLAG1] = biosflag;
	mem[MEMB_EXPMMSZ] = (BYTE)(pccore.extmem << 3);
	mem[MEMB_CRT_RASTER] = 0x0f;

	gdc.display &= ~4;
	if (!(np2cfg.dipsw[0] & 0x04)) {			// dipsw1-3 on
		gdc.display |= 4;
	}
	gdcs.textdisp |= GDCSCRN_EXT;

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

	// IDE initialize
	if (pccore.hddif & PCHDD_IDE) {
		mem[MEMB_SYS_TYPE] |= 0x80;		// IDE
		CPU_AX = 0x8300;
		sasibios_operate();
	}
}

static void bios_vectorset(void) {									// ver0.30

	UINT	i;

	for (i=0; i<0x400; i+=4) {
		SETBIOSMEM16(i+0, BIOSOFST_IRET);
		SETBIOSMEM16(i+2, BIOS_SEG);
	}
	for (i=0; i<0x20; i++) {
		*(UINT16 *)(mem + (i*4)) = *(UINT16 *)(mem + 0xfd868 + (i*2));
	}
	SETBIOSMEM32(0x1e*4, 0xe8000000);
}

static void bios_screeninit(void) {

	UINT	ax;

	ax = 0x0a04;
	ax += (np2cfg.dipsw[1] & 0x04) >> 1;
	ax += (np2cfg.dipsw[1] & 0x08) >> 3;
	CPU_AX = ax;
	bios0x18();
}


// CDSで見てる為、変更…(涙
static const UINT16 biosoffset[0x20] = {
			BIOSOFST_IRET,	BIOSOFST_IRET,	BIOSOFST_02,	BIOSOFST_IRET,
			BIOSOFST_IRET,	BIOSOFST_IRET,	BIOSOFST_IRET,	BIOSOFST_IRET,

			BIOSOFST_08,	BIOSOFST_09,	BIOSOFST_EOIM,	BIOSOFST_EOIM,
			BIOSOFST_0c,	BIOSOFST_EOIM,	BIOSOFST_EOIM,	BIOSOFST_EOIM,

			BIOSOFST_EOIS,	BIOSOFST_EOIS,	BIOSOFST_12,	BIOSOFST_13,
			BIOSOFST_EOIS,	BIOSOFST_EOIS,	BIOSOFST_EOIS,	BIOSOFST_EOIS,

			BIOSOFST_18,	BIOSOFST_19,	BIOSOFST_1a,	BIOSOFST_1b,
			BIOSOFST_1c,	BIOSOFST_IRET,	BIOSOFST_1e,	BIOSOFST_IRET};


void bios_init(void) {

	char	path[MAX_PATH];
	FILEH	fh;
	UINT	i;
	UINT	pos;

	biosrom = FALSE;

	// まぁDISK BASIC動くようになるからいいんじゃないですか？
	// BASIC BIOSは 8086コードのように見えるけど…
	getbiospath(path, file_biosrom, sizeof(path));
	fh = file_open_rb(path);
	if (fh != FILEH_INVALID) {
		if (file_read(fh, mem + 0x0e8000, 0x18000) == 0x18000) {
			TRACEOUT(("load bios.rom"));
			biosrom = TRUE;
		}
		file_close(fh);
	}
	if (!biosrom) {
		CopyMemory(mem + 0x0e8000, nosyscode, sizeof(nosyscode));
	}

#if defined(BIOS_SIMULATE)
	// BIOS hookのアドレス変更
	for (i=0; i<0x20; i++) {
		STOREINTELWORD(mem + 0xfd868 + i*2, biosoffset[i]);
	}
	SETBIOSMEM16(BIOS_BASE + BIOSOFST_IRET, 0x50cf);
	SETBIOSMEM16(BIOS_BASE + BIOSOFST_WAIT, 0xcf90);
	for (i=(BIOS_BASE+BIOSOFST_EOIM); i<=(BIOS_BASE+BIOSOFST_1c); i+=2) {
		SETBIOSMEM16(i, 0xcf90);
	}
	CopyMemory(mem + BIOS_BASE + BIOSOFST_PRT, printmain, sizeof(printmain));

	bios_vectorset();												// ver0.29
	if (!biosrom) {
		lio_init();
	}

	SETBIOSMEM16(0xfd800, 0xcb90);
	SETBIOSMEM16(0xfd802, 0xcb90);
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

	bios_reinitbyswitch();
	mem[MEMB_CRT_STS_FLAG] = 0x84;
//	mem[MEMB_BIOS_FLAG0] = 0x03;
//	mem[MEMB_F2DD_MODE] = 0xff;										// ver0.29
// 	SETBIOSMEM16(MEMW_DISK_EQUIP, 0x0003);							// ver0.29
	mem[0x005ae] |= 0x03;											// ver0.31

	CopyMemory(mem + 0x0fde00, keytable[0], 0x300);
	bios0x09_init();

	CopyMemory(mem + ITF_ADRS, itfrom, sizeof(itfrom));
	mem[ITF_ADRS + 0x7ff0] = 0xea;
	STOREINTELDWORD(mem + ITF_ADRS + 0x7ff1, 0xf8000000);
	if (pccore.model & PCMODEL_EPSON) {
		mem[ITF_ADRS + 0x7ff1] = 0x04;
	}
	else if ((pccore.model & PCMODELMASK) == PCMODEL_VM) {
		mem[ITF_ADRS + 0x7ff1] = 0x08;
	}

	CopyMemory(mem + 0xfd800 + 0x2400, biosboot, sizeof(biosboot));
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

static void bios_boot(void) {

	if (sysport.c & 0x80) {
		if (!(sysport.c & 0x20)) {
			bios_reinitbyswitch();
		}
		CPU_CS = 0x0000;
		CPU_IP = 0x04f8;
		CPU_DS = 0x0000;
		CPU_DX = 0x43d;
		CPU_AL = 0x10;
		mem[0x004f8] = 0xee;		// out	dx, al
		mem[0x004f9] = 0xea;		// call	far
		SETBIOSMEM16(0x004fa, 0x0000);
		SETBIOSMEM16(0x004fc, 0xffff);
	}
	else {
		CPU_SP = GETBIOSMEM16(0x00404);
		CPU_SS = GETBIOSMEM16(0x00406);
//		TRACEOUT(("CPU Reset... SS:SP = %.4x:%.4x", CPU_SS, CPU_SP));
	}
}


UINT MEMCALL biosfunc(UINT32 adrs) {

	UINT16	bootseg;

	if ((CPU_ITFBANK) && (adrs >= 0xf8000) && (adrs < 0x100000)) {
#if 1					// for epson ITF
		return(0);
#else
		CPU_IP--;
		CPU_REMCLOCK = -1;
		return(1);
#endif
	}

	switch(adrs) {													// ver0.30
		case BIOS_BASE + BIOSOFST_EOIM:
			CPU_REMCLOCK -= 300;
			iocore_out8(0x00, 0x20);
			return(0);

		case BIOS_BASE + BIOSOFST_EOIS:
			iocore_out8(0x08, 0x20);
			if (!pic.pi[1].isr) {
				iocore_out8(0x00, 0x20);
			}
			return(0);

		case BIOS_BASE + BIOSOFST_02:
			CPU_REMCLOCK -= 300;
			bios0x02();
			return(1);

		case BIOS_BASE + BIOSOFST_08:
			CPU_REMCLOCK -= 300;
			bios0x08();
			return(1);

		case BIOS_BASE + BIOSOFST_09:
			CPU_REMCLOCK -= 300;
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

		case BIOS_BASE + BIOSOFST_1a:
			CPU_REMCLOCK -= 200;
			bios0x1a();
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

		case BIOS_BASE + BIOSOFST_WAIT:
			CPU_STI;
			if (fddmtr_biosbusy) {						// ver0.26
				CPU_IP--;
				CPU_REMCLOCK = -1;
			}
			else {
				switch(CTRL_FDMEDIA) {
					case DISKTYPE_2HD:
						if (pic.pi[1].isr & PIC_INT42) {
							CPU_IP--;
							CPU_REMCLOCK -= 1000;
						}
						break;
					case DISKTYPE_2DD:
						if (pic.pi[1].isr & PIC_INT41) {
							CPU_IP--;
							CPU_REMCLOCK -= 1000;
						}
						break;
				}
			}
			return(1);

		case BIOS_BASE + BIOSOFST_PRT:
			bios0x1a_main();
			return(0);

		case 0xfd800:					// リセット
			bios_boot();
			return(1);

		case 0xfd802:					// ブート
			bios_reinitbyswitch();
			bios_vectorset();
			bios_screeninit();
			if (((pccore.model & PCMODELMASK) >= PCMODEL_VX) &&
				(pccore.sound & 0x7e)) {
				iocore_out8(0x188, 0x27);
				iocore_out8(0x18a, 0x3f);
			}

#if 1																// ver0.73
			CPU_CS = 0xfd80;			// SASI/SCSIリセット
			CPU_IP = 0x2400;
#else
			bootseg = bootstrapload();
			CPU_STI;
			CPU_CS = (bootseg != 0)?bootseg:0xe800;
			CPU_DS = 0x0000;
			CPU_SS = 0x0030;
			CPU_SP = 0x00e6;
			CPU_IP = 0x0000;
#endif
			return(1);

		case 0xfffe8:					// ブートストラップロード
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

	if ((adrs >= 0xf9a00) && (adrs < 0x0f9a24)) {
		if (!(adrs & 1)) {
			bios_lio((BYTE)((adrs - 0xf9a00) >> 1));
		}
		return(0);
	}

	if (biosrom) {
		return(0);
	}
	CPU_IP--;
	CPU_REMCLOCK = -1;
	return(1);
}

