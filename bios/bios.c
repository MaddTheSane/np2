#include	"compiler.h"
#include	"dosio.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"lio.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"fdfmt.h"
#include	"keytable.res"
#include	"itfrom.res"


	BOOL	biosrom = FALSE;

static const char file_biosrom[] = "bios.rom";

static const char neccheck[] = "Copyright (C) 1983 by NEC Corporation";

						// 00/05/18 MS-DOS6.2 on PC-9801VX calling proc
static const BYTE printmain[] = {0x90, 0x5a, 0x1f, 0xcf};

static const BYTE nosyscode[] = {
			0xeb,0x05,0xbe,0x6f,0x00,0xeb,0x03,0xbe,0x47,0x00,0xb8,0x00,
			0x10,0xbb,0x00,0x80,0x8e,0xd0,0x8b,0xe3,0x8c,0xc8,0x8e,0xd8,
			0xb8,0x00,0xa0,0x8e,0xc0,0xb9,0x00,0x10,0xb8,0x20,0x00,0x33,
			0xff,0xfc,0xf3,0xab,0xb0,0xe1,0xb9,0xe0,0x1f,0xaa,0x47,0xe2,
			0xfc,0x2e,0xad,0x8b,0xf8,0x2e,0xad,0x85,0xc0,0x74,0x0a,0xab,
			0x0a,0xe4,0x74,0xf5,0x83,0xc7,0x02,0xeb,0xf0,0xeb,0xfe,0xac,
			0x07,0x05,0x37,0x05,0x39,0x05,0x46,0x05,0x60,0x05,0x47,0x05,
			0x23,0x05,0x39,0x05,0x2f,0x04,0x72,0x05,0x3b,0x05,0x43,0x05,
			0x48,0x04,0x37,0x04,0x46,0x04,0x2f,0x04,0x40,0x04,0x35,0x04,
			0x24,0x00,0x00,0xae,0x07,0x42,0x00,0x41,0x00,0x53,0x00,0x49,
			0x00,0x43,0x00,0x04,0x4e,0x15,0x2f,0x26,0x30,0x04,0x4b,0x04,
			0x4f,0x20,0x00,0x42,0x00,0x49,0x00,0x4f,0x00,0x53,0x00,0x2e,
			0x00,0x52,0x00,0x4f,0x00,0x4d,0x00,0x04,0x2c,0x29,0x2c,0x2d,
			0x57,0x04,0x47,0x04,0x39,0x00,0x00};


static void bios_reinitbyswitch(void) {

	BYTE	prxcrt;
	BYTE	prxdupd;
	BYTE	biosflag;
	BYTE	ext_mem;

	CPUTYPE = 0;
	prxcrt = 0xc8;
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
	if (pc.cpumode & CPUMODE_8MHz) {
		biosflag |= 0x80;
	}
	biosflag |= mem[0xa3fea] & 7;
	if (!(np2cfg.dipsw[2] & 0x80)) {
		ext_mem = np2cfg.EXTMEM;									// ver0.28
	}
	else {
		CPUTYPE = CPUTYPE_V30;
		ext_mem = 0;
		biosflag |= 0x40;
	}
	if (extmem_init(ext_mem)) {										// ver0.28
		ext_mem = 0;							// メモリ確保に失敗
	}
	mem[MEMB_BIOS_FLAG] = biosflag;
	mem[MEMB_EXPMMSZ] = (BYTE)(ext_mem << 3);
	mem[MEMB_CRT_RASTER] = 0x0f;

	gdc.display &= ~4;
	if (!(np2cfg.dipsw[0] & 0x04)) {			// dipsw1-3 on
		gdc.display |= 4;
	}
	gdcs.textdisp |= GDCSCRN_EXT;

	if ((np2cfg.model >= PCMODEL_VX) && (usesound & 0x7e)) {
		iocore_out8(0x188, 0x27);
		iocore_out8(0x18a, 0x3f);
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

	FILEH	fh;
	UINT	i;
	UINT	pos;

	biosrom = FALSE;

	// まぁDISK BASIC動くようになるからいいんじゃないですか？
	// BASIC BIOSは 8086コードのように見えるけど…
	fh = file_open_c(file_biosrom);
	if (fh != FILEH_INVALID) {
		if (file_read(fh, mem + 0x0e8000, 0x18000) == 0x18000) {
			biosrom = TRUE;
		}
		file_close(fh);
	}
	if (!biosrom) {
		CopyMemory(mem + 0x0e8000, nosyscode, sizeof(nosyscode));
	}

	itf.bank = 0;

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
		STOREINTELWORD(mem + 0xfd800 + 0x2361 + i, 0x1980);			// ver0.31
	}
	CopyMemory(mem + 0xfd800 + 0x1ab7, fdfmt2hd, sizeof(fdfmt2hd));
	CopyMemory(mem + 0xfd800 + 0x1adf, fdfmt2dd, sizeof(fdfmt2dd));
	CopyMemory(mem + 0xfd800 + 0x1980, fdfmt144, sizeof(fdfmt144));	// ver0.31

	SETBIOSMEM16(0xfffe8, 0xcb90);
	SETBIOSMEM16(0xfffec, 0xcb90);
	mem[0xffff0] = 0xea;
	STOREINTELDWORD(mem + 0xffff1, 0xfd800000);

#if 1
	CopyMemory(mem + ITF_ADRS, itfrom, sizeof(itfrom));
	mem[ITF_ADRS + 0x7ff0] = 0xea;
	STOREINTELDWORD(mem + ITF_ADRS + 0x7ff1, 0xf8000000);
#if 0
fh = file_create("itf.rom");
if (fh != FILEH_INVALID) {
	file_write(fh, itfrom, sizeof(itfrom));
	file_close(fh);
}
#endif
#else
	fh = file_open_c("itf.rom");
	if (fh != FILEH_INVALID) {
		file_read(fh, &mem[ITF_ADRS], 0x8000);
		file_close(fh);
	}
#endif
	if (!biosrom) {
		CopyMemory(mem + 0xe8dd8, neccheck, 0x25);
		pos = LOADINTELWORD(itfrom + 2);
		CopyMemory(mem + 0xf538e, itfrom + pos, 0x27);
	}

	bios_reinitbyswitch();
	mem[MEMB_CRT_STS_FLAG] = 0x84;
	mem[MEMB_BIOS_FLAG0] = 0x03;						// 00/05/17 beep tone
	mem[MEMB_F2DD_MODE] = 0xff;										// ver0.29
 	SETBIOSMEM16(MEMW_DISK_EQUIP, 0x0003);							// ver0.29
	SETBIOSMEM32(MEMD_F2DD_POINTER, 0xfd801ad7);
	SETBIOSMEM32(MEMD_F2HD_POINTER, 0xfd801aaf);
	mem[0x005ae] |= 0x03;											// ver0.31

	CopyMemory(mem + 0x0fde00, keytable[0], 0x300);
	bios0x09_init();
}

