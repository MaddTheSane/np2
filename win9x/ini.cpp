#include	"compiler.h"
#include	<windowsx.h>
#include	<io.h>
#include	"np2.h"
#include	"np2arg.h"
#include	"dosio.h"
#include	"ini.h"
#include	"winkbd.h"
#include	"pccore.h"


#define		CFGLEN		(MAX_PATH + 32)

static const char ini_title[] = "NekoProjectII";

typedef struct {
	char	item[9];
	BYTE	itemtype;
	WORD	flag;
	void	*value;
	DWORD	arg;
} INI_ITEM;

#define	INI_RO		0x0001
#define	INIAND		0x0002
#define	INIMAX		0x0004
#define	INIA4		0x0008
#define	INIEND		0x8000

#define	INI_STR		0x00
#define	INI_BOOL	0x01
#define	INI_ARG		0x02
#define	INI_BYTE	0x11
#define	INI_WORD	0x12
#define	INI_DWORD	0x14
#define	INI_HEXB	0x21
#define	INI_HEXD	0x24
#define	INI_EX		0x80


static INI_ITEM iniitem[] = {

	{"np2title", INI_STR,	INI_RO|INIMAX,
									np2oscfg.titles, sizeof(np2oscfg.titles)},
	{"np2winid", INI_EX|0,	INI_RO,	np2oscfg.winid,			0},

	{"WindposX", INI_DWORD,	0,		&np2oscfg.winx,			0},
	{"WindposY", INI_DWORD, 0,		&np2oscfg.winy,			0},
	{"paddingx", INI_DWORD,	INI_RO|INIMAX,
									&np2oscfg.paddingx,		32},
	{"paddingy", INI_DWORD,	INI_RO|INIMAX,
									&np2oscfg.paddingy,		32},
	{"Win_Snap", INI_BOOL,	0,		&np2oscfg.WINSNAP,		0},

	{"FDfolder", INI_STR,	INIMAX,	fddfolder,				MAX_PATH},
	{"HDfolder", INI_STR,	INIMAX,	hddfolder,				MAX_PATH},
	{"bmap_Dir", INI_STR,	INIMAX,	bmpfilefolder,			MAX_PATH},

	{"fontfile", INI_STR,	INIMAX, np2cfg.fontfile,		MAX_PATH},

	{"clk_base", INI_DWORD,	0,		&np2cfg.baseclock,		0},
	{"clk_mult", INI_DWORD,	0,		&np2cfg.multiple,		0},

	{"DIPswtch", INI_ARG,	0,		np2cfg.dipsw,			3},
	{"MEMswtch", INI_ARG,	0,		np2cfg.memsw,			8},
	{"ExMemory", INI_BYTE,	INIMAX,	&np2cfg.EXTMEM,			13},
	{"ITF_WORK", INI_BOOL,	INI_RO,	&np2cfg.ITF_WORK,		0},

	{"HDD1FILE", INI_STR,	INIMAX,	np2cfg.hddfile[0],		MAX_PATH},
	{"HDD2FILE", INI_STR,	INIMAX,	np2cfg.hddfile[1],		MAX_PATH},
//	{"Removabl", INI_EX|1,	0,		&np2cfg.REMOVEHD,		0},

	{"SampleHz", INI_WORD,	0,		&np2cfg.samplingrate,	0},
	{"Latencys", INI_WORD,	0,		&np2cfg.delayms,		0},
	{"SNDboard", INI_HEXB,	0,		&np2cfg.SOUND_SW,		0},
	{"BEEP_vol", INI_BYTE,	INIAND,	&np2cfg.BEEP_VOL,		0x03},
	{"xspeaker", INI_BOOL,	INI_RO,	&np2cfg.snd_x,			0},

	{"SND14vol", INI_ARG,	INIA4,	np2cfg.vol14,			6},		// ver0.27
//	{"opt14BRD", INI_ARG,	0,		np2cfg.snd14opt,		3},
	{"opt26BRD", INI_HEXB,	0,		&np2cfg.snd26opt,		0},
	{"opt86BRD", INI_HEXB,	0,		&np2cfg.snd86opt,		0},
	{"optSPBRD", INI_HEXB,	0,		&np2cfg.spbopt,			0},
	{"optSPBVR", INI_HEXB,	0,		&np2cfg.spb_vrc,		0},		// ver0.30
	{"optSPBVL", INI_BYTE,	INIMAX,	&np2cfg.spb_vrl,		24},	// ver0.30
	{"optSPB_X", INI_BOOL,	0,		&np2cfg.spb_x,			0},
	{"optMPU98", INI_HEXB,	0,		&np2cfg.mpuopt,			0},

	{"volume_F", INI_BYTE,	INIMAX,	&np2cfg.vol_fm,			128},	// ver0.28
	{"volume_S", INI_BYTE,	INIMAX,	&np2cfg.vol_ssg,		128},
	{"volume_A", INI_BYTE,	INIMAX,	&np2cfg.vol_adpcm,		128},
	{"volume_P", INI_BYTE,	INIMAX,	&np2cfg.vol_pcm,		128},
	{"volume_R", INI_BYTE,	INIMAX,	&np2cfg.vol_rhythm,		128},

	{"Seek_Snd", INI_BOOL,	0,		&np2cfg.MOTOR,			0},
	{"Seek_Vol", INI_BYTE,	INIMAX,	&np2cfg.MOTORVOL,		100},

	{"btnRAPID", INI_BOOL,	0,		&np2cfg.BTN_RAPID,		0},
	{"btn_MODE", INI_BOOL,	0,		&np2cfg.BTN_MODE,		0},
	{"Mouse_sw", INI_BOOL,	0,		&np2oscfg.MOUSE_SW,		0},
	{"MS_RAPID", INI_BOOL,	0,		&np2cfg.MOUSERAPID,		0},

	{"backgrnd", INI_BYTE,	INIAND,	&np2oscfg.background,	0x03},
	{"VRAMwait", INI_ARG,	0,		np2cfg.wait,			6},
	{"DspClock", INI_BYTE,	INIAND,	&np2oscfg.DISPCLK,		0x03},
	{"DispSync", INI_BOOL,	0,		&np2cfg.DISPSYNC,		0},
	{"Real_Pal", INI_BOOL,	0,		&np2cfg.RASTER,			0},
	{"RPal_tim", INI_BYTE,	INIMAX,	&np2cfg.realpal,		64},
	{"s_NOWAIT", INI_BOOL,	0,		&np2oscfg.NOWAIT,		0},
	{"SkpFrame", INI_BYTE,	0,		&np2oscfg.DRAW_SKIP,	0},
	{"uPD72020", INI_BOOL,	0,		&np2cfg.uPD72020,		0},
	{"GRCG_EGC", INI_BYTE,	INIAND,	&np2cfg.grcg,			0x03},
	{"color16b", INI_BOOL,	0,		&np2cfg.color16,		0},
	{"skipline", INI_BOOL,	0,		&np2cfg.skipline,		0},
	{"skplight", INI_WORD,	0,		&np2cfg.skiplight,		0},
	{"LCD_MODE", INI_BYTE,	INIAND,	&np2cfg.LCD_MODE,		0x03},
	{"BG_COLOR", INI_HEXD,	INI_RO|INIAND,
									&np2cfg.BG_COLOR,		0xffffff},
	{"FG_COLOR", INI_HEXD,	INI_RO|INIAND,
									&np2cfg.FG_COLOR,		0xffffff},

	{"pc9861_e", INI_BOOL,	0,		&np2cfg.pc9861enable,	0},		// ver0.29
	{"pc9861_s", INI_ARG,	0,		np2cfg.pc9861sw,		3},
	{"pc9861_j", INI_ARG,	0,		np2cfg.pc9861jmp,		6},

	{"calendar", INI_BOOL,	0,		&np2cfg.calendar,		0},		// ver0.34
	{"USE144FD", INI_BOOL,	0,		&np2cfg.usefd144,		0},		// ver0.31

	// OSˆË‘¶H														// ver0.29
	{"keyboard", INI_EX|3,	INI_RO,	&np2oscfg.KEYBOARD,		0},
	{"F12_COPY", INI_BYTE,	0,		&np2oscfg.F12COPY,		0},
	{"Joystick", INI_BOOL,	0,		&np2oscfg.JOYPAD1,		0},		// ver0.28
	{"Joy1_btn", INI_ARG,	0,		np2oscfg.JOY1BTN,		4},

	{"clocknow", INI_BYTE,	0,		&np2oscfg.clk_x,		0},
	{"clockfnt", INI_BYTE,	0,		&np2oscfg.clk_fnt,		0},
	{"clock_up", INI_HEXD,	INI_RO|INIAND,
									&np2oscfg.clk_color1,	0xffffff},
	{"clock_dn", INI_HEXD,	INI_RO|INIAND,
									&np2oscfg.clk_color2,	0xffffff},

	{"use_sstp", INI_BOOL,	0,		&np2oscfg.sstp,			0},
	{"sstpport", INI_WORD,	0,		&np2oscfg.sstpport,		0},
	{"comfirm_", INI_BOOL,	0,		&np2oscfg.comfirm,		0},
	{"shortcut", INI_HEXB,	0,		&np2oscfg.shortcut,		0},		// ver0.30


	{"mpu98map", INI_STR,	INIMAX, np2oscfg.mpu.mout,		MAXPNAMELEN},
	{"mpu98min", INI_STR,	INIMAX,	np2oscfg.mpu.min,		MAXPNAMELEN},
	{"mpu98mdl", INI_STR,	INIMAX,	np2oscfg.mpu.mdl,		64},
	{"mpu98def", INI_STR,	INIMAX, np2oscfg.mpu.def,		MAX_PATH},

	{"com1port", INI_BYTE,	INIMAX,	&np2oscfg.com1.port,	5},		// ver0.34
	{"com1para", INI_BYTE,	0,		&np2oscfg.com1.param,	0},
	{"com1_bps", INI_DWORD,	0,		&np2oscfg.com1.speed,	0},
	{"com1mmap", INI_STR,	INIMAX, np2oscfg.com1.mout,		MAXPNAMELEN},
	{"com1mmdl", INI_STR,	INIMAX,	np2oscfg.com1.mdl,		64},
	{"com1mdef", INI_STR,	INIMAX, np2oscfg.com1.def,		MAX_PATH},

	{"com2port", INI_BYTE,	INIMAX,	&np2oscfg.com2.port,	5},		// ver0.34
	{"com2para", INI_BYTE,	0,		&np2oscfg.com2.param,	0},
	{"com2_bps", INI_DWORD,	0,		&np2oscfg.com2.speed,	0},
	{"com2mmap", INI_STR,	INIMAX, np2oscfg.com2.mout,		MAXPNAMELEN},
	{"com2mmdl", INI_STR,	INIMAX,	np2oscfg.com2.mdl,		64},
	{"com2mdef", INI_STR,	INIMAX, np2oscfg.com2.def,		MAX_PATH},

	{"com3port", INI_BYTE,	INIMAX,	&np2oscfg.com3.port,	5},		// ver0.34
	{"com3para", INI_BYTE,	0,		&np2oscfg.com3.param,	0},
	{"com3_bps", INI_DWORD,	0,		&np2oscfg.com3.speed,	0},
	{"com3mmap", INI_STR,	INIMAX, np2oscfg.com3.mout,		MAXPNAMELEN},
	{"com3mmdl", INI_STR,	INIMAX,	np2oscfg.com3.mdl,		64},
	{"com3mdef", INI_STR,	INIMAX, np2oscfg.com3.def,		MAX_PATH},

	{"force400", INI_BOOL,	INI_RO,	&np2oscfg.force400,		0},
	{"e_resume", INI_BOOL,	0,		&np2oscfg.resume,		0},		// ver0.30
	{"STATSAVE", INI_BOOL,	INI_RO,	&np2oscfg.statsave,		0},		// ver0.30

	{"I286SAVE", INI_BOOL,	INI_RO|INIEND,
									&np2oscfg.I286SAVE,		0}};

