#include	"compiler.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"i286.h"
#include	"memory.h"
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
#include	"font.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"sxsi.h"
#include	"calendar.h"
#include	"timing.h"
//#include	"hostdrv.h"
#include	"debugsub.h"


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
				1, 80, 0,
				{"", ""}, ""};

	PCCORE	pc = {	PCBASECLOCK25,
					4,
					4 * PCBASECLOCK25,
					4 * PCBASECLOCK25 * 50 / 3104,
					4 * PCBASECLOCK25 * 5 / 3104,
					4 * PCBASECLOCK25 / 120,
					4 * PCBASECLOCK25 / 1920,
					4 * PCBASECLOCK25 / 3125,
					4 * PCBASECLOCK25 / 56400,
					100, 20, 0, PCMODEL_VX};

									// on=0, off=1
//	BYTE	dip_default[3] = {0x3e, 0x63, 0x7a};
static const BYTE msw_default[8] =
							{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e};

	BYTE	screenupdate = 3;
	int		screendispflag = 1;
	int		soundrenewal = 0;
	BOOL	drawframe;
	UINT	drawcount = 0;

	BYTE	mem[0x200000];


// ---------------------------------------------------------------------------

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
	cnt = (pc.realclock * 5) / 282;
	pc.raster = cnt / maxy;
	pc.hsync = (pc.raster * 4) / 5;
	pc.dispclock = pc.raster * disp;
	pc.vsyncclock = cnt - pc.dispclock;
}

static void setpcclock(UINT base, UINT multiple) {			// ver0.28

	pc.model = PCMODEL_VX;

	if (base >= ((PCBASECLOCK25 + PCBASECLOCK20) / 2)) {
		pc.baseclock = PCBASECLOCK25;			// 2.5MHz
		pc.cpumode = 0;
	}
	else {
		pc.baseclock = PCBASECLOCK20;			// 2.0MHz
		pc.cpumode = CPUMODE_8MHz;
	}
	if (multiple == 0) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	pc.multiple = multiple;
	pc.realclock = pc.baseclock * multiple;
	pc.raster = pc.realclock / 24816;							// ver0.28
	pc.hsync = (pc.raster * 4) / 5;								// ver0.28
	pc.dispclock = pc.realclock * 50 / 3102;
	pc.vsyncclock = pc.realclock * 5 / 3102;
	pc.mouseclock = pc.realclock / 120;
	pc.keyboardclock = pc.realclock / 1920;
	pc.midiclock = pc.realclock / 3125;
	pc.frame1000 = pc.realclock / 56400;
}


// --------------------------------------------------------------------------

static void sound_init(void) {

	UINT	rate;

	rate = np2cfg.samplingrate;
	if ((rate != 11025) && (rate != 22050) && (rate != 44100)) {
		rate = 0;
	}
	sound_create(rate, np2cfg.delayms);
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
}

static void sound_term(void) {

	soundmng_stop();
	rhythm_deinitialize();
	sound_destroy();
}

void pccore_init(void) {

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
}

void pccore_term(void) {

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

//	reset_hostdrv();

	ZeroMemory(mem, 0x10fff0);									// ver0.28
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
	ZeroMemory(mem + FONT_ADRS, 0x08000);

	i286_reset();
	CPUTYPE = 0;
	if (np2cfg.dipsw[2] & 0x80) {
		CPUTYPE = CPUTYPE_V30;
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

	setpcclock(np2cfg.baseclock, np2cfg.multiple);
	sound_changeclock();
	beep_changeclock();
	nevent_init();

	sound_reset();

	iocore_reset();								// サウンドでpicを呼ぶので…
	cbuscore_reset();
	fmboard_reset(np2cfg.SOUND_SW);

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
		I286_CS = 0xf000;
		I286_IP = 0xfff0;
	}
	else {
		for (i=0; i<8; i++) {
			mem[0xa3fe2 + i*4] = msw_default[i];
		}
		CS_BASE = 0xfd800;
		I286_CS = 0xfd80;
		I286_IP = 0x0002;
	}
	i286_resetprefetch();
	sysmng_cpureset();

	soundmng_play();
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

	vramop.tramwait = np2cfg.wait[1];
	vramop.vramwait = np2cfg.wait[3];
	vramop.grcgwait = np2cfg.wait[5];
	gdc_work(GDCWORK_MASTER);
	gdc.vsync = 0x20;
	if (gdc.vsyncint) {
		gdc.vsyncint = 0;
		pic_setirq(2);
	}
	nevent_set(NEVENT_FLAMES, pc.vsyncclock, screendisp, NEVENT_RELATIVE);

	// drawscreenで pc.vsyncclockが変更される可能性があります		// ver0.28
	if (np2cfg.DISPSYNC) {											// ver0.29
		drawscreen();
	}
	(void)item;
}

// ---------------------------------------------------------------------------

void pccore_exec(BOOL draw) {

	drawframe = draw;
	keyext_flash();
	soundmng_sync();
	mouseif_sync();
	pal_eventclear();

	gdc.vsync = 0;
	screendispflag = 1;
	vramop.tramwait = np2cfg.wait[0];
	vramop.vramwait = np2cfg.wait[2];
	vramop.grcgwait = np2cfg.wait[4];
	nevent_set(NEVENT_FLAMES, pc.dispclock, screenvsync, NEVENT_RELATIVE);

//	nevent_get1stevent();

	while(screendispflag) {
		pic_irq();
		if (cpuio.reset_req) {
			cpuio.reset_req = 0;
			I286_CS = 0xf000;
			CS_BASE = 0xf0000;
			I286_IP = 0xfff0;
#ifdef CPU386											// defineを変えてね
			I286_DX = 0x0300;
#endif
			i286_resetprefetch();
		}

#ifndef TRACE
		if (I286_REMCLOCK > 0) {
			if (!(CPUTYPE & CPUTYPE_V30)) {
				i286();
			}
			else {
				v30();
			}
		}
#else
		while(I286_REMCLOCK > 0) {
			i286_step();
		}
#endif
		nevent_progress();
	}
	artic_callback();												// ver0.28
	mpu98ii_callback();
	diskdrv_callback();
	calendar_inc();
	sound_sync();													// happy!
}

