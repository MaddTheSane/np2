#include	"compiler.h"
#include	"strres.h"
#include	"bmpdata.h"
#include	"dosio.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"dialogs.h"


static const char str_nc[] = "N/C";

const char str_int0[] = "INT0";
const char str_int1[] = "INT1";
const char str_int2[] = "INT2";
const char str_int4[] = "INT4";
const char str_int5[] = "INT5";
const char str_int6[] = "INT6";


// ---- file select

BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
											char *path, UINT size, int *ro) {

	OPENFILENAME	ofn;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetOpenFileName(&ofn)) {
		return(FALSE);
	}
	if (ro) {
		*ro = ofn.Flags & OFN_READONLY;
	}
	return(TRUE);
}

BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
											char *path, UINT size) {

	OPENFILENAME	ofn;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = item->title;
	if (!GetSaveFileName(&ofn)) {
		return(FALSE);
	}
	return(TRUE);
}

BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
											char *path, UINT size) {

	char	*file;
	char	*p;
	char	*q;
	UINT	i;
	BOOL	r;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	file = (char *)_MALLOC(size + 16, path);
	if (file == NULL) {
		return(FALSE);
	}
	p = file_getname(path);
	milstr_ncpy(file, path, size);
	file_cutname(file);
	q = file + strlen(file);

	for (i=0; i<10000; i++) {
		SPRINTF(q, p, i);
		if (file_attr(file) == (short)-1) {
			break;
		}
	}
	r = dlgs_selectwritefile(hWnd, item, file, size);
	if (r) {
		milstr_ncpy(path, file, size);
	}
	_MFREE(file);
	return(r);
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

