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
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"fdfmt.h"
#include	"keytable.res"
#include	"itfrom.res"
#include	"startup.res"
#include	"biosboot.res"


#define	BIOS_SIMULATE

	BOOL	biosrom = FALSE;

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

	// IDE initialize
	if (pccore.hddif & PCHDD_IDE) {
		mem[MEMB_SYS_TYPE] |= 0x80;		// IDE
		CPU_AX = 0x8300;
		sasibios_operate();
	}
}

static void bios_memclear(void) {

	ZeroMemory(mem, 0xa0000);
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
}

static void bios_vectorset(void) {

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

	REG8	al;

	al = 4;
	al += (np2cfg.dipsw[1] & 0x04) >> 1;
	al += (np2cfg.dipsw[1] & 0x08) >> 3;
	bios0x18_0a(al);
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
			BIOSOFST_1c,	BIOSOFST_IRET,	BIOSOFST_1e,	BIOSOFST_1f};


void bios_initialize(void) {

	char	path[MAX_PATH];
	FILEH	fh;
	UINT	i;
	UINT	pos;

	biosrom = FALSE;

	// まぁDISK BASIC動くようになるからいいんじゃないですか？
	// BASIC BIOSは 8086コードのように見えるけど…
	getbiospath(path, str_biosrom, sizeof(path));
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
	for (i=(BIOS_BASE+BIOSOFST_EOIM); i<=(BIOS_BASE+BIOSOFST_1f); i+=2) {
		SETBIOSMEM16(i, 0xcf90);
	}
	CopyMemory(mem + BIOS_BASE + BIOSOFST_PRT, printmain, sizeof(printmain));

//	bios_vectorset();
	if (!biosrom) {
		lio_initialize();
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

//	bios_reinitbyswitch();
//	mem[MEMB_CRT_STS_FLAG] = 0x84;		// -> bios_screeninit()
//	mem[MEMB_BIOS_FLAG0] = 0x03;
//	mem[MEMB_F2DD_MODE] = 0xff;
// 	SETBIOSMEM16(MEMW_DISK_EQUIP, 0x0003);
	mem[0x005ae] |= 0x03;											// ver0.31

	CopyMemory(mem + 0x0fde00, keytable[0], 0x300);
//	bios0x09_init();

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

	if (!(sysport.c & 0x80)) {
		CPU_SP = GETBIOSMEM16(0x00404);
		CPU_SS = GETBIOSMEM16(0x00406);
//		TRACEOUT(("CPU Reset... SS:SP = %.4x:%.4x", CPU_SS, CPU_SP));
	}
	else {
		bios_memclear();
		bios_vectorset();
		bios0x09_init();
		bios_reinitbyswitch();

		if (sysport.c & 0x20) {
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
			CPU_IP = 0x0002;
		}
	}
}

// テスト(こんなんじゃだめぽ
static void bios0x1f(void) {

	BYTE	work[256];
	UINT32	src;
	UINT32	dst;
	UINT	leng;
	UINT	l;

	if (CPU_AH == 0x90) {
		i286_memstr_read(CPU_ES, CPU_BX + 0x10, work, 0x10);
		src = work[2] + (work[3] << 8) + (work[4] << 16) + CPU_SI;
		dst = work[10] + (work[11] << 8) + (work[12] << 16) + CPU_DI;
		leng = LOW16(CPU_CX - 1) + 1;
		TRACEOUT(("protect bios: memmove"));
		TRACEOUT(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x",
				work[0], work[1], work[2], work[3],
				work[4], work[5], work[6], work[7]));
		TRACEOUT(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x",
				work[8], work[9], work[10], work[11],
				work[12], work[13], work[14], work[15]));
		TRACEOUT(("SI=%.4x DI=%.4x CX=%.4x", CPU_SI, CPU_DI, CPU_CX));
		TRACEOUT(("src:%08x dst:%08x leng:%08x", src, dst, leng));
		do {
			l = min(leng, 256);
			i286_memx_read(src, work, l);
			i286_memx_write(dst, work, l);
			src += l;
			dst += l;
			leng -= l;
		} while(leng);
	}
	else {
		TRACEOUT(("unsupport protect bios AH=%.2x", CPU_AH));
	}
}

UINT MEMCALL biosfunc(UINT32 adrs) {

	UINT16	bootseg;

	if ((CPU_ITFBANK) && (adrs >= 0xf8000) && (adrs < 0x100000)) {
		// for epson ITF
		return(0);
	}

	switch(adrs) {
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

	if (biosrom) {
		return(0);
	}
	CPU_IP--;
	CPU_REMCLOCK = -1;
	return(1);
}

