#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"np2ver.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"dispsync.h"
#include	"palettes.h"
#include	"maketext.h"
#include	"maketgrp.h"
#include	"makegrph.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"s98.h"
#include	"font.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"sxsi.h"
#include	"calendar.h"
#include	"timing.h"
#include	"debugsub.h"
#if defined(SUPPORT_HOSTDRV)
#include	"hostdrv.h"
#endif


	const char	np2version[] = NP2VER_CORE;

	NP2CFG	np2cfg = {
				0, 1, 0, 32, 0, 0, 0x40,
				0, 0, 0, 0,
				{0x3e, 0x63, 0x7a}, 0,
				0, 0, {1, 1, 6, 1, 8, 1},

				"VX", PCBASECLOCK25, 4,
				{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e},
				1, 1, 2, 1, 0x000000, 0xffffff,
				22050, 800, 4, 0,
				{0, 0, 0}, 0xd1, 0x7f, 0xd1, 0, 0, 1,
				3, {0x0c, 0x0c, 0x08, 0x06, 0x03, 0x0c}, 64, 64, 64, 64, 64,
				1, 0x82,
				0, {0x17, 0x04, 0x1f}, {0x0c, 0x0c, 0x02, 0x10, 0x3f, 0x3f},
				3, 1, 80, 0, 0,
				{"", ""}, {"", "", "", ""}, "", "", ""};

	PCCORE	pccore = {	PCBASECLOCK25, 4,
						0, PCMODEL_VX, 0, 0,
						0, 0,
						4 * PCBASECLOCK25,
						4 * PCBASECLOCK25 * 50 / 3104,
						4 * PCBASECLOCK25 * 5 / 3104,
						4 * PCBASECLOCK25 / 1920,
						4 * PCBASECLOCK25 / 3125,
						100, 20};

									// on=0, off=1
//	BYTE	dip_default[3] = {0x3e, 0x63, 0x7a};
static const BYTE msw_default[8] =
							{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e};

	BYTE	screenupdate = 3;
	int		screendispflag = 1;
	int		soundrenewal = 0;
	BOOL	drawframe;
	UINT	drawcount = 0;


// ---------------------------------------------------------------------------

void getbiospath(char *path, const char *fname, int maxlen) {

const char	*p;

	p = np2cfg.biospath;
	if (p[0]) {
		file_cpyname(path, p, maxlen);
		file_setseparator(path, maxlen);
		file_catname(path, fname, maxlen);
	}
	else {
		file_cpyname(path, file_getcd(fname), maxlen);
	}
}


// ----

static void setvsyncclock(void) {

	UINT	vfp;
	UINT	vbp;
	UINT	lf;
	UINT	disp;
	UINT	vs;
	UINT	maxy;
	UINT	cnt;

	vfp = gdc.m.para[GDC_SYNC + 5] & 0x3f;
	if (!vfp) {
		vfp = 1;
	}
	vbp = gdc.m.para[GDC_SYNC + 7] >> 2;
	if (!vbp) {
		vbp = 1;
	}
	lf = LOADINTELWORD(gdc.m.para + GDC_SYNC + 6);
	lf &= 0x3ff;
	if (!lf) {
		lf = 1024;
	}
	disp = vfp + vbp + lf;
	vs = LOADINTELWORD(gdc.m.para + GDC_SYNC + 4);
	vs = (vs >> 5) & 0x1f;
	if (!vs) {
		vs = 1;
	}
	maxy = disp + vs;
	cnt = (pccore.realclock * 5) / 282;
	pccore.raster = cnt / maxy;
	pccore.hsync = (pccore.raster * 4) / 5;
	pccore.dispclock = pccore.raster * disp;
	pccore.vsyncclock = cnt - pccore.dispclock;
}