#define	INIITEMS	(sizeof(iniitem) / sizeof(INI_ITEM))


static long solveHEX(char *str) {

	return(milstr_solveHEX(str));
}

static char *makeBOOL(BYTE value) {

	if (value) {
		return("true");
	}
	return("false");
}

static BYTE solveBOOL(LPSTR str) {

	if ((!milstr_cmp(str, "TRUE")) || (!milstr_cmp(str, "ON")) ||
		(!milstr_cmp(str, "+")) || (!milstr_cmp(str, "1")) ||
		(!milstr_cmp(str, "ENABLE"))) {
		return(1);
	}
	return(0);
}


static void getinifile(char *initfile, int max) {

	char	*p;

	milstr_ncpy(initfile, modulefile, max);
	if (np2arg.ini) {
		if ((np2arg.ini[0] == '/') || (np2arg.ini[0] == '-')) {
			file_cutname(initfile);
			milstr_ncat(initfile, file_getname(np2arg.ini + 2), max);
		}
		else {
			milstr_ncpy(initfile, np2arg.ini, max);
		}
		p = file_getext(initfile);
		if (!(*p)) {
			milstr_ncat(initfile, ".ini", max);
		}
	}
	else {
		file_cutext(initfile);
		milstr_ncat(initfile, ".ini", max);
	}
}


