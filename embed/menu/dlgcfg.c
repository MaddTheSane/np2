#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"menustr.h"
#include	"sysmenu.res"


enum {
	DID_CLOCK1	= DID_USER,
	DID_CLOCK2,
	DID_MULTIPLE,
	DID_MULSTR,
	DID_CLOCKSTR,
	DID_MODELVM,
	DID_MODELVX,
	DID_MODELEPSON,
	DID_RATE11,
	DID_RATE22,
	DID_RATE44,
	DID_BUFFER,
	DID_BUFSTR,
	DID_RESUME
};

#if defined(SIZE_QVGA)
static const MENUPRM res_cfg[] = {
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"CPU",									  6,   6, 185,  58},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Base",									 16,  21,  38,  11},
			{DLGTYPE_RADIO,		DID_CLOCK1,		MENU_TABSTOP,
				"1.9968MHz",							 54,  21,  64,  11},
			{DLGTYPE_RADIO,		DID_CLOCK2,		0,
				"2.4576MHz",							119,  21,  64,  11},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Multiple",								 16,  39,  63,  11},
			{DLGTYPE_SLIDER,	DID_MULTIPLE,	MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(1, 16),				 80,  38,  63,  13},
			{DLGTYPE_LTEXT,		DID_MULSTR,		0,
				NULL,									152,  39,  36,  11},
			{DLGTYPE_RTEXT,		DID_CLOCKSTR,	0,
				NULL,									105,  51,  80,  11},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"Architecture",							  6,  70, 244,  30},
			{DLGTYPE_RADIO,		DID_MODELVM,	MENU_TABSTOP,
				"PC-9801VM",							 16,  84,  78,  11},
			{DLGTYPE_RADIO,		DID_MODELVX,	0,
				"PC-9801VX",							 96,  84,  78,  11},
			{DLGTYPE_RADIO,		DID_MODELEPSON,	0,
				"EPSON",								176,  84,  60,  11},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"Sound",								  6, 106, 244,  52},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Rate",									 16, 121,  40,  11},
			{DLGTYPE_RADIO,		DID_RATE11,		MENU_TABSTOP,
				"11KHz",								 60, 121,  60,  11},
			{DLGTYPE_RADIO,		DID_RATE22,		0,
				"22KHz",								120, 121,  60,  11},
			{DLGTYPE_RADIO,		DID_RATE44,		0,
				"44KHz",								180, 121,  60,  11},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Buffer",								 16, 139,  40,  11},
			{DLGTYPE_SLIDER,	DID_BUFFER,		MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(100, 1000),			 60, 138, 140,  13},
			{DLGTYPE_RTEXT,		DID_BUFSTR,		0,
				NULL,									200, 139,  44,  11},
			{DLGTYPE_CHECK,		DID_RESUME,		MENU_TABSTOP,
				str_resume,								  6, 164, 128,  11},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								197,  11,  52,  15},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							197,  30,  52,  15}};
#else
static const MENUPRM res_cfg[] = {
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"CPU",									  8,   7, 202,  69},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Base",									 20,  24,  28,  13},
			{DLGTYPE_RADIO,		DID_CLOCK1,		MENU_TABSTOP,
				"1.9968MHz",							 52,  24,  78,  13},
			{DLGTYPE_RADIO,		DID_CLOCK2,		0,
				"2.4576MHz",							130,  24,  78,  13},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Multiple",								 20,  45,  64,  13},
			{DLGTYPE_SLIDER,	DID_MULTIPLE,	MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(1, 16),				 84,  45,  80,  13},
			{DLGTYPE_LTEXT,		DID_MULSTR,		0,
				NULL,									172,  45,  32,  13},
			{DLGTYPE_RTEXT,		DID_CLOCKSTR,	0,
				NULL,									124,  59,  80,  13},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"Architecture",							  8,  83, 288,  38},
			{DLGTYPE_RADIO,		DID_MODELVM,	MENU_TABSTOP,
				"PC-9801VM",							 20, 100,  98,  13},
			{DLGTYPE_RADIO,		DID_MODELVX,	0,
				"PC-9801VX",							120, 100,  98,  13},
			{DLGTYPE_RADIO,		DID_MODELEPSON,	0,
				"EPSON",								220, 100,  60,  13},
			{DLGTYPE_FRAME,		DID_STATIC,		0,
				"Sound",								  8, 128, 288,  60},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Rate",									 20, 145,  48,  13},
			{DLGTYPE_RADIO,		DID_RATE11,		MENU_TABSTOP,
				"11KHz",								 68, 145,  66,  13},
			{DLGTYPE_RADIO,		DID_RATE22,		0,
				"22KHz",								134, 145,  66,  13},
			{DLGTYPE_RADIO,		DID_RATE44,		0,
				"44KHz",								200, 145,  66,  13},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				"Buffer",								 20, 165,  48,  13},
			{DLGTYPE_SLIDER,	DID_BUFFER,		MSS_BOTH | MENU_TABSTOP,
				(void *)SLIDERPOS(100, 1000),			 68, 165, 160,  13},
			{DLGTYPE_RTEXT,		DID_BUFSTR,		0,
				NULL,									228, 165,  48,  13},
			{DLGTYPE_CHECK,		DID_RESUME,		MENU_TABSTOP,
				"Resume",								  8, 197, 288,  13},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								218,  13,  77,  21},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							218,  38,  77,  21}};
#endif


// ----

static const char str_mulfmt[] = "x%u";
static const char str_clockfmt[] = "%2u.%.4uMHz";
static const char str_buffmt[] = "%ums";

static void setmulstr(void) {

	UINT	multiple;
	char	work[32];

	multiple = menudlg_getval(DID_MULTIPLE);
	if (multiple < 1) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	SPRINTF(work, str_mulfmt, multiple);
	menudlg_settext(DID_MULSTR, work);
}

