#include	"compiler.h"
#include	"strres.h"
#include	"bmpdata.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"commng.h"
#include	"dialogs.h"
#if defined(MT32SOUND_DLL)
#include	"mt32snd.h"
#endif

extern HINSTANCE hInst;


const TCHAR str_nc[] = _T("N/C");

const TCHAR str_int0[] = _T("INT0");
const TCHAR str_int1[] = _T("INT1");
const TCHAR str_int2[] = _T("INT2");
const TCHAR str_int4[] = _T("INT4");
const TCHAR str_int5[] = _T("INT5");
const TCHAR str_int6[] = _T("INT6");


// ---- enable

void dlgs_enablebyautocheck(HWND hWnd, UINT uID, UINT uCheckID)
{
	EnableWindow(GetDlgItem(hWnd, uID),
			(SendDlgItemMessage(hWnd, uCheckID, BM_GETCHECK, 0, 0) != 0));
}

void dlgs_disablebyautocheck(HWND hWnd, UINT uID, UINT uCheckID)
{
	EnableWindow(GetDlgItem(hWnd, uID),
			(SendDlgItemMessage(hWnd, uCheckID, BM_GETCHECK, 0, 0) == 0));

}


// ---- file select

BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
										OEMCHAR *path, UINT size, int *ro) {

	TCHAR			*pszTitle;
	OPENFILENAME	ofn;
#if defined(OSLANG_UTF8)
	TCHAR			_path[MAX_PATH];
#endif
	BOOL			bResult;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	pszTitle = lockstringresource(hInst, item->title);

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
#if defined(OSLANG_UTF8)
	oemtotchar(_path, NELEMENTS(_path), path, -1);
	ofn.lpstrFile = _path;
	ofn.nMaxFile = NELEMENTS(_path);
#else
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
#endif
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = pszTitle;

	bResult = GetOpenFileName(&ofn);
	unlockstringresource(pszTitle);

	if (bResult) {
#if defined(OSLANG_UTF8)
		tchartooem(path, NELEMENTS(path), _path, -1);
#endif
		if (ro) {
			*ro = ofn.Flags & OFN_READONLY;
		}
	}
	return(bResult);
}

BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
											OEMCHAR *path, UINT size) {

	OPENFILENAME	ofn;
	TCHAR			*pszTitle;
#if defined(OSLANG_UTF8)
	TCHAR			_path[MAX_PATH];
#endif
	BOOL			bResult;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}

	pszTitle = lockstringresource(hInst, item->title);

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = item->filter;
	ofn.nFilterIndex = item->defindex;
#if defined(OSLANG_UTF8)
	oemtotchar(_path, NELEMENTS(_path), path, -1);
	ofn.lpstrFile = _path;
	ofn.nMaxFile = NELEMENTS(_path);
#else
	ofn.lpstrFile = path;
	ofn.nMaxFile = size;
#endif
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = item->ext;
	ofn.lpstrTitle = pszTitle;

	bResult = GetSaveFileName(&ofn);

	unlockstringresource(pszTitle);

#if defined(OSLANG_UTF8)
	if (bResult) {
		tchartooem(path, NELEMENTS(path), _path, -1);
	}
#endif
	return(bResult);
}

BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
											OEMCHAR *path, UINT size) {

	OEMCHAR	*file;
	OEMCHAR	*p;
	OEMCHAR	*q;
	UINT	i;
	BOOL	r;

	if ((item == NULL) || (path == NULL) || (size == 0)) {
		return(FALSE);
	}
	file = (OEMCHAR *)_MALLOC((size + 16) * sizeof(OEMCHAR), path);
	if (file == NULL) {
		return(FALSE);
	}
	p = file_getname(path);
	milstr_ncpy(file, path, size);
	file_cutname(file);
	q = file + OEMSTRLEN(file);

	for (i=0; i<10000; i++) {
		OEMSPRINTF(q, p, i);
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


// ---- mimpi def file

static const TCHAR mimpi_title[] = _T("Open MIMPI define file");
static const TCHAR mimpi_ext[] = _T("def");
static const TCHAR mimpi_filter[] = _T("MIMPI define file(*.def)\0*.def\0");
static const FILESEL mimpi = {mimpi_title, mimpi_ext, mimpi_filter, 1};

void dlgs_browsemimpidef(HWND hWnd, UINT16 res) {

	HWND		subwnd;
	OEMCHAR		path[MAX_PATH];
const OEMCHAR	*p;

	subwnd = GetDlgItem(hWnd, res);
	GetWindowText(subwnd, path, NELEMENTS(path));
	if (dlgs_selectfile(hWnd, &mimpi, path, NELEMENTS(path), NULL)) {
		p = path;
	}
	else {
		p = str_null;
	}
	SetWindowText(subwnd, p);
}


// ---- list

void dlgs_setliststr(HWND hWnd, UINT16 res, const TCHAR **item, UINT items) {

	HWND	wnd;
	UINT	i;

	wnd = GetDlgItem(hWnd, res);
	for (i=0; i<items; i++) {
		SendMessage(wnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)item[i]);
	}
}

void dlgs_setlistuint32(HWND hWnd, UINT16 res, const UINT32 *item, UINT items) {
	HWND	wnd;
	UINT	i;
	OEMCHAR	str[16];

	wnd = GetDlgItem(hWnd, res);
	for (i=0; i<items; i++) {
		OEMSPRINTF(str, str_u, item[i]);
		SendMessage(wnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)str);
	}
}