static void pccore_set(void) {

	UINT8	model;
	UINT32	multiple;

	ZeroMemory(&pccore, sizeof(pccore));
	model = PCMODEL_VX;
	if (!milstr_cmp(np2cfg.model, str_VM)) {
		model = PCMODEL_VM;
	}
	else if (!milstr_cmp(np2cfg.model, str_EPSON)) {
		model = PCMODEL_EPSON | PCMODEL_VM;
	}
	pccore.model = model;

	if (np2cfg.baseclock >= ((PCBASECLOCK25 + PCBASECLOCK20) / 2)) {
		pccore.baseclock = PCBASECLOCK25;			// 2.5MHz
		pccore.cpumode = 0;
	}
	else {
		pccore.baseclock = PCBASECLOCK20;			// 2.0MHz
		pccore.cpumode = CPUMODE_8MHz;
	}
	multiple = np2cfg.multiple;
	if (multiple == 0) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	pccore.multiple = multiple;
	pccore.realclock = pccore.baseclock * multiple;
	pccore.raster = pccore.realclock / 24816;
	pccore.hsync = (pccore.raster * 4) / 5;
	pccore.dispclock = pccore.realclock * 50 / 3102;
	pccore.vsyncclock = pccore.realclock * 5 / 3102;
	pccore.keyboardclock = pccore.realclock / 1920;
	pccore.midiclock = pccore.realclock / 3125;

	// 拡張メモリ
	pccore.extmem = 0;

	// HDDの接続 (I/Oの使用状態が変わるので..
//	if (np2cfg.dipsw[1] & 0x20) {
		pccore.hddif |= PCHDD_IDE;
//	}
	pccore.hddif |= PCHDD_SCSI;

	// サウンドボードの接続
	pccore.sound = np2cfg.SOUND_SW;

	// その他CBUSの接続
	pccore.device = 0;
	if (np2cfg.pc9861enable) {
		pccore.device |= PCCBUS_PC9861K;
	}
	if (np2cfg.mpuenable) {
		pccore.device |= PCCBUS_MPU98;
	}
}


// --------------------------------------------------------------------------

static void sound_init(void) {

	UINT	rate;

	rate = np2cfg.samplingrate;
	if ((rate != 11025) && (rate != 22050) && (rate != 44100)) {
		rate = 0;
	}
	sound_create(rate, np2cfg.delayms);
#if defined(SUPPORT_WAVEMIX)
	wavemix_initialize(rate);
#endif
	beep_initialize(rate);
	beep_setvol(np2cfg.BEEP_VOL);
	tms3631_initialize(rate);
	tms3631_setvol(np2cfg.vol14);
	opngen_initialize(rate);
	opngen_setvol(np2cfg.vol_fm);
	psggen_initialize(rate);
	psggen_setvol(np2cfg.vol_ssg);
	rhythm_initialize(rate);
	rhythm_setvol(np2cfg.vol_rhythm);
	adpcm_initialize(rate);
	adpcm_setvol(np2cfg.vol_adpcm);
	pcm86gen_initialize(rate);
	pcm86gen_setvol(np2cfg.vol_pcm);
	cs4231_initialize(rate);
}

static void sound_term(void) {

	soundmng_stop();
#if defined(SUPPORT_WAVEMIX)
	wavemix_deinitialize();
#endif
	rhythm_deinitialize();
	sound_destroy();
}

void pccore_init(void) {

	CPU_INITIALIZE();

	pal_initlcdtable();
	pal_makelcdpal();
	pal_makeskiptable();
	dispsync_init();
	sxsi_initialize();

	font_init();
	font_load(np2cfg.fontfile, TRUE);
	maketext_init();
	makegrph_init();
	gdcsub_init();
	fddfile_init();

	sound_init();

	mpu98ii_construct();
	rs232c_construct();
	pc9861k_construct();

	iocore_create();

#if defined(SUPPORT_HOSTDRV)
	hostdrv_initialize();
#endif
}

void pccore_term(void) {

#if defined(SUPPORT_HOSTDRV)
	hostdrv_deinitialize();
#endif

	sound_term();

	fdd_eject(0);
	fdd_eject(1);
	fdd_eject(2);
	fdd_eject(3);

	extmemmng_clear();

	iocore_destroy();

	pc9861k_destruct();
	rs232c_destruct();
	mpu98ii_destruct();

	sxsi_trash();
}


void pccore_cfgupdate(void) {

	BOOL	renewal;
	int		i;

	renewal = FALSE;
	for (i=0; i<8; i++) {
		if (np2cfg.memsw[i] != mem[MEMB_MSW + i*4]) {
			np2cfg.memsw[i] = mem[MEMB_MSW + i*4];
			renewal = TRUE;
		}
	}
	if (renewal) {
		sysmng_update(SYS_UPDATECFG);
	}
}

