#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrnsave.h"
#include	"font.h"


static const TCHAR fontui_title[] = _T("Select font file");
static const TCHAR fontui_filter[] =									\
					_T("PC-98 BMP font (*.bmp)\0")						\
						_T("*.bmp\0")									\
					_T("Virtual98 font\0")								\
						_T("font.rom\0")								\
					_T("All supported files\0")							\
						_T("*.bmp;")									\
						_T("pc88.fnt;kanji1.rom;kanji2.rom;")			\
						_T("font.rom;")									\
						_T("subsys_c.rom;kanji.rom;")					\
						_T("fnt0808.x1;fnt0816.x1;fnt1616.x1;")			\
						_T("cgrom.dat\0");
static const FILESEL fontui = {fontui_title, tchar_bmp, fontui_filter, 3};

static const OEMCHAR bmpui_file[] = OEMTEXT("NP2_%04d.BMP");

static const TCHAR bmpui_title[] = _T("Save as bitmap file");
static const TCHAR bmpui_filter1[] =									\
					_T("1bit-bitmap (*.bmp)\0*.bmp\0")					\
					_T("Graphics Interchange Format (*.gif)\0*.gif\0");
static const TCHAR bmpui_filter4[] =									\
					_T("4bit-bitmap (*.bmp)\0*.bmp\0")					\
					_T("Graphics Interchange Format (*.gif)\0*.gif\0");
static const TCHAR bmpui_filter8[] =									\
					_T("8bit-bitmap (*.bmp)\0*.bmp\0")
					_T("Graphics Interchange Format (*.gif)\0*.gif\0");
static const TCHAR bmpui_filter24[] = _T("24bit-bitmap (*.bmp)\0*.bmp\0");
static const TCHAR *bmpui_filter[4] = {
				bmpui_filter1, bmpui_filter4, bmpui_filter8, bmpui_filter24};


void dialog_font(HWND hWnd) {

	OEMCHAR	path[MAX_PATH];

	file_cpyname(path, np2cfg.fontfile, NELEMENTS(path));
	if ((dlgs_selectfile(hWnd, &fontui, path, NELEMENTS(path), NULL)) &&
		(font_load(path, FALSE))) {
		gdcs.textdisp |= GDCSCRN_ALLDRAW2;
		milstr_ncpy(np2cfg.fontfile, path, NELEMENTS(np2cfg.fontfile));
		sysmng_update(SYS_UPDATECFG);
	}
}

void dialog_writebmp(HWND hWnd) {

	SCRNSAVE	ss;
	FILESEL		bmpui;
	OEMCHAR		path[MAX_PATH];
const OEMCHAR	*ext;

	ss = scrnsave_get();
	if (ss == NULL) {
		return;
	}
	bmpui.title = bmpui_title;
	bmpui.ext = tchar_bmp;
	bmpui.filter = bmpui_filter[ss->type];
	bmpui.defindex = 1;
	file_cpyname(path, bmpfilefolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, bmpui_file, NELEMENTS(path));
	if (dlgs_selectwritenum(hWnd, &bmpui, path, NELEMENTS(path))) {
		file_cpyname(bmpfilefolder, path, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		ext = file_getext(path);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(ext, OEMTEXT("gif")))) {
			scrnsave_writegif(ss, path, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(ext, str_bmp)) {
			scrnsave_writebmp(ss, path, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