void dlgs_setdroplistitem(HWND hWnd, UINT uID,
										const TCHAR **ppszItem, UINT uItems)
{
	HWND	hItem;
	UINT	uPos;
	UINT	i;
	TCHAR	szString[128];

	hItem = GetDlgItem(hWnd, uID);
	uPos = 0;
	for (i=0; i<uItems; i++) {
		LPCTSTR lpcszStr = ppszItem[i];
		if (!HIWORD(lpcszStr))
		{
			if (!loadstringresource(hInst, LOWORD(lpcszStr),
											szString, NELEMENTS(szString)))
			{
				continue;
			}
			lpcszStr = szString;
		}
		SendMessage(hItem, CB_INSERTSTRING, (WPARAM)uPos, (LPARAM)lpcszStr);
		SendMessage(hItem, CB_SETITEMDATA, (WPARAM)uPos, (LPARAM)i);
		uPos++;
	}
}

void dlgs_setdroplistnumber(HWND hWnd, UINT uID, int nNumber)
{
	HWND	hItem;
	int		nItems;
	int		i;

	hItem = GetDlgItem(hWnd, uID);
	nItems = SendMessage(hItem, CB_GETCOUNT, 0, 0);
	for (i=0; i<nItems; i++)
	{
		if (SendMessage(hItem, CB_GETITEMDATA, (WPARAM)i, 0) == nNumber)
		{
			SendMessage(hItem, CB_SETCURSEL, (WPARAM)i, 0);
			break;
		}
	}
}

int dlgs_getdroplistnumber(HWND hWnd, UINT uID)
{
	HWND	hItem;
	int		nPos;

	hItem = GetDlgItem(hWnd, uID);
	nPos = SendMessage(hItem, CB_GETCURSEL, 0, 0);
	if (nPos >= 0)
	{
		return SendMessage(hItem, CB_GETITEMDATA, (WPARAM)nPos, 0);
	}
	return -1;
}


// ---- MIDIデバイスのリスト

void dlgs_setlistmidiout(HWND hWnd, UINT16 res, const OEMCHAR *defname) {

	HWND		wnd;
	UINT		defcur;
	UINT		devs;
	UINT		num;
	UINT		i;
	MIDIOUTCAPS	moc;

	wnd = GetDlgItem(hWnd, res);
	defcur = 0;
	devs = midiOutGetNumDevs();
	SendMessage(wnd, CB_INSERTSTRING, (WPARAM)0, (LPARAM)str_nc);
	SendMessage(wnd, CB_INSERTSTRING, (WPARAM)1, (LPARAM)cmmidi_midimapper);
	if (!milstr_cmp(defname, cmmidi_midimapper)) {
		defcur = 1;
	}
	num = 2;
#if defined(VERMOUTH_LIB)
	SendMessage(wnd, CB_INSERTSTRING, (WPARAM)num, (LPARAM)cmmidi_vermouth);
	if (!milstr_cmp(defname, cmmidi_vermouth)) {
		defcur = num;
	}
	num++;
#endif
#if defined(MT32SOUND_DLL)
	if (mt32sound_isenable()) {
		SendMessage(wnd, CB_INSERTSTRING, (WPARAM)num,
													(LPARAM)cmmidi_mt32sound);
		if (!milstr_cmp(defname, cmmidi_mt32sound)) {
			defcur = num;
		}
		num++;
	}
#endif
	for (i=0; i<devs; i++) {
		if (midiOutGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR) {
			SendMessage(wnd, CB_INSERTSTRING,
											(WPARAM)num, (LPARAM)moc.szPname);
			if ((!defcur) && (!milstr_cmp(defname, moc.szPname))) {
				defcur = num;
			}
			num++;
		}
	}
	SendMessage(wnd, CB_SETCURSEL, (WPARAM)defcur, (LPARAM)0);
}

void dlgs_setlistmidiin(HWND hWnd, UINT16 res, const OEMCHAR *defname) {

	HWND		wnd;
	UINT		defcur;
	UINT		num;
	UINT		i;
	MIDIINCAPS	mic;

	wnd = GetDlgItem(hWnd, res);
	defcur = 0;
	num = midiInGetNumDevs();
	SendMessage(wnd, CB_INSERTSTRING, (WPARAM)0, (LPARAM)str_nc);
	for (i=0; i<num; i++) {
		if (midiInGetDevCaps(i, &mic, sizeof(mic)) == MMSYSERR_NOERROR) {
			SendMessage(wnd, CB_INSERTSTRING,
									(WPARAM)(i+1), (LPARAM)mic.szPname);
			if ((!defcur) && (!milstr_cmp(defname, mic.szPname))) {
				defcur = (i+1);
			}
		}
	}
	SendMessage(wnd, CB_SETCURSEL, (WPARAM)defcur, (LPARAM)0);
}



// ---- draw

void dlgs_drawbmp(HDC hdc, UINT8 *bmp) {

	BMPFILE		*bf;
	BMPINFO		*bi;
	BMPDATA		inf;
	HBITMAP		hbmp;
	UINT8		*image;
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


// ----

BOOL dlgs_getitemrect(HWND hWnd, UINT uID, RECT *pRect)
{
	HWND	hItem;
	POINT	pt;

	if (pRect == NULL)
	{
		return FALSE;
	}
	hItem = GetDlgItem(hWnd, uID);
	if (!GetWindowRect(hItem, pRect))
	{
		return FALSE;
	}
	ZeroMemory(&pt, sizeof(pt));
	if (!ClientToScreen(hWnd, &pt))
	{
		return FALSE;
	}
	pRect->left -= pt.x;
	pRect->top -= pt.y;
	pRect->right -= pt.x;
	pRect->bottom -= pt.y;
	return TRUE;
}

