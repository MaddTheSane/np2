#include	"compiler.h"
#include	"resource.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"newdisk.h"


void AboutDialogProc(void) {

	DialogPtr	hDlg;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_ABOUT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
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

static void fsspec2path(FSSpec *fs, char *dst, int leng) {

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
static pascal void dummyproc(NavEventCallbackMessage sel, NavCBRecPtr prm,
													NavCallBackUserData ud) {

	(void)sel;
	(void)prm;
	(void)ud;
}

static BOOL dialog_fileselect(char *name, int size) {

	BOOL				ret;
	OSErr				err;
	NavDialogOptions	opt;
	NavReplyRecord		reply;
	NavEventUPP			proc;
	long				count;
	long				i;
	FSSpec				fss;

	ret = FALSE;
	err = NavGetDefaultDialogOptions(&opt);
	if (err != noErr) {
		goto fsel_exit;
	}
	opt.dialogOptionFlags &= ~kNavAllowPreviews;
	opt.dialogOptionFlags &= ~kNavAllowMultipleFiles;
	proc = NewNavEventUPP(dummyproc);
	err = NavGetFile(NULL, &reply, &opt, proc, NULL, NULL, NULL, NULL);
	DisposeNavEventUPP(proc);
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