static void bios_boot(void) {

	if (sysport.c & 0x80) {
		if (!(sysport.c & 0x20)) {
			bios_reinitbyswitch();
		}
		I286_CS = 0x0000;
		I286_IP = 0x04f8;
		I286_DS = 0x0000;
		I286_DX = 0x43d;
		I286_AL = 0x10;
		mem[0x004f8] = 0xee;		// out	dx, al
		mem[0x004f9] = 0xea;		// call	far
		SETBIOSMEM32(0x004fa, 0xffff0000);
	}
	else {
		I286_SP = GETBIOSMEM16(0x00404);
		I286_SS = GETBIOSMEM16(0x00406);
	}
}


UINT MEMCALL biosfunc(UINT32 adrs) {

	UINT16	bootseg;

	if ((itf.bank) && (adrs >= 0xf8000) && (adrs < 0x100000)) {
		I286_IP--;
		I286_REMCLOCK = -1;
		return(1);
	}

	switch(adrs) {													// ver0.30
		case BIOS_BASE + BIOSOFST_EOIM:
			I286_REMCLOCK -= 300;
			iocore_out8(0x00, 0x20);
			return(0);

		case BIOS_BASE + BIOSOFST_EOIS:
			iocore_out8(0x08, 0x20);
			if (!pic.pi[1].isr) {
				iocore_out8(0x00, 0x20);
			}
			return(0);

		case BIOS_BASE + BIOSOFST_02:
			I286_REMCLOCK -= 300;
			bios0x02();
			return(1);

		case BIOS_BASE + BIOSOFST_08:
			I286_REMCLOCK -= 300;
			bios0x08();
			return(1);

		case BIOS_BASE + BIOSOFST_09:
			I286_REMCLOCK -= 300;
			bios0x09();
			return(1);

		case BIOS_BASE + BIOSOFST_0c:
			I286_REMCLOCK -= 500;
			bios0x0c();
			return(1);

		case BIOS_BASE + BIOSOFST_12:
			I286_REMCLOCK -= 500;
			bios0x12();
			return(1);

		case BIOS_BASE + BIOSOFST_13:
			I286_REMCLOCK -= 500;
			bios0x13();
			return(1);

		case BIOS_BASE + BIOSOFST_18:
			I286_REMCLOCK -= 200;
			bios0x18();
			return(1);

		case BIOS_BASE + BIOSOFST_19:
			I286_REMCLOCK -= 200;
			bios0x19();
			return(1);

		case BIOS_BASE + BIOSOFST_1a:
			I286_REMCLOCK -= 200;
			bios0x1a();
			return(1);

		case BIOS_BASE + BIOSOFST_1b:
			I286_REMCLOCK -= 200;
			bios0x1b();
			return(1);

		case BIOS_BASE + BIOSOFST_1c:
			I286_REMCLOCK -= 200;
			bios0x1c();
			return(1);

		case BIOS_BASE + BIOSOFST_WAIT:
			I286_STI;
			if (fddmtr_biosbusy) {						// ver0.26
				I286_IP--;
				I286_REMCLOCK = -1;
			}
			else {
				switch(CTRL_FDMEDIA) {
					case DISKTYPE_2HD:
						if (pic.pi[1].isr & PIC_INT42) {
							I286_IP--;
							I286_REMCLOCK -= 1000;
						}
						break;
					case DISKTYPE_2DD:
						if (pic.pi[1].isr & PIC_INT41) {
							I286_IP--;
							I286_REMCLOCK -= 1000;
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
			bios_reinitbyswitch();									// ver0.27
			bios_vectorset();										// ver0.29
			bootseg = bootstrapload();
			I286_STI;
			I286_CS = (bootseg != 0)?bootseg:0xe800;
			I286_DS = 0x0000;
			I286_SS = 0x0030;
			I286_SP = 0x00e6;
			I286_IP = 0x0000;
			return(1);

		case 0xfffe8:					// ブートストラップロード
			I286_REMCLOCK -= 2000;
			bootseg = bootstrapload();
			if (bootseg) {
				I286_STI;
				I286_CS = bootseg;
				I286_IP = 0x0000;
				I286_SS = 0x0030;
				I286_SP = 0x00e6;
				return(1);
			}
			return(0);

		case 0xfffec:
			I286_REMCLOCK -= 2000;
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
	I286_IP--;
	I286_REMCLOCK = -1;
	return(1);
}

