#include	"compiler.h"

#include	"resource.h"
#include	"np2.h"
#include	"newdisk.h"
#include	"dialog.h"
#include	"macnewdisk.h"

Boolean saveFile(OSType type, char *title, FSSpec *fsc);

#if 0
static BYTE hdddiskboot[] = {
			0xb8,0x00,0x10,0xbb,0x00,0x80,0x8e,0xd0,0x8b,0xe3,0x8c,0xc8,
			0x8e,0xd8,0xb8,0x00,0xa0,0x8e,0xc0,0xb9,0x00,0x10,0xb8,0x20,
			0x00,0x33,0xff,0xfc,0xf3,0xab,0xb0,0xe1,0xb9,0xe0,0x1f,0xaa,
			0x47,0xe2,0xfc,0xbe,0x44,0x00,0x33,0xff,0xe8,0x08,0x00,0xbf,
			0xa0,0x00,0xe8,0x02,0x00,0xeb,0xfe,0x2e,0xad,0x85,0xc0,0x74,
			0x06,0xab,0x83,0xc7,0x02,0xeb,0xf4,0xc3,0x04,0x33,0x04,0x4e,
			0x05,0x4f,0x01,0x3c,0x05,0x49,0x05,0x47,0x05,0x23,0x05,0x39,
			0x05,0x2f,0x05,0x24,0x05,0x61,0x01,0x3c,0x05,0x38,0x04,0x4f,
			0x05,0x55,0x05,0x29,0x01,0x3c,0x05,0x5e,0x05,0x43,0x05,0x48,
			0x04,0x35,0x04,0x6c,0x04,0x46,0x04,0x24,0x04,0x5e,0x04,0x3b,
			0x04,0x73,0x01,0x25,0x00,0x00,0x05,0x47,0x05,0x23,0x05,0x39,
			0x05,0x2f,0x05,0x24,0x05,0x61,0x01,0x3c,0x05,0x38,0x04,0x72,
			0x21,0x5e,0x26,0x7e,0x18,0x65,0x01,0x24,0x05,0x6a,0x05,0x3b,
			0x05,0x43,0x05,0x48,0x04,0x37,0x04,0x46,0x12,0x3c,0x04,0x35,
			0x04,0x24,0x01,0x25,0x00,0x00};
#endif

static Handle GetDlgItem(DialogPtr hWnd, short pos) {

	Handle	ret;
	Rect	rct;
	short	s;

	GetDialogItem(hWnd, pos, &s, &ret, &rct);
	return(ret);
}

static void macos_setradiobtn(ControlHandle *hRadio, int items, int pos) {

	int		i;

	for (i=0; i<items; i++) {
		SetControlValue(hRadio[i], ((i==pos)?1:0));
	}
}


// とりあえずモーダルで
void newdisk(void) {

	DialogPtr			hDlg;
	int					done;
	short				item;
	Str255				disklabel;
	ControlHandle		hmedia[2];
	int					media;

    OSType	type='.D88';
    FSSpec	fss;
    char	fileName[] = "Newdisk.d88";
    
	char	fname[MAX_PATH];
    char	label[255];

	hDlg = GetNewDialog(IDD_NEWDISK, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}

	// テキスト～
	SelectDialogItemText(hDlg, IDC_DISKLABEL, 0x0000, 0x7fff);

	// チェックボックスのハンドル
	hmedia[0] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2DD);
	hmedia[1] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2HD);
	media = 1;
	macos_setradiobtn(hmedia, 2, 1);
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				done = 1;
				break;
			case IDCANCEL:
				done = -1;
				break;
			case IDC_DISKLABEL:
				break;
			case IDC_MAKE2DD:
				media = 0;
				macos_setradiobtn(hmedia, 2, 0);
				break;
			case IDC_MAKE2HD:
				media = 1;
				macos_setradiobtn(hmedia, 2, 1);
				break;
		}
	}
	if (done > 0) {
		GetDialogItemText(GetDlgItem(hDlg, IDC_DISKLABEL), disklabel);
	}
	DisposeDialog(hDlg);

	if (done > 0) {								// making

        if (saveFile(type, fileName, &fss)) {
            fsspec2path(&fss, fname, sizeof(fname));
            mkcstr(label, sizeof(label), disklabel);
            newdisk_fdd(fname, media, label);
        }
	}
}

#if 0
static BOOL gethddsize(Str255 str, int *size) {

	int		hddsize;
	BYTE	*p;
	BYTE	c;
	UInt32	remain;

	p = (BYTE *)str;
	remain = *p++;
	while((remain) && (*p == ' ')) {
		remain--;
		p++;
	}
	hddsize = 0;
	while(remain--) {
		c = *p++;
		if ((c < '0') || (c > '9')) {
			break;
		}
		hddsize *= 10;
		hddsize += (c - '0');
	}
	if (hddsize < 5) {
		*size = 5;
		return(FAILURE);
	}
	else if (hddsize > 256) {
		*size = 256;
		return(FAILURE);
	}
	*size = hddsize;
	return(SUCCESS);
}


// とりあえずモーダルで
void newhdddisk(void) {

	DialogPtr			hDlg;
	int					done;
	short				item;
	Str255				sizestr;
	int					size;

    OSType	type='.THD';
    FSSpec	fss;
    char	fileName[] = "Newdisk.thd";
    char	fname[MAX_PATH];

	hDlg = GetNewDialog(IDD_NEWHDDDISK, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	SelectDialogItemText(hDlg, IDC_HDDSIZE, 0x0000, 0x7fff);

	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	size = 41;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				GetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
				if (gethddsize(sizestr, &size)) {
					ZeroMemory(sizestr, sizeof(sizestr));
					sprintf(((char *)sizestr) + 1, "%d", size);
					sizestr[0] = (BYTE)strlen(((char *)sizestr) + 1);
					SetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
					break;
				}
				done = 1;
				break;
			case IDCANCEL:
				done = -1;
				break;
		}
	}
	DisposeDialog(hDlg);
	if (done > 0) {								// making

        if (saveFile(type, fileName, &fss)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_hdd(fname, size);
        }
	}
}
#endif

static pascal void navEventProc( NavEventCallbackMessage sel,NavCBRecPtr parm,NavCallBackUserData ud )
{
	switch( sel )
	{
		case kNavCBEvent:
		{
			switch( parm->eventData.eventDataParms.event->what )
			{
			}
			break;
		}
	}
}

Boolean saveFile(OSType type, char *title, FSSpec *fsc)
{	
	OSType				sign='SMil';
	NavEventUPP			eventUPP;
	NavReplyRecord		reply;
	DescType			rtype;
	short				ret;
	AEKeyword			key;
	Size				len;
	NavDialogOptions	opt;
		
	InitCursor();
	NavGetDefaultDialogOptions( &opt );
    mkstr255(opt.savedFileName, title);
	opt.dialogOptionFlags+=kNavNoTypePopup;
	
	eventUPP=NewNavEventUPP( navEventProc );
	ret=NavPutFile( NULL,&reply,&opt,eventUPP,type,sign,NULL );
	DisposeNavEventUPP( eventUPP );

	if( reply.validRecord && ret==0 )
	{
		ret=AEGetNthPtr( &(reply.selection),1,typeFSS,&key,&rtype,(Ptr)fsc,(long)sizeof(FSSpec),&len );
		NavDisposeReply( &reply );
	}
        if (ret == noErr) {
            return true;
        }
	return( false );
}
