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
#include	"scrnbmp.h"
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
static const char *bmpui_filter[3] = {
								bmpui_filter1, bmpui_filter4, bmpui_filter8};


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

	SCRNBMP	bmp;
	FILESEL	bmpui;
	char	path[MAX_PATH];
	FILEH	fh;

	bmp = scrnbmp();
	if (bmp) {
		bmpui.title = bmpui_title;
		bmpui.ext = str_bmp;
		bmpui.filter = bmpui_filter[bmp->type];
		bmpui.defindex = 1;
		file_cpyname(path, bmpfilefolder, sizeof(path));
		file_cutname(path);
		file_catname(path, bmpui_file, sizeof(path));
		if (dlgs_selectwritenum(hWnd, &bmpui, path, sizeof(path))) {
			file_cpyname(bmpfilefolder, path, sizeof(bmpfilefolder));
			sysmng_update(SYS_UPDATEOSCFG);
			fh = file_create(path);
			if (fh != FILEH_INVALID) {
				file_write(fh, bmp->ptr, bmp->size);
				file_close(fh);
			}
		}
		_MFREE(bmp);
	}
}

