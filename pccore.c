#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"gdc_sub.h"
#include	"cbuscore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"amd98.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"dispsync.h"
#include	"palettes.h"
#include	"maketext.h"
#include	"maketgrp.h"
#include	"makegrph.h"
#include	"makegrex.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"s98.h"
#include	"font.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"sxsi.h"
#if defined(SUPPORT_HOSTDRV)
#include	"hostdrv.h"
#endif
#include	"np2ver.h"
#include	"calendar.h"
#include	"timing.h"
#include	"keystat.h"
#include	"debugsub.h"


	const char	np2version[] = NP2VER_CORE;

	NP2CFG	np2cfg = {
				0, 1, 0, 32, 0, 0, 0x40,
				0, 0, 0, 0,
				{0x3e, 0x73, 0x7b}, 0,
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
				{"", ""},
#if defined(SUPPORT_SCSI)
				{"", "", "", ""},
#endif
				"", "", ""};

	PCCORE	pccore = {	PCBASECLOCK25, 4,
						0, PCMODEL_VX, 0, 0, {0x3e, 0x73, 0x7b}, 0,
						0, 0,
						4 * PCBASECLOCK25};

static const BYTE msw_default[8] =
							{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e};

	BYTE	screenupdate = 3;
	int		screendispflag = 1;
	int		soundrenewal = 0;
	BOOL	drawframe;
	UINT	drawcount = 0;
	BOOL	hardwarereset = FALSE;


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

static void pccore_set(void) {

	UINT8	model;
	UINT32	multiple;
	UINT8	extsize;

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
		pccore.cpumode = CPUMODE_8MHZ;
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

	// HDDの接続 (I/Oの使用状態が変わるので..
	if (np2cfg.dipsw[1] & 0x20) {
		pccore.hddif |= PCHDD_IDE;
	}

	// 拡張メモリ
	extsize = 0;
	if (!(np2cfg.dipsw[2] & 0x80)) {
		extsize = min(np2cfg.EXTMEM, 13);
	}
	pccore.extmem = extsize;
	CopyMemory(pccore.dipsw, np2cfg.dipsw, 3);

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
	fddmtrsnd_initialize(rate);
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
	amd98_initialize(rate);
}

static void sound_term(void) {

	soundmng_stop();
	amd98_deinitialize();
	rhythm_deinitialize();
	fddmtrsnd_deinitialize();
	sound_destroy();
}

void pccore_init(void) {

	CPU_INITIALIZE();

	pal_initlcdtable();
	pal_makelcdpal();
	pal_makeskiptable();
	dispsync_initialize();
	sxsi_initialize();

	font_initialize();
	font_load(np2cfg.fontfile, TRUE);
	maketext_initialize();
	makegrph_initialize();
	gdcsub_initialize();
	fddfile_initialize();

	sound_init();

	rs232c_construct();
	mpu98ii_construct();
	pc9861k_initialize();

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

	iocore_destroy();

	pc9861k_deinitialize();
	mpu98ii_destruct();
	rs232c_destruct();

	sxsi_trash();

	CPU_DEINITIALIZE();
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

	soundmng_stop();
	if (soundrenewal) {
		soundrenewal = 0;
		sound_term();
		sound_init();
	}

	ZeroMemory(mem, 0x110000);
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
	ZeroMemory(mem + FONT_ADRS, 0x08000);

	//メモリスイッチ
	for (i=0; i<8; i++) {
		mem[0xa3fe2 + i*4] = np2cfg.memsw[i];
	}

	pccore_set();
	nevent_allreset();

	CPU_RESET();
	CPU_SETEXTSIZE((UINT32)pccore.extmem);

	CPU_TYPE = 0;
	if (np2cfg.dipsw[2] & 0x80) {
		CPU_TYPE = CPUTYPE_V30;
	}
	if (pccore.model & PCMODEL_EPSON) {			// RAM ctrl
		CPU_RAM_D000 = 0xffff;
	}

	// HDDセット
	sxsi_open();
#if defined(SUPPORT_SASI)
	if (sxsi_issasi()) {
		pccore.hddif &= ~PCHDD_IDE;
		pccore.hddif |= PCHDD_SASI;
		TRACEOUT(("supported SASI"));
	}
#endif
#if defined(SUPPORT_SCSI)
	if (sxsi_isscsi()) {
		pccore.hddif |= PCHDD_SCSI;
		TRACEOUT(("supported SCSI"));
	}
#endif

	sound_changeclock();
	beep_changeclock();
	sound_reset();
	fddmtrsnd_bind();

	fddfile_reset2dmode();
	bios0x18_16(0x20, 0xe1);

	iocore_reset();								// サウンドでpicを呼ぶので…
	cbuscore_reset();
	fmboard_reset(pccore.sound);

	i286_memorymap((pccore.model & PCMODEL_EPSON)?1:0);
	iocore_build();
	iocore_bind();
	cbuscore_bind();
	fmboard_bind();

	fddmtr_initialize();
	calendar_initialize();
	vram_initialize();

	pal_change(1);

	bios_initialize();

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

#if defined(SUPPORT_HOSTDRV)
	hostdrv_reset();
#endif

	timing_reset();
	soundmng_play();
}

