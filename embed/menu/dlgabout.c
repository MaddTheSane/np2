#include	"compiler.h"
#include	"strres.h"
#include	"pccore.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"menustr.h"
#include	"sysmenu.res"


enum {
	DID_ICON	= DID_USER,
	DID_VER
};

#if defined(SIZE_QVGA)
static const MENUPRM res_about[] = {
			{DLGTYPE_ICON,		DID_ICON,		0,
				(void *)MICON_NP2,						  7,   7,  24,  24},
			{DLGTYPE_LTEXT,		DID_VER,		0,
				NULL,									 40,  14, 128,  11},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								176,   8,  48,  15}};
#else
static const MENUPRM res_about[] = {
			{DLGTYPE_ICON,		DID_ICON,		0,
				(void *)MICON_NP2,						 14,  12,  32,  32},
			{DLGTYPE_LTEXT,		DID_VER,		0,
				NULL,									 64,  24, 180,  13},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				mstr_ok,								258,  12,  70,  21}};
#endif


// ----

static void dlginit(void) {

	char	work[128];

	menudlg_appends(res_about, sizeof(res_about)/sizeof(MENUPRM));
	milstr_ncpy(work, str_np2, sizeof(work));
	milstr_ncat(work, str_space, sizeof(work));
	milstr_ncat(work, np2version, sizeof(work));
	menudlg_settext(DID_VER, work);
}

int dlgabout_cmd(int msg, MENUID id, long param) {

	switch(msg) {
		case DLGMSG_CREATE:
			dlginit();
			break;

		case DLGMSG_COMMAND:
			switch(id) {
				case DID_OK:
					menubase_close();
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

