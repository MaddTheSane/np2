#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrnsave.h"
#include	"font.h"


static const char fontui_title[] = "Select font file";
static const char fontui_filter[] =										\
						"PC-98 BMP font (*.bmp)\0"						\
							"*.bmp\0"									\
						"Virtual98 font\0"								\
							"font.rom\0"								\
						"All supported files\0"							\
							"*.bmp;"									\
							"pc88.fnt;kanji1.rom;kanji2.rom;"			\
							"font.rom;"									\
							"subsys_c.rom;kanji.rom;"					\
							"fnt0808.x1;fnt0816.x1;fnt1616.x1;"			\
							"cgrom.dat\0";
static const FILESEL fontui = {fontui_title, str_bmp, fontui_filter, 3};

static const char bmpui_file[] = "NP2_%04d.BMP";
static const char bmpui_title[] = "Save as bitmap file";
static const char bmpui_filter1[] = "1bit-bitmap (*.bmp)\0*.bmp\0";
static const char bmpui_filter4[] = "4bit-bitmap (*.bmp)\0*.bmp\0";
static const char bmpui_filter8[] = "8bit-bitmap (*.bmp)\0*.bmp\0";
static const char bmpui_filter24[] = "24bit-bitmap (*.bmp)\0*.bmp\0";
static const char *bmpui_filter[4] = {
				bmpui_filter1, bmpui_filter4, bmpui_filter8, bmpui_filter24};


void dialog_font(HWND hWnd) {

	char	path[MAX_PATH];

	file_cpyname(path, np2cfg.fontfile, sizeof(path));
	if ((dlgs_selectfile(hWnd, &fontui, path, sizeof(path), NULL)) &&
		(font_load(path, FALSE))) {
		gdcs.textdisp |= GDCSCRN_ALLDRAW2;
		milstr_ncpy(np2cfg.fontfile, path, sizeof(np2cfg.fontfile));
		sysmng_update(SYS_UPDATECFG);
	}
}

void dialog_writebmp(HWND hWnd) {

	SCRNSAVE	ss;
	FILESEL		bmpui;
	char		path[MAX_PATH];
const char		*ext;

	ss = scrnsave_get();
	if (ss == NULL) {
		return;
	}
	bmpui.title = bmpui_title;
	bmpui.ext = str_bmp;
	bmpui.filter = bmpui_filter[ss->type];
	bmpui.defindex = 1;
	file_cpyname(path, bmpfilefolder, sizeof(path));
	file_cutname(path);
	file_catname(path, bmpui_file, sizeof(path));
	if (dlgs_selectwritenum(hWnd, &bmpui, path, sizeof(path))) {
		file_cpyname(bmpfilefolder, path, sizeof(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		ext = file_getext(path);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(ext, "gif"))) {
			scrnsave_writegif(ss, path, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(ext, str_bmp)) {
			scrnsave_writebmp(ss, path, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

