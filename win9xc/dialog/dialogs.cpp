#include	"compiler.h"
#include	"strres.h"
#include	"bmpdata.h"
#include	"dosio.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"dialogs.h"


static const char str_nc[] = "N/C";

static	char	pathname[MAX_PATH];
static	char	filename[MAX_PATH];

const char str_int0[] = "INT0";
const char str_int1[] = "INT1";
const char str_int2[] = "INT2";
const char str_int4[] = "INT4";
const char str_int5[] = "INT5";
const char str_int6[] = "INT6";


// ---- file select

const char *dlgs_selectfile(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size, int *ro) {

	OPENFILENAME	ofn;

	if ((defname) && (defname[0])) {
		milstr_ncpy(pathname, defname, sizeof(pathname));
	}
	else if (folder) {
		milstr_ncpy(pathname, folder, sizeof(pathname));
	}
	else {
		pathname[0] = '\0';
	}
	filename[0] = '\0';

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
	ofn.lpstrFile = pathname;
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetOpenFileName(&ofn)) {
		return(NULL);
	}
	if (folder) {
		milstr_ncpy(folder, pathname, size);
		sysmng_update(SYS_UPDATEOSCFG);
	}
	if (ro) {
		*ro = ofn.Flags & OFN_READONLY;
	}
	return(pathname);
}

const char *dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size) {

	OPENFILENAME	ofn;

	if (defname) {
		milstr_ncpy(pathname, defname, sizeof(pathname));
	}
	else {
		pathname[0] = '\0';
	}
	filename[0] = '\0';

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.lpstrFile = pathname;
	ofn.lpstrFileTitle = filename;
	ofn.nFilterIndex = item->defindex;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetSaveFileName(&ofn)) {
		return(NULL);
	}
	if (folder) {
		milstr_ncpy(folder, pathname, size);
		sysmng_update(SYS_UPDATEOSCFG);
	}
	return(pathname);
}

const char *dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size) {

	char	numfile[MAX_PATH];
	char	*p;
	int		i;

	if (folder) {
		milstr_ncpy(numfile, folder, sizeof(numfile));
	}
	else {
		numfile[0] = '\0';
	}
	file_cutname(numfile);
	p = numfile + strlen(numfile);
	for (i=0; i<10000; i++) {
		wsprintf(p, defname, i);
		if (file_attr(numfile) == -1) {
			break;
		}
	}
	return(dlgs_selectwritefile(hWnd, item, numfile, folder, size));
}


// ---- list

void dlgs_setliststr(HWND hWnd, WORD res, const char **item, UINT items) {

	HWND	wnd;
	UINT	i;

	wnd = GetDlgItem(hWnd, res);
	for (i=0; i<items; i++) {
		SendMessage(wnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)item[i]);
	}
}

void dlgs_setlistuint32(HWND hWnd, WORD res, const UINT32 *item, UINT items) {

	HWND	wnd;
	UINT	i;
	char	str[16];

	wnd = GetDlgItem(hWnd, res);
	for (i=0; i<items; i++) {
		wsprintf(str, str_u, item[i]);
		SendMessage(wnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)str);
	}
}


// ---- draw

void dlgs_drawbmp(HDC hdc, BYTE *bmp) {

	BMPFILE		*bf;
	BMPINFO		*bi;
	BMPDATA		inf;
	HBITMAP		hbmp;
	BYTE		*image;
	HDC			hmdc;

	if (bmp == NULL) {
		return;
	}
	bf = (BMPFILE *)bmp;
	bi = (BMPINFO *)(bf + 1);
	if (bmpdata_getinfo(bi, &inf) != SUCCESS) {
		goto dsdb_err1;
	}
	hbmp = CreateDIBSection(hdc, (BITMAPINFO *)bi, DIB_RGB_COLORS,
												(void **)&image, NULL, 0);
	if (hbmp == NULL) {
		goto dsdb_err1;
	}
	CopyMemory(image, bmp + (LOADINTELDWORD(bf->bfOffBits)),
													bmpdata_getdatasize(bi));
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);
	if (inf.height < 0) {
		inf.height *= -1;
	}
	BitBlt(hdc, 0, 0, inf.width, inf.height, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	DeleteObject(hbmp);

dsdb_err1:
	_MFREE(bmp);
}