void initload(void) {

	char		initfile[MAX_PATH];
	char		cfgstr[CFGLEN];
	DWORD		i;
	char		*arg[8];
	DWORD		args;
	INI_ITEM	*p;

	getinifile(initfile, MAX_PATH);									// ver0.29
	p = iniitem;
	while(1) {
		if (!(p->itemtype & 0xf0)) {
			switch(p->itemtype) {
				case INI_STR:
					GetPrivateProfileString(ini_title, p->item,
										(char *)p->value, (char *)p->value,
										p->arg, initfile);
					break;
				case INI_BOOL:
					GetPrivateProfileString(ini_title, p->item,
										makeBOOL(*((BYTE *)p->value) & 1),
												cfgstr, CFGLEN, initfile);
					*(BYTE *)(p->value) = solveBOOL(cfgstr);
					break;
				case INI_ARG:
					GetPrivateProfileString(ini_title, p->item, "",
												cfgstr, CFGLEN, initfile);
					args = milstr_getarg(cfgstr, arg, p->arg);
					if (args == p->arg) {
						for (i=0; i<args; i++) {
							((BYTE *)p->value)[i] = (BYTE)solveHEX(arg[i]);
							if (p->flag & INIA4) {
								((BYTE *)p->value)[i] &= 0x0f;
							}
						}
					}
					break;
			}
		}
		else if (!(p->itemtype & INI_EX)) {
			switch(p->itemtype) {
				case INI_BYTE:
					*(BYTE *)p->value = (BYTE)GetPrivateProfileInt(
							ini_title, p->item, *(BYTE *)p->value, initfile);
					break;
				case INI_WORD:
					*(WORD *)p->value = (WORD)GetPrivateProfileInt(
							ini_title, p->item, *(WORD *)p->value, initfile);
					break;
				case INI_DWORD:
					*(DWORD *)p->value = (DWORD)GetPrivateProfileInt(
							ini_title, p->item, *(DWORD *)p->value, initfile);
					break;
				case INI_HEXB:
					wsprintf(cfgstr, "%x", *(BYTE *)p->value),
					GetPrivateProfileString(ini_title, p->item,
										cfgstr, cfgstr, CFGLEN, initfile);
					*(BYTE *)p->value = (BYTE)solveHEX(cfgstr);
					break;
				case INI_HEXD:
					wsprintf(cfgstr, "%x", *(DWORD *)p->value),
					GetPrivateProfileString(ini_title, p->item,
										cfgstr, cfgstr, CFGLEN, initfile);
					*(DWORD *)p->value = solveHEX(cfgstr);
					break;
			}
			if (p->flag & INIMAX) {
				switch((p->itemtype) & 0x0f) {
					case 1:
						if (*(BYTE *)p->value > (BYTE)p->arg) {
							*(BYTE *)p->value = (BYTE)p->arg;
						}
						break;
					case 2:
						if (*(WORD *)p->value > (WORD)p->arg) {
							*(WORD *)p->value = (WORD)p->arg;
						}
						break;
					case 4:
						if (*(DWORD *)p->value > (DWORD)p->arg) {
							*(DWORD *)p->value = (DWORD)p->arg;
						}
						break;
				}
			}
			if (p->flag & INIAND) {
				switch((p->itemtype) & 0x0f) {
					case 1:
						*(BYTE *)p->value &= (BYTE)p->arg;
						break;
					case 2:
						*(WORD *)p->value &= (WORD)p->arg;
						break;
					case 4:
						*(DWORD *)p->value &= (DWORD)p->arg;
						break;
				}
			}
		}
		else {
			switch(p->itemtype) {
				case INI_EX|0:				// winid
					GetPrivateProfileString(ini_title, p->item,
								(char *)p->value, cfgstr, CFGLEN, initfile);
					for (i=0; i<3; i++) {
						if (((cfgstr[i] >= '0') && (cfgstr[i] < '9')) ||
							((cfgstr[i] >= 'A') && (cfgstr[i] < 'Z')) ||
							((cfgstr[i] >= 'a') && (cfgstr[i] < 'z'))) {
							((BYTE *)p->value)[i] = cfgstr[i];
						}
					}
					break;
				case INI_EX|1:				// remove hdd
					*(BYTE *)p->value =
							(BYTE)((GetPrivateProfileInt(ini_title, p->item,
										((*(BYTE *)p->value) & 1) + 1,
											initfile) - 1) & 1);
					break;
#if 0
				case INI_EX|2:				// midi module
					GetPrivateProfileString(ini_title, p->item,
										MIDI_num2module(*(BYTE *)p->value),
										cfgstr, CFGLEN, initfile);
					*(BYTE *)p->value = MIDI_module2num(cfgstr);
					break;
#endif
				case INI_EX|3:				// keyboard
					GetPrivateProfileString(ini_title, p->item, "unknown",
												cfgstr, CFGLEN, initfile);
					if ((!milstr_extendcmp(cfgstr, "PC98")) ||
						(!milstr_cmp(cfgstr, "98"))) {
						*(BYTE *)p->value = KEY_PC98;
					}
					else if ((!milstr_extendcmp(cfgstr, "DOS")) ||
							(!milstr_cmp(cfgstr, "PCAT")) ||
							(!milstr_cmp(cfgstr, "AT"))) {
						*(BYTE *)p->value = KEY_KEY106;
					}
					else if ((!milstr_extendcmp(cfgstr, "KEY101")) ||
							(!milstr_cmp(cfgstr, "101"))) {
						*(BYTE *)p->value = KEY_KEY101;
					}
					break;
			}
		}
		if (p->flag & INIEND) {
			break;
		}
		p++;
	}
}