static void drawscreen(void) {

	UINT8	timing;
	void	(VRAMCALL * grphfn)(int page, int alldraw);
	UINT8	bit;

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

	if (gdcs.textdisp & GDCSCRN_EXT) {
		gdc_updateclock();
	}

	if (!drawframe) {
		return;
	}
	if ((gdcs.textdisp & GDCSCRN_EXT) || (gdcs.grphdisp & GDCSCRN_EXT)) {
		if (dispsync_renewalvertical()) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
		}
	}
	if (gdcs.textdisp & GDCSCRN_EXT) {
		gdcs.textdisp &= ~GDCSCRN_EXT;
		dispsync_renewalhorizontal();
		tramflag.renewal |= 1;
		if (dispsync_renewalmode()) {
			screenupdate |= 2;
		}
	}
	if (gdcs.palchange) {
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
			grphfn = makegrph;
			bit = GDCSCRN_MAKE;
			if (gdcs.disp) {
				bit <<= 1;
			}
#if defined(SUPPORT_PC9821)
			if (gdc.analog & 2) {
				grphfn = makegrphex;
				if (gdc.analog & 4) {
					bit = GDCSCRN_MAKE | (GDCSCRN_MAKE << 1);
				}
			}
#endif
			if (gdcs.grphdisp & bit) {
				(*grphfn)(gdcs.disp, gdcs.grphdisp & bit & GDCSCRN_ALLDRAW2);
				gdcs.grphdisp &= ~bit;
				screenupdate |= 1;
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

void screendisp(NEVENTITEM item) {

	PICITEM		pi;

	gdc_work(GDCWORK_SLAVE);
	gdc.vsync = 0;
	screendispflag = 0;
	if (!np2cfg.DISPSYNC) {
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
	nevent_set(NEVENT_FLAMES, gdc.vsyncclock, screendisp, NEVENT_RELATIVE);

	// drawscreenで pccore.vsyncclockが変更される可能性があります
	if (np2cfg.DISPSYNC) {
		drawscreen();
	}
	(void)item;
}


// ---------------------------------------------------------------------------

// #define	IPTRACE			(1 << 12)

#if defined(TRACE) && IPTRACE
static UINT		trpos = 0;
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
UINT	cflg;
#endif


void pccore_exec(BOOL draw) {

	drawframe = draw;
	keystat_sync();
	soundmng_sync();
	mouseif_sync();
	pal_eventclear();

	gdc.vsync = 0;
	screendispflag = 1;
	MEMWAIT_TRAM = np2cfg.wait[0];
	MEMWAIT_VRAM = np2cfg.wait[2];
	MEMWAIT_GRCG = np2cfg.wait[4];
	nevent_set(NEVENT_FLAMES, gdc.dispclock, screenvsync, NEVENT_RELATIVE);

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
#if 0
			if ((CPU_CS == 0xf800) && (CPU_IP == 0x0B5B)) {
				TRACEOUT(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x",
							mem[0x9000], mem[0x9001],
							mem[0x9002], mem[0x9003],
							mem[0x9004], mem[0x9005],
							mem[0x9006], mem[0x9007],
							mem[0x9008], mem[0x9009]));
			}
			if ((CPU_CS == 0xf800) && (CPU_IP == 0x15FF)) {
				TRACEOUT(("DX = %.4x / DS:DI = %.4x:%.4x [%.2x]",
									CPU_DX, CPU_DS, CPU_DI, mem[0xa3fe0]));
			}
#endif
#if 0
			if (CPU_IP == 0x2E4F) {
				TRACEOUT(("CS = %.4x - 0x2e4f", CPU_CS));
			}
#endif
			if (CPU_CS == 0x8b6) {
				TRACEOUT(("%.4x:%.4x", CPU_CS, CPU_IP));
			}
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

	if (hardwarereset) {
		hardwarereset = FALSE;
		pccore_cfgupdate();
		pccore_reset();
	}

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