void pccore_reset(void) {

	int		i;

	ZeroMemory(mem, 0x10fff0);									// ver0.28
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
	ZeroMemory(mem + FONT_ADRS, 0x08000);

	CPU_RESET();
	CPU_TYPE = 0;
	if (np2cfg.dipsw[2] & 0x80) {
		CPU_TYPE = CPUTYPE_V30;
	}

	//メモリスイッチ
	for (i=0; i<8; i++) {
		mem[0xa3fe2 + i*4] = np2cfg.memsw[i];
	}

	fddfile_reset2dmode();
	bios0x18_16(0x20, 0xe1);

	soundmng_stop();
	if (soundrenewal) {
		soundrenewal = 0;
		sound_term();
		sound_init();
	}

	pccore_set();

	sound_changeclock();
	beep_changeclock();
	nevent_init();

	sound_reset();
#if defined(SUPPORT_WAVEMIX)
	wavemix_bind();
#endif

	if (pccore.model & PCMODEL_EPSON) {			// RAM ctrl
		CPU_RAM_D000 = 0xffff;
	}

	iocore_reset();								// サウンドでpicを呼ぶので…
	cbuscore_reset();
	fmboard_reset(np2cfg.SOUND_SW);

	i286_memorymap((pccore.model & PCMODEL_EPSON)?1:0);
	iocore_build();
	iocore_bind();
	cbuscore_bind();
	fmboard_bind();

	timing_reset();
	fddmtr_init();
	calendar_init();
	vram_init();

	pal_change(1);

	bios_init();
	sxsi_open();

	if (np2cfg.ITF_WORK) {
		CS_BASE = 0xf0000;
		CPU_CS = 0xf000;
		CPU_IP = 0xfff0;
	}
	else {
		for (i=0; i<8; i++) {
			mem[0xa3fe2 + i*4] = msw_default[i];
		}
		CS_BASE = 0xfd800;
		CPU_CS = 0xfd80;
		CPU_IP = 0x0002;
	}
	CPU_CLEARPREFETCH();
	sysmng_cpureset();

	soundmng_play();

#if defined(SUPPORT_HOSTDRV)
	hostdrv_reset();
#endif
}

