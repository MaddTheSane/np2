
static const char np2toolclass[] = "np2-toolwin";
static const char np2tooltitle[] = "NP2 tool";

static const char str_deffont[] = "‚l‚r ‚oƒSƒVƒbƒN";
static const char str_browse[] = "...";
static const char str_eject[] = "Eject";

static const char str_static[] = "STATIC";
static const char str_combobox[] = "COMBOBOX";
static const char str_button[] = "BUTTON";

static const SUBITEM defsubitem[IDC_MAXITEMS] = {
		{str_static,	NULL,		WS_VISIBLE,		47, 44, 8, 3, FALSE},
		{str_static,	NULL,		WS_VISIBLE,		93, 19, 8, 3, FALSE},
		{str_combobox,	NULL,		WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
													104, 6, 248, 160, TRUE},
		{str_button,	str_browse,	WS_VISIBLE | BS_PUSHBUTTON,	
													352, 7, 18, 17, TRUE},
		{str_button,	str_eject,	WS_VISIBLE | BS_PUSHBUTTON,
													370, 7, 34, 17, TRUE},
		{str_static,	NULL,		WS_VISIBLE,		93, 41, 8, 3, FALSE},
		{str_combobox,	NULL,		WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
													104, 28, 248, 160, TRUE},
		{str_button,	str_browse,	WS_VISIBLE | BS_PUSHBUTTON,
													352, 29, 18, 17, TRUE},
		{str_button,	str_eject,	WS_VISIBLE | BS_PUSHBUTTON,
													370, 29, 34, 17, TRUE}};


// ----

static const char np2skin[] = "np2skin.ini";
static const char skintitle[] = "ToolWindow";

static const INITBL skinini[] = {
	{"MAIN",		INITYPE_STR,	toolskin.main,	sizeof(toolskin.main)},
	{"FONT",		INITYPE_STR,	toolskin.font,	sizeof(toolskin.font)},
	{"FONTSIZE",	INITYPE_SINT32,	&toolskin.fontsize,					0},
	{"COLOR1",		INITYPE_HEX32,	&toolskin.color1,					0},
	{"COLOR2",		INITYPE_HEX32,	&toolskin.color2,					0},
	{"HDDACC",		INITYPE_ARGS16,	&subitem[IDC_TOOLHDDACC].posx,		4},
	{"FD1ACC",		INITYPE_ARGS16,	&subitem[IDC_TOOLFDD1ACC].posx,		4},
	{"FD1LIST",		INITYPE_ARGS16,	&subitem[IDC_TOOLFDD1LIST].posx,	4},
	{"FD1BROWSE",	INITYPE_ARGS16,	&subitem[IDC_TOOLFDD1BROWSE].posx,	4},
	{"FD1EJECT",	INITYPE_ARGS16,	&subitem[IDC_TOOLFDD1EJECT].posx,	4},
	{"FD2ACC",		INITYPE_ARGS16,	&subitem[IDC_TOOLFDD2ACC].posx,		4},
	{"FD2LIST",		INITYPE_ARGS16,	&subitem[IDC_TOOLFDD2LIST].posx,	4},
	{"FD2BROWSE",	INITYPE_ARGS16,	&subitem[IDC_TOOLFDD2BROWSE].posx,	4},
	{"FD2EJECT",	INITYPE_ARGS16,	&subitem[IDC_TOOLFDD2EJECT].posx,	4}};


// static const DWORD mvccol[MVC_MAXCOLOR] = {
//						0xc0e8f8, 0xd8ecf4, 0x48a8c8, 0x000000};