static void setclockstr(void) {

	UINT32	clock;
	UINT	multiple;
	char	work[32];

	if (menudlg_getval(DID_CLOCK1)) {
		clock = PCBASECLOCK20 / 100;
	}
	else {
		clock = PCBASECLOCK25 / 100;
	}
	multiple = menudlg_getval(DID_MULTIPLE);
	if (multiple < 1) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	clock *= multiple;
	SPRINTF(work, str_clockfmt, clock / 10000, clock % 10000);
	menudlg_settext(DID_CLOCKSTR, work);
}

static void setbufstr(void) {

	UINT	val;
	char	work[32];

	val = menudlg_getval(DID_BUFFER);
	if (val < 100) {
		val = 100;
	}
	else if (val > 1000) {
		val = 1000;
	}
	SPRINTF(work, str_buffmt, val);
	menudlg_settext(DID_BUFSTR, work);
}

static void dlginit(void) {

	MENUID	id;

	menudlg_appends(res_cfg, sizeof(res_cfg)/sizeof(MENUPRM));
	if (np2cfg.baseclock < ((PCBASECLOCK25 + PCBASECLOCK20) / 2)) {
		id = DID_CLOCK1;
	}
	else {
		id = DID_CLOCK2;
	}
	menudlg_setval(id, 1);
	menudlg_setval(DID_MULTIPLE, np2cfg.multiple);

	if (!milstr_cmp(np2cfg.model, str_VM)) {
		id = DID_MODELVM;
	}
	else if (!milstr_cmp(np2cfg.model, str_EPSON)) {
		id = DID_MODELEPSON;
	}
	else {
		id = DID_MODELVX;
	}
	menudlg_setval(id, 1);

	if (np2cfg.samplingrate < ((11025 + 22050) / 2)) {
		id = DID_RATE11;
	}
	else if (np2cfg.samplingrate < ((22050 + 44100) / 2)) {
		id = DID_RATE22;
	}
	else {
		id = DID_RATE44;
	}
	menudlg_setval(id, 1);
	menudlg_setval(DID_BUFFER, np2cfg.delayms);

	menudlg_setval(DID_RESUME, np2oscfg.resume);

	setmulstr();
	setclockstr();
	setbufstr();

#if defined(DISABLE_SOUND)
	menudlg_setenable(DID_RATE11, FALSE);
	menudlg_setenable(DID_RATE22, FALSE);
	menudlg_setenable(DID_RATE44, FALSE);
	menudlg_setenable(DID_BUFFER, FALSE);
#endif
}

static void dlgupdate(void) {

	UINT	update;
	UINT	val;
const char	*str;

	update = 0;
	if (menudlg_getval(DID_CLOCK1)) {
		val = PCBASECLOCK20;
	}
	else {
		val = PCBASECLOCK25;
	}
	if (np2cfg.baseclock != val) {
		np2cfg.baseclock = val;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}
	val = menudlg_getval(DID_MULTIPLE);
	if (val < 1) {
		val = 1;
	}
	else if (val > 32) {
		val = 32;
	}
	if (np2cfg.multiple != val) {
		np2cfg.multiple = val;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	if (menudlg_getval(DID_RATE11)) {
		val = 11025;
	}
	else if (menudlg_getval(DID_RATE44)) {
		val = 44100;
	}
	else {
		val = 22050;
	}
	if (np2cfg.samplingrate != (UINT16)val) {
		np2cfg.samplingrate = (UINT16)val;
		update |= SYS_UPDATECFG | SYS_UPDATERATE;
		soundrenewal = 1;
	}

	if (menudlg_getval(DID_MODELVM)) {
		str = str_VM;
	}
	else if (menudlg_getval(DID_MODELEPSON)) {
		str = str_EPSON;
	}
	else {
		str = str_VX;
	}
	if (milstr_cmp(np2cfg.model, str)) {
		milstr_ncpy(np2cfg.model, str, sizeof(np2cfg.model));
		update |= SYS_UPDATECFG;
	}

	if (menudlg_getval(DID_RATE11)) {
		val = 11025;
	}
	else if (menudlg_getval(DID_RATE44)) {
		val = 44100;
	}
	else {
		val = 22050;
	}
	if (np2cfg.samplingrate != (UINT16)val) {
		np2cfg.samplingrate = (UINT16)val;
		update |= SYS_UPDATECFG | SYS_UPDATERATE;
		soundrenewal = 1;
	}
	val = menudlg_getval(DID_BUFFER);
	if (val < 100) {
		val = 100;
	}
	else if (val > 1000) {
		val = 1000;
	}
	if (np2cfg.delayms != (UINT16)val) {
		np2cfg.delayms = (UINT16)val;
		update |= SYS_UPDATECFG | SYS_UPDATESBUF;
		soundrenewal = 1;
	}

	val = menudlg_getval(DID_RESUME);
	if (np2oscfg.resume != (BYTE)val) {
		np2oscfg.resume = (BYTE)val;
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);
}

int dlgcfg_cmd(int msg, MENUID id, long param) {

	switch(msg) {
		case DLGMSG_CREATE:
			dlginit();
			break;

		case DLGMSG_COMMAND:
			switch(id) {
				case DID_OK:
					dlgupdate();
					menubase_close();
					break;

				case DID_CANCEL:
					menubase_close();
					break;

				case DID_CLOCK1:
				case DID_CLOCK2:
					setclockstr();
					break;

				case DID_MULTIPLE:
					setmulstr();
					setclockstr();
					break;

				case DID_BUFFER:
					setbufstr();
					break;
			}
			break;

		case DLGMSG_CLOSE:
			menubase_close();
			break;
	}
	(void)param;
	return(0);
}

