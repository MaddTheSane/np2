#include	"compiler.h"
#include	<windowsx.h>
#include	<io.h>
#include	"strres.h"
#include	"np2.h"
#include	"np2arg.h"
#include	"dosio.h"
#include	"ini.h"
#include	"winkbd.h"
#include	"pccore.h"


static void inirdargs16(const char *src, const INITBL *ini) {

	SINT16	*dst;
	int		dsize;
	int		i;
	char	c;

	dst = (SINT16 *)ini->value;
	dsize = ini->arg;

	for (i=0; i<dsize; i++) {
		while(*src == ' ') {
			src++;
		}
		if (*src == '\0') {
			break;
		}
		dst[i] = (SINT16)milstr_solveINT(src);
		while(*src != '\0') {
			c = *src++;
			if (c == ',') {
				break;
			}
		}
	}
}

static void inirdargh8(const char *src, const INITBL *ini) {

	BYTE	*dst;
	int		dsize;
	int		i;
	BYTE	val;
	BOOL	set;
	char	c;

	dst = (BYTE *)ini->value;
	dsize = ini->arg;

	for (i=0; i<dsize; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		dst[i] = val;
	}
}

static void iniwrsetargh8(char *work, int size, const INITBL *ini) {

	UINT	i;
const BYTE	*ptr;
	UINT	arg;
	char	tmp[8];

	ptr = (BYTE *)ini->value;
	arg = ini->arg;
	if (arg > 0) {
		SPRINTF(tmp, "%.2x", ptr[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<arg; i++) {
		SPRINTF(tmp, " %.2x", ptr[i]);
		milstr_ncat(work, tmp, size);
	}
}


// ---- user

static void inirdbyte3(const char *src, const INITBL *ini) {

	UINT	i;

	for (i=0; i<3; i++) {
		if (src[i] == '\0') {
			break;
		}
		if ((((src[i] - '0') & 0xff) < 9) ||
			(((src[i] - 'A') & 0xdf) < 26)) {
			((BYTE *)ini->value)[i] = src[i];
		}
	}
}

static void inirdkb(const char *src, const INITBL *ini) {

	if ((!milstr_extendcmp(src, "PC98")) ||
		(!milstr_cmp(src, "98"))) {
		*(BYTE *)ini->value = KEY_PC98;
	}
	else if ((!milstr_extendcmp(src, "DOS")) ||
			(!milstr_cmp(src, "PCAT")) ||
			(!milstr_cmp(src, "AT"))) {
		*(BYTE *)ini->value = KEY_KEY106;
	}
	else if ((!milstr_extendcmp(src, "KEY101")) ||
			(!milstr_cmp(src, "101"))) {
		*(BYTE *)ini->value = KEY_KEY101;
	}
}


// ----

void ini_read(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

const INITBL	*p;
const INITBL	*pterm;
	char		work[512];
	UINT32		val;

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		switch(p->itemtype & INITYPE_MASK) {
			case INITYPE_STR:
				GetPrivateProfileString(title, p->item, (char *)p->value,
											(char *)p->value, p->arg, path);
				break;

			case INITYPE_BOOL:
				GetPrivateProfileString(title, p->item,
									(*((BYTE *)p->value))?str_true:str_false,
												work, sizeof(work), path);
				*((BYTE *)p->value) = (!milstr_cmp(work, str_true))?1:0;
				break;

			case INITYPE_ARGS16:
				GetPrivateProfileString(title, p->item, str_null,
												work, sizeof(work), path);
				inirdargs16(work, p);
				break;

			case INITYPE_ARGH8:
				GetPrivateProfileString(title, p->item, str_null,
												work, sizeof(work), path);
				inirdargh8(work, p);
				break;

			case INITYPE_SINT8:
			case INITYPE_UINT8:
				val = (BYTE)GetPrivateProfileInt(title, p->item,
												*(BYTE *)p->value, path);
				*(BYTE *)p->value = (BYTE)val;
				break;

			case INITYPE_SINT16:
			case INITYPE_UINT16:
				val = (UINT16)GetPrivateProfileInt(title, p->item,
												*(UINT16 *)p->value, path);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case INITYPE_SINT32:
			case INITYPE_UINT32:
				val = (UINT32)GetPrivateProfileInt(title, p->item,
												*(UINT32 *)p->value, path);
				*(UINT32 *)p->value = (UINT32)val;
				break;

			case INITYPE_HEX8:
				SPRINTF(work, str_x, *(BYTE *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, sizeof(work), path);
				val = (BYTE)milstr_solveHEX(work);
				*(BYTE *)p->value = (BYTE)val;
				break;

			case INITYPE_HEX16:
				SPRINTF(work, str_x, *(UINT16 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, sizeof(work), path);
				val = (UINT16)milstr_solveHEX(work);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case INITYPE_HEX32:
				SPRINTF(work, str_x, *(UINT32 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, sizeof(work), path);
				val = (UINT32)milstr_solveHEX(work);
				*(UINT16 *)p->value = (UINT32)val;
				break;

			case INITYPE_BYTE3:
				GetPrivateProfileString(title, p->item, str_null,
												work, sizeof(work), path);
				inirdbyte3(work, p);
				break;

			case INITYPE_KB:
				GetPrivateProfileString(title, p->item, str_null,
												work, sizeof(work), path);
				inirdkb(work, p);
				break;
		}
		p++;
	}
}

void ini_write(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

const INITBL	*p;
const INITBL	*pterm;
const char		*set;
	char		work[512];

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & INIFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & INITYPE_MASK) {
				case INITYPE_STR:
					set = (char *)p->value;
					break;

				case INITYPE_BOOL:
					set = (*((BYTE *)p->value))?str_true:str_false;
					break;

				case INITYPE_ARGH8:
					iniwrsetargh8(work, sizeof(work), p);
					break;

				case INITYPE_SINT8:
					SPRINTF(work, str_d, *((char *)p->value));
					break;

				case INITYPE_SINT16:
					SPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case INITYPE_SINT32:
					SPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case INITYPE_UINT8:
					SPRINTF(work, str_u, *((BYTE *)p->value));
					break;

				case INITYPE_UINT16:
					SPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case INITYPE_UINT32:
					SPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case INITYPE_HEX8:
					SPRINTF(work, str_x, *((BYTE *)p->value));
					break;

				case INITYPE_HEX16:
					SPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case INITYPE_HEX32:
					SPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					set = NULL;
					break;
			}
			if (set) {
				WritePrivateProfileString(title, p->item, set, path);
			}
		}
		p++;
	}
}


// ----

static const char ini_title[] = "NekoProjectII";

enum {
	INIRO_STR			= INITYPE_STR + INIFLAG_RO,
	INIRO_BOOL			= INITYPE_BOOL + INIFLAG_RO,
	INIMAX_UINT8		= INITYPE_UINT8 + INIFLAG_MAX,
	INIAND_UINT8		= INITYPE_UINT8 + INIFLAG_AND,
	INIROMAX_SINT32		= INITYPE_SINT32 + INIFLAG_RO + INIFLAG_MAX,
	INIROAND_HEX32		= INITYPE_HEX32 + INIFLAG_RO + INIFLAG_AND,

	INIRO_BYTE3			= INITYPE_BYTE3 + INIFLAG_RO,
	INIRO_KB			= INITYPE_KB + INIFLAG_RO
};

static const INITBL iniitem[] = {
	{"np2title", INIRO_STR,			np2oscfg.titles,
													sizeof(np2oscfg.titles)},
	{"np2winid", INIRO_BYTE3,		np2oscfg.winid,			0},
	{"WindposX", INITYPE_SINT32,	&np2oscfg.winx,			0},
	{"WindposY", INITYPE_SINT32,	&np2oscfg.winy,			0},
	{"paddingx", INIROMAX_SINT32,	&np2oscfg.paddingx,		32},
	{"paddingy", INIROMAX_SINT32,	&np2oscfg.paddingy,		32},
	{"Win_Snap", INITYPE_BOOL,		&np2oscfg.WINSNAP,		0},

	{"FDfolder", INITYPE_STR,		fddfolder,				MAX_PATH},
	{"HDfolder", INITYPE_STR,		hddfolder,				MAX_PATH},
	{"bmap_Dir", INITYPE_STR,		bmpfilefolder,			MAX_PATH},
	{"fontfile", INITYPE_STR,		np2cfg.fontfile,		MAX_PATH},
	{"biospath", INIRO_STR,			np2cfg.biospath,		MAX_PATH},

	{"pc_model", INITYPE_STR,		&np2cfg.model,
													sizeof(np2cfg.model)},
	{"clk_base", INITYPE_UINT32,	&np2cfg.baseclock,		0},
	{"clk_mult", INITYPE_UINT32,	&np2cfg.multiple,		0},

	{"DIPswtch", INITYPE_ARGH8,		np2cfg.dipsw,			3},
	{"MEMswtch", INITYPE_ARGH8,		np2cfg.memsw,			8},
	{"ExMemory", INIMAX_UINT8,		&np2cfg.EXTMEM,			13},
	{"ITF_WORK", INIRO_BOOL,		&np2cfg.ITF_WORK,		0},

	{"HDD1FILE", INITYPE_STR,		np2cfg.hddfile[0],		MAX_PATH},
	{"HDD2FILE", INITYPE_STR,		np2cfg.hddfile[1],		MAX_PATH},
//	{"Removabl", INI_EX|1,	0,		&np2cfg.REMOVEHD,		0},

	{"SampleHz", INITYPE_UINT16,	&np2cfg.samplingrate,	0},
	{"Latencys", INITYPE_UINT16,	&np2cfg.delayms,		0},
	{"SNDboard", INITYPE_HEX8,		&np2cfg.SOUND_SW,		0},
	{"BEEP_vol", INIAND_UINT8,		&np2cfg.BEEP_VOL,		3},
	{"xspeaker", INIRO_BOOL,		&np2cfg.snd_x,			0},

	{"SND14vol", INITYPE_ARGH8,		np2cfg.vol14,			6},
//	{"opt14BRD", INITYPE_ARGH8,		np2cfg.snd14opt,		3},
	{"opt26BRD", INITYPE_HEX8,		&np2cfg.snd26opt,		0},
	{"opt86BRD", INITYPE_HEX8,		&np2cfg.snd86opt,		0},
	{"optSPBRD", INITYPE_HEX8,		&np2cfg.spbopt,			0},
	{"optSPBVR", INITYPE_HEX8,		&np2cfg.spb_vrc,		0},
	{"optSPBVL", INIMAX_UINT8,		&np2cfg.spb_vrl,		24},
	{"optSPB_X", INITYPE_BOOL,		&np2cfg.spb_x,			0},
	{"optMPU98", INITYPE_HEX8		&np2cfg.mpuopt,			0},

	{"volume_F", INIMAX_UINT8,		&np2cfg.vol_fm,			128},
	{"volume_S", INIMAX_UINT8,		&np2cfg.vol_ssg,		128},
	{"volume_A", INIMAX_UINT8,		&np2cfg.vol_adpcm,		128},
	{"volume_P", INIMAX_UINT8,		&np2cfg.vol_pcm,		128},
	{"volume_R", INIMAX_UINT8,		&np2cfg.vol_rhythm,		128},

	{"Seek_Snd", INITYPE_BOOL,		&np2cfg.MOTOR,			0},
	{"Seek_Vol", INIMAX_UINT8,		&np2cfg.MOTORVOL,		100},

	{"btnRAPID", INITYPE_BOOL,		&np2cfg.BTN_RAPID,		0},
	{"btn_MODE", INITYPE_BOOL,		&np2cfg.BTN_MODE,		0},
	{"Mouse_sw", INITYPE_BOOL,		&np2oscfg.MOUSE_SW,		0},
	{"MS_RAPID", INITYPE_BOOL,		&np2cfg.MOUSERAPID,		0},

	{"backgrnd", INIAND_UINT8,		&np2oscfg.background,	3},
	{"VRAMwait", INITYPE_ARGH8,		np2cfg.wait,			6},
	{"DspClock", INIAND_UINT8,		&np2oscfg.DISPCLK,		3},
	{"DispSync", INITYPE_BOOL,		&np2cfg.DISPSYNC,		0},
	{"Real_Pal", INITYPE_BOOL,		&np2cfg.RASTER,			0},
	{"RPal_tim", INIMAX_UINT8,		&np2cfg.realpal,		64},
	{"s_NOWAIT", INITYPE_BOOL,		&np2oscfg.NOWAIT,		0},
	{"SkpFrame", INITYPE_UINT8,		&np2oscfg.DRAW_SKIP,	0},
	{"uPD72020", INITYPE_BOOL,		&np2cfg.uPD72020,		0},
	{"GRCG_EGC", INIAND_UINT8,		&np2cfg.grcg,			3},
	{"color16b", INITYPE_BOOL,		&np2cfg.color16,		0},
	{"skipline", INITYPE_BOOL,		&np2cfg.skipline,		0},
	{"skplight", INITYPE_UINT16,	&np2cfg.skiplight,		0},
	{"LCD_MODE", INIAND_UINT8,		&np2cfg.LCD_MODE,		0x03},
	{"BG_COLOR", INIROAND_HEX32,	&np2cfg.BG_COLOR,		0xffffff},
	{"FG_COLOR", INIROAND_HEX32,	&np2cfg.FG_COLOR,		0xffffff},

	{"pc9861_e", INITYPE_BOOL,		&np2cfg.pc9861enable,	0},
	{"pc9861_s", INITYPE_ARGH8,		np2cfg.pc9861sw,		3},
	{"pc9861_j", INITYPE_ARGH8,		np2cfg.pc9861jmp,		6},

	{"calendar", INITYPE_BOOL,		&np2cfg.calendar,		0},
	{"USE144FD", INITYPE_BOOL,		&np2cfg.usefd144,		0},

	// OSàÀë∂ÅH
	{"keyboard", INIRO_KB,			&np2oscfg.KEYBOARD,		0},
	{"F12_COPY", INITYPE_UINT8,		&np2oscfg.F12COPY,		0},
	{"Joystick", INITYPE_BOOL,		&np2oscfg.JOYPAD1,		0},
	{"Joy1_btn", INITYPE_ARGH8,		np2oscfg.JOY1BTN,		4},

	{"clocknow", INITYPE_UINT8,		&np2oscfg.clk_x,		0},
	{"clockfnt", INITYPE_UINT8,		&np2oscfg.clk_fnt,		0},
	{"clock_up", INIROAND_HEX32,	&np2oscfg.clk_color1,	0xffffff},
	{"clock_dn", INIROAND_HEX32,	&np2oscfg.clk_color2,	0xffffff},

	{"use_sstp", INITYPE_BOOL,		&np2oscfg.sstp,			0},
	{"sstpport", INITYPE_UINT16,	&np2oscfg.sstpport,		0},
	{"comfirm_", INITYPE_BOOL,		&np2oscfg.comfirm,		0},
	{"shortcut", INITYPE_HEX8,		&np2oscfg.shortcut,		0},		// ver0.30

	{"mpu98map", INITYPE_STR,		np2oscfg.mpu.mout,		MAXPNAMELEN},
	{"mpu98min", INITYPE_STR,		np2oscfg.mpu.min,		MAXPNAMELEN},
	{"mpu98mdl", INITYPE_STR,		np2oscfg.mpu.mdl,		64},
	{"mpu98def", INITYPE_STR,		np2oscfg.mpu.def,		MAX_PATH},

	{"com1port", INIMAX_UINT8,		&np2oscfg.com1.port,	5},		// ver0.34
	{"com1para", INITYPE_UINT8,		&np2oscfg.com1.param,	0},
	{"com1_bps", INITYPE_UINT32,	&np2oscfg.com1.speed,	0},
	{"com1mmap", INITYPE_STR,		np2oscfg.com1.mout,		MAXPNAMELEN},
	{"com1mmdl", INITYPE_STR,		np2oscfg.com1.mdl,		64},
	{"com1mdef", INITYPE_STR,		np2oscfg.com1.def,		MAX_PATH},

	{"com2port", INIMAX_UINT8,		&np2oscfg.com2.port,	5},		// ver0.34
	{"com2para", INITYPE_UINT8,		&np2oscfg.com2.param,	0},
	{"com2_bps", INITYPE_UINT32,	&np2oscfg.com2.speed,	0},
	{"com2mmap", INITYPE_STR,		np2oscfg.com2.mout,		MAXPNAMELEN},
	{"com2mmdl", INITYPE_STR,		np2oscfg.com2.mdl,		64},
	{"com2mdef", INITYPE_STR,		np2oscfg.com2.def,		MAX_PATH},

	{"com3port", INIMAX_UINT8,		&np2oscfg.com3.port,	5},		// ver0.34
	{"com3para", INITYPE_UINT8,		&np2oscfg.com3.param,	0},
	{"com3_bps", INITYPE_UINT32,	&np2oscfg.com3.speed,	0},
	{"com3mmap", INITYPE_STR,		np2oscfg.com3.mout,		MAXPNAMELEN},
	{"com3mmdl", INITYPE_STR,		np2oscfg.com3.mdl,		64},
	{"com3mdef", INITYPE_STR,		np2oscfg.com3.def,		MAX_PATH},

	{"force400", INIRO_BOOL,		&np2oscfg.force400,		0},
	{"e_resume", INITYPE_BOOL,		&np2oscfg.resume,		0},
	{"STATSAVE", INIRO_BOOL,		&np2oscfg.statsave,		0},
	{"nousemmx", INITYPE_BOOL,		&np2oscfg.disablemmx,	0},		// ver0.36
	{"windtype", INITYPE_UINT8,		&np2oscfg.wintype,		0},
	{"toolwind", INITYPE_BOOL,		&np2oscfg.toolwin,		0},		// ver0.38
	{"keydispl", INITYPE_BOOL,		&np2oscfg.keydisp,		0},
	{"jast_snd", INITYPE_BOOL,		&np2oscfg.jastsnd,		0},		// ver0.73
	{"I286SAVE", INIRO_BOOL,		&np2oscfg.I286SAVE,		0}};


void initgetfile(char *path, UINT size) {

	char	*p;

	file_cpyname(path, modulefile, size);
	if (np2arg.ini) {
		if ((np2arg.ini[0] == '/') || (np2arg.ini[0] == '-')) {
			file_cutname(path);
			file_catname(path, file_getname(np2arg.ini + 2), size);
		}
		else {
			file_cpyname(path, np2arg.ini, size);
		}
		p = file_getext(path);
		if (!(*p)) {
			file_catname(path, ".ini", size);
		}
	}
	else {
		file_cutext(path);
		file_catname(path, ".ini", size);
	}
}

void initload(void) {

	char	path[MAX_PATH];

	initgetfile(path, sizeof(path));
	ini_read(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

void initsave(void) {

	char	path[MAX_PATH];

	initgetfile(path, sizeof(path));
	ini_write(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

