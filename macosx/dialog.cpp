#include	"compiler.h"
#include	"resource.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#if 0
#include	"newdisk.h"
#endif
#include	"font.h"
#include	"iocore.h"
#include	"np2.h"
#include	"macnewdisk.h"
#include	"scrnbmp.h"
#include	"dosio.h"

static Handle GetDlgItem(DialogPtr hWnd, short pos) {

	Handle	ret;
	Rect	rct;
	short	s;

	GetDialogItem(hWnd, pos, &s, &ret, &rct);
	return(ret);
}

void AboutDialogProc(void) {

	DialogPtr	hDlg;
	Str255		verstr;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_ABOUT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	mkstr255(verstr, np2version);
	SetDialogItemText(GetDlgItem(hDlg, IDD_VERSION), verstr);
	SetDialogDefaultItem(hDlg, IDOK);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				done = 1;
				break;
		}
	}
	DisposeDialog(hDlg);
}


// ----

static const BYTE pathsep[2] = {0x01, ':'};

static void backpalcalcat(char *dst, int leng, const BYTE *src) {

	int		dlen;
	int		slen;

	if (leng < 2) {
		return;
	}
	slen = src[0];
	dlen = strlen(dst);
	if ((slen + dlen + 1) > leng) {
		if (slen >= leng) {
			slen = leng - 1;
			dlen = 0;
		}
		else {
			dlen = leng - slen - 1;
		}
	}
	dst[slen + dlen] = '\0';
	while(dlen--) {
		dst[slen + dlen] = dst[dlen];
	}
	CopyMemory(dst, (char *)src + 1, slen);
}

void fsspec2path(FSSpec *fs, char *dst, int leng) {

	CInfoPBRec	cipbr;
	Str255		dname;

	if (!leng) {
		return;
	}
	dst[0] = '\0';
	backpalcalcat(dst, leng, fs->name);
	cipbr.dirInfo.ioVRefNum = fs->vRefNum;
	cipbr.dirInfo.ioDrParID = fs->parID;
	cipbr.dirInfo.ioFDirIndex = -1;
	cipbr.dirInfo.ioNamePtr = dname;
	do {
		cipbr.dirInfo.ioDrDirID = cipbr.dirInfo.ioDrParID;
		if (PBGetCatInfo(&cipbr, FALSE) != noErr) {
			break;
		}
		backpalcalcat(dst, leng, pathsep);
		backpalcalcat(dst, leng, dname);
	} while(cipbr.dirInfo.ioDrDirID != fsRtDirID);
}

#if 1
static NavDialogRef navWin;
static pascal void dummyproc(NavEventCallbackMessage sel, NavCBRecPtr prm,
													NavCallBackUserData ud) {
	switch( sel )
	{
        case kNavCBCancel:
        case kNavCBAccept:
            QuitAppModalLoopForWindow(NavDialogGetWindow(navWin));
            break;
        default:
            break;
	}

	(void)sel;
	(void)prm;
	(void)ud;
}

static BOOL dialog_fileselect(char *name, int size) {

	BOOL				ret;
	OSErr				err;
    NavDialogCreationOptions optNav;
	NavReplyRecord		reply;
	NavEventUPP			proc;
	long				count;
	long				i;
	FSSpec				fss;

	ret = FALSE;
    NavGetDefaultDialogCreationOptions(&optNav);
    optNav.modality=kWindowModalityWindowModal;
    optNav.parentWindow=hWndMain;
    optNav.optionFlags+=kNavNoTypePopup;
	proc = NewNavEventUPP(dummyproc);
    ret=NavCreateChooseFileDialog(&optNav,NULL,proc,NULL,NULL,NULL,&navWin);
    NavDialogRun(navWin);
    RunAppModalLoopForWindow(NavDialogGetWindow(navWin));
    NavDialogGetReply(navWin, &reply);
    NavDialogDispose(navWin);
	DisposeNavEventUPP(proc);
    
    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
	if ((!reply.validRecord) && (ret != noErr)) {
		goto fsel_exit;
	}
	err = AECountItems(&reply.selection, &count);
	if (err == noErr) {
		for (i=1; i<= count; i++) {
			err = AEGetNthPtr(&reply.selection, i, typeFSS, NULL, NULL,
													&fss, sizeof(fss), NULL);
			if (err == noErr) {
				fsspec2path(&fss, name, size);
				ret = TRUE;
				break;
			}
		}
		err = NavDisposeReply(&reply);
	}

fsel_exit:
	return(ret);
}
#else
BOOL dialog_fileselect(char *name, int size) {

	StandardFileReply	sfr;

	StandardGetFile(NULL, -1, NULL, &sfr);
	if (sfr.sfGood) {
		fsspec2path(&sfr.sfFile, name, size);
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}
#endif


void dialog_changefdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dialog_fileselect(fname, sizeof(fname))) {
			diskdrv_setfdd(drv, fname, 0);
		}
	}
}

void dialog_changehdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 2) {
		if (dialog_fileselect(fname, sizeof(fname))) {
			diskdrv_sethdd(drv, fname);
		}
	}
}


void dialog_font(void) {

    char	name[1024];

	if (dialog_fileselect(name, 1024)) {
        if ((name != NULL) && (font_load(name, FALSE))) {
            gdcs.textdisp |= GDCSCRN_ALLDRAW2;
            milstr_ncpy(np2cfg.fontfile, name, sizeof(np2cfg.fontfile));
            sysmng_update(SYS_UPDATECFG);
        }
    }
}

void dialog_writebmp(void) {

	SCRNBMP	bmp;
	char	path[MAX_PATH];
	FILEH	fh;
    FSSpec	fss;

	bmp = scrnbmp();
	if (bmp) {
		if (saveFile('BMP ', "np2.bmp", &fss)) {
            fsspec2path(&fss, path, MAX_PATH);
			fh = file_create(path);
			if (fh != FILEH_INVALID) {
				file_write(fh, bmp->ptr, bmp->size);
				file_close(fh);
			}
		}
		_MFREE(bmp);
	}
}
