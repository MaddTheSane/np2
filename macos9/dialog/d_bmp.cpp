#include	"compiler.h"
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


void dialog_font(void) {

	char	path[MAX_PATH];

	if (dlgs_selectfile(path, sizeof(path))) {
		if (font_load(path, FALSE)) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			milstr_ncpy(np2cfg.fontfile, path, sizeof(np2cfg.fontfile));
			sysmng_update(SYS_UPDATECFG);
		}
	}
}

void dialog_writebmp(void) {

	SCRNBMP	bmp;
	char	path[MAX_PATH];
	FILEH	fh;

	bmp = scrnbmp();
	if (bmp) {
		if (dlgs_selectwritefile(path, sizeof(path), "np2.bmp")) {
			fh = file_create(path);
			if (fh != FILEH_INVALID) {
				file_write(fh, bmp->ptr, bmp->size);
				file_close(fh);
			}
		}
		_MFREE(bmp);
	}
}