static void drawscreen(void) {

	BYTE	timing;

	tramflag.timing++;
	timing = ((LOADINTELWORD(gdc.m.para + GDC_CSRFORM + 1)) >> 5) & 0x3e;
	if (!timing) {
		timing = 0x40;
	}
	if (tramflag.timing >= timing) {
		tramflag.timing = 0;
		tramflag.count++;
		tramflag.renewal |= (tramflag.count ^ 2) & 2;
		tramflag.renewal |= 1;
	}

	if ((gdcs.textdisp & GDCSCRN_EXT) ||						// ver0.28
		(gdcs.grphdisp & GDCSCRN_EXT)) {
		setvsyncclock();
	}

	if (drawframe) {
		if ((gdcs.textdisp & GDCSCRN_EXT) ||					// ver0.26
			(gdcs.grphdisp & GDCSCRN_EXT)) {
			if (dispsync_renewalvertical()) {
				gdcs.textdisp |= GDCSCRN_ALLDRAW2;
				gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			}
		}
																// ver0.28/pr4
		if (gdcs.textdisp & GDCSCRN_EXT) {
			gdcs.textdisp &= ~GDCSCRN_EXT;
			dispsync_renewalhorizontal();
			tramflag.renewal |= 1;
			if (dispsync_renewalmode()) {
				screenupdate |= 2;
			}
		}
																// ver0.28/pr4
		if (gdcs.palchange) {									// grphを先に
			gdcs.palchange = 0;
			pal_change(0);
			screenupdate |= 1;
		}
		if (gdcs.grphdisp & GDCSCRN_EXT) {
			gdcs.grphdisp &= ~GDCSCRN_EXT;
			if (((gdc.clock & 0x80) && (gdc.clock != 0x83)) ||
				(gdc.clock == 0x03)) {
				gdc.clock ^= 0x80;
				gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			}
		}
		if (gdcs.grphdisp & GDCSCRN_ENABLE) {
			if (!(gdc.mode1 & 2)) {
				if (!gdcs.disp) {
					if (gdcs.grphdisp & GDCSCRN_MAKE) {
						makegrph(0, gdcs.grphdisp & GDCSCRN_ALLDRAW);
						gdcs.grphdisp &= ~GDCSCRN_MAKE;
						screenupdate |= 1;
					}
				}
				else {
					if (gdcs.grphdisp & (GDCSCRN_MAKE << 1)) {
						makegrph(1, gdcs.grphdisp & (GDCSCRN_ALLDRAW << 1));
						gdcs.grphdisp &= ~(GDCSCRN_MAKE << 1);
						screenupdate |= 1;
					}
				}
			}
			else if (gdcs.textdisp & GDCSCRN_ENABLE) {
				if (!gdcs.disp) {
					if ((gdcs.grphdisp & GDCSCRN_MAKE) ||
						(gdcs.textdisp & GDCSCRN_MAKE)) {
						if (!(gdc.mode1 & 0x4)) {
							maketextgrph(0, gdcs.textdisp & GDCSCRN_ALLDRAW,
									gdcs.grphdisp & GDCSCRN_ALLDRAW);
						}
						else {
							maketextgrph40(0, gdcs.textdisp & GDCSCRN_ALLDRAW,
									gdcs.grphdisp & GDCSCRN_ALLDRAW);
						}
						gdcs.grphdisp &= ~GDCSCRN_MAKE;
						screenupdate |= 1;
					}
				}
				else {
					if ((gdcs.grphdisp & (GDCSCRN_MAKE << 1)) ||
						(gdcs.textdisp & GDCSCRN_MAKE)) {
						if (!(gdc.mode1 & 0x4)) {
							maketextgrph(1, gdcs.textdisp & GDCSCRN_ALLDRAW,
									gdcs.grphdisp & (GDCSCRN_ALLDRAW << 1));
						}
						else {
							maketextgrph40(1, gdcs.textdisp & GDCSCRN_ALLDRAW,
									gdcs.grphdisp & (GDCSCRN_ALLDRAW << 1));
						}
						gdcs.grphdisp &= ~(GDCSCRN_MAKE << 1);
						screenupdate |= 1;
					}
				}
			}
		}

		if (gdcs.textdisp & GDCSCRN_ENABLE) {
			if (tramflag.renewal) {
				gdcs.textdisp |= maketext_curblink();
			}
			if ((cgwindow.writable & 0x80) && (tramflag.gaiji)) {
				gdcs.textdisp |= GDCSCRN_ALLDRAW;
			}
			cgwindow.writable &= ~0x80;
			if (gdcs.textdisp & GDCSCRN_MAKE) {
				if (!(gdc.mode1 & 0x4)) {
					maketext(gdcs.textdisp & GDCSCRN_ALLDRAW);
				}
				else {
					maketext40(gdcs.textdisp & GDCSCRN_ALLDRAW);
				}
				gdcs.textdisp &= ~GDCSCRN_MAKE;
				screenupdate |= 1;
			}
		}
		if (screenupdate) {
			screenupdate = scrndraw_draw((BYTE)(screenupdate & 2));
			drawcount++;
		}
	}
}

void screendisp(NEVENTITEM item) {

	PICITEM		pi;

	gdc_work(GDCWORK_SLAVE);
	gdc.vsync = 0;
	screendispflag = 0;
	if (!np2cfg.DISPSYNC) {											// ver0.29
		drawscreen();
	}
	pi = &pic.pi[0];
	if (pi->irr & PIC_CRTV) {
		pi->irr &= ~PIC_CRTV;
		gdc.vsyncint = 1;
	}
	(void)item;
}

void screenvsync(NEVENTITEM item) {

	MEMWAIT_TRAM = np2cfg.wait[1];
	MEMWAIT_VRAM = np2cfg.wait[3];
	MEMWAIT_GRCG = np2cfg.wait[5];
	gdc_work(GDCWORK_MASTER);
	gdc.vsync = 0x20;
	if (gdc.vsyncint) {
		gdc.vsyncint = 0;
		pic_setirq(2);
	}
	nevent_set(NEVENT_FLAMES, pccore.vsyncclock, screendisp, NEVENT_RELATIVE);

	// drawscreenで pccore.vsyncclockが変更される可能性があります
	if (np2cfg.DISPSYNC) {											// ver0.29
		drawscreen();
	}
	(void)item;
}


