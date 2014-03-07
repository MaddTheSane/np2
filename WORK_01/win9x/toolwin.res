
static const TCHAR np2tooltitle[] = _T("NP2 tool");
static const TCHAR np2toolclass[] = _T("np2-toolwin");

static const OEMCHAR str_deffont[] = OEMTEXT("�l�r �o�S�V�b�N");
static const OEMCHAR str_browse[] = OEMTEXT("...");
static const OEMCHAR str_eject[] = OEMTEXT("Eject");
static const OEMCHAR str_reset[] = OEMTEXT("Reset");
static const OEMCHAR str_power[] = OEMTEXT("Power");

static const TCHAR str_static[] = _T("STATIC");
static const TCHAR str_combobox[] = _T("COMBOBOX");
static const TCHAR str_button[] = _T("BUTTON");

static const SUBITEM defsubitem[IDC_MAXITEMS] = {
		{TCTL_STATIC,	NULL,		 49, 44,   8,   3, 0, 0},
		{TCTL_STATIC,	NULL,		 93, 19,   8,   3, 0, 0},
		{TCTL_DDLIST,	NULL,		104,  6, 248, 160, 0, 0},
		{TCTL_BUTTON,	str_browse,	352,  7,  18,  17, 0, 0},
		{TCTL_BUTTON,	str_eject,	370,  7,  34,  17, 0, 0},
		{TCTL_STATIC,	NULL,		 93, 41,   8,   3, 0, 0},
		{TCTL_DDLIST,	NULL,		104, 28, 248, 160, 0, 0},
		{TCTL_BUTTON,	str_browse,	352, 29,  18,  17, 0, 0},
		{TCTL_BUTTON,	str_eject,	370, 29,  34,  17, 0, 0},
		{TCTL_BUTTON,	str_reset,    0,  0,   0,   0, 0, 0},
		{TCTL_BUTTON,	str_power,	  0,  0,   0,   0, 0, 0}};

// ----

static const OEMCHAR skintitle[] = OEMTEXT("ToolWindow");

static const PFTBL skinini1[] = {
	PFSTR("MAIN",		PFTYPE_STR,		toolskin.main),
	PFSTR("FONT",		PFTYPE_STR,		toolskin.font),
	PFVAL("FONTSIZE",	PFTYPE_SINT32,	&toolskin.fontsize),
	PFVAL("COLOR1",		PFTYPE_HEX32,	&toolskin.color1),
	PFVAL("COLOR2",		PFTYPE_HEX32,	&toolskin.color2)};

static const PFTBL skinini2[] = {
	PFEXT("HDDACC",		PFTYPE_ARGS16,	&subitem[IDC_TOOLHDDACC].posx,		5),
	PFEXT("FD1ACC",		PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD1ACC].posx,		5),
	PFEXT("FD1LIST",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD1LIST].posx,	5),
	PFEXT("FD1BROWSE",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD1BROWSE].posx,	5),
	PFEXT("FD1EJECT",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD1EJECT].posx,	5),
	PFEXT("FD2ACC",		PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD2ACC].posx,		5),
	PFEXT("FD2LIST",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD2LIST].posx,	5),
	PFEXT("FD2BROWSE",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD2BROWSE].posx,	5),
	PFEXT("FD2EJECT",	PFTYPE_ARGS16,	&subitem[IDC_TOOLFDD2EJECT].posx,	5),
	PFEXT("RESETBTN",	PFTYPE_ARGS16,	&subitem[IDC_TOOLRESET].posx,		5),
	PFEXT("POWERBTN",	PFTYPE_ARGS16,	&subitem[IDC_TOOLPOWER].posx,		5)
};


// static const DWORD mvccol[MVC_MAXCOLOR] = {
//						0xc0e8f8, 0xd8ecf4, 0x48a8c8, 0x000000};


// ----

static const TCHAR str_skindef[] = _T("<&Base Skin>");
static const TCHAR str_skinsel[] = _T("&Select Skin...");
static const TCHAR str_toolskin[] = _T("&Skins");
static const TCHAR str_toolclose[] = _T("&Close");

static const TCHAR skinui_title[] = _T("Select skin file");
static const TCHAR skinui_filter[] =								\
					_T("ini files (*.ini)\0*.ini\0")				\
					_T("text files (*.txt)\0*.txt\0")				\
					_T("All files (*.*)\0*.*\0");
static const TCHAR skinui_ext[] = _T("ini");
static const FILESEL skinui = {skinui_title, skinui_ext, skinui_filter, 1};