void initsave(void) {

	char		initfile[MAX_PATH];
	char		cfgstr[CFGLEN];
	DWORD		i;
	INI_ITEM	*p;

	getinifile(initfile, MAX_PATH);									// ver0.29
	p = iniitem;
	while(!(p->flag & INIEND)) {
		if (p->flag & INI_RO) {
		}
		else if (!(p->itemtype & INI_EX)) {
			switch(p->itemtype) {
				case INI_STR:
					WritePrivateProfileString(ini_title, p->item,
												(char *)p->value, initfile);
					break;
				case INI_BOOL:
					WritePrivateProfileString(ini_title, p->item,
								makeBOOL(*((BYTE *)p->value) & 1), initfile);
					break;
				case INI_ARG:
					for (i=0; i<p->arg; i++) {
						wsprintf(&cfgstr[i*3], "%02x ",
													((BYTE *)p->value)[i]);
					}
					cfgstr[i*3-1] = '\0';
					WritePrivateProfileString(ini_title, p->item,
															cfgstr, initfile);
					break;
				case INI_BYTE:
					wsprintf(cfgstr, "%u", *(BYTE *)p->value);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
				case INI_WORD:
					wsprintf(cfgstr, "%u", *(WORD *)p->value);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
				case INI_DWORD:
					wsprintf(cfgstr, "%u", *(DWORD *)p->value);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
				case INI_HEXB:
					wsprintf(cfgstr, "%x", *(BYTE *)p->value);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
				case INI_HEXD:
					wsprintf(cfgstr, "%x", *(DWORD *)p->value);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
			}
		}
		else {
			switch(p->itemtype) {
				case INI_EX|1:				// remove hdd
					wsprintf(cfgstr, "%u", (*(BYTE *)p->value) + 1);
					WritePrivateProfileString(ini_title, p->item,
														cfgstr, initfile);
					break;
#if 0
				case INI_EX|2:				// midi module
					WritePrivateProfileString(ini_title, p->item,
								MIDI_num2module(*(BYTE *)p->value), initfile);
					break;
#endif
			}
		}
		if (p->flag & INIEND) {
			break;
		}
		p++;
	}
}