// ---------------------------------------------------------------------------

// #define	IPTRACE			(1 << 16)

#if IPTRACE
static UINT		trpos;
static UINT32	treip[IPTRACE];

void iptrace_out(void) {

	FILEH	fh;
	UINT	s;
	UINT32	eip;
	char	buf[32];

	s = trpos;
	if (s > IPTRACE) {
		s -= IPTRACE;
	}
	else {
		s = 0;
	}
	fh = file_create_c("his.txt");
	while(s < trpos) {
		eip = treip[s & (IPTRACE - 1)];
		s++;
		SPRINTF(buf, "%.4x:%.4x\r\n", (eip >> 16), eip & 0xffff);
		file_write(fh, buf, strlen(buf));
	}
	file_close(fh);
}
#endif


#if defined(TRACE)
static int resetcnt = 0;
static int execcnt = 0;
int piccnt = 0;
int tr = 0;
#endif

	UINT	cflg;

void pccore_exec(BOOL draw) {

	drawframe = draw;
	keyext_flash();
	soundmng_sync();
	mouseif_sync();
	pal_eventclear();

	gdc.vsync = 0;
	screendispflag = 1;
	MEMWAIT_TRAM = np2cfg.wait[0];
	MEMWAIT_VRAM = np2cfg.wait[2];
	MEMWAIT_GRCG = np2cfg.wait[4];
	nevent_set(NEVENT_FLAMES, pccore.dispclock, screenvsync, NEVENT_RELATIVE);

//	nevent_get1stevent();

	while(screendispflag) {
#if defined(TRACE)
	resetcnt++;
#endif
		pic_irq();
		if (CPU_RESETREQ) {
			CPU_RESETREQ = 0;
#if 1
			CPU_SHUT();
#else
			CPU_CS = 0xf000;
			CS_BASE = 0xf0000;
			CPU_IP = 0xfff0;
#if defined(CPUCORE_IA32)
			CPU_DX = 0x0300;
#endif
			CPU_CLEARPREFETCH();
#endif
		}

#if 1 // ndef TRACE
		if (CPU_REMCLOCK > 0) {
			if (!(CPU_TYPE & CPUTYPE_V30)) {
				CPU_EXEC();
			}
			else {
				CPU_EXECV30();
			}
		}
#else
		while(CPU_REMCLOCK > 0) {
#if IPTRACE
			treip[trpos & (IPTRACE - 1)] = (CPU_CS << 16) + CPU_IP;
			trpos++;
#endif
			if (tr) {
				TRACEOUT(("%.4x:%.4x", CPU_CS, CPU_IP));
			}
#if 0
			if ((tr & 2) && (mem[0x0471e] == '\\')) {
				TRACEOUT(("DTA BREAK %.4x:%.4x", CPU_CS, CPU_IP));
				TRACEOUT(("0471:000e %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x",
	mem[0x0471e+0], mem[0x0471e+1], mem[0x0471e+2], mem[0x0471e+3],
	mem[0x0471e+4], mem[0x0471e+5], mem[0x0471e+6], mem[0x0471e+7]));
				tr -= 2;
			}
			// DOS6
			if (CPU_CS == 0xffd0) {
				if (CPU_IP == 0xc4c2) {
					TRACEOUT(("DS:DX = %.4x:%.4x / CX = %.4x", CPU_DS, CPU_DX, CPU_CX));
				}
				else if (CPU_IP == 0xc21d) {
					TRACEOUT(("-> DS:BX = %.4x:%.4x", CPU_DS, CPU_BX));
				}
			}
#endif
//			i286x_step();
			i286c_step();
		}
#endif
		nevent_progress();
	}
	artic_callback();
	mpu98ii_callback();
	diskdrv_callback();
	calendar_inc();
	S98_sync();
	sound_sync();													// happy!

#if defined(TRACE)
	execcnt++;
	if (execcnt >= 60) {
//		TRACEOUT(("resetcnt = %d / pic %d", resetcnt, piccnt));
		execcnt = 0;
		resetcnt = 0;
		piccnt = 0;
	}
#endif
}

