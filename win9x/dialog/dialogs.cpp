#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"commng.h"
#include	"dialogs.h"


static const char str_nc[] = "N/C";

// static	char	pathname[MAX_PATH];
// static	char	filename[MAX_PATH];

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


// ---- mimpi def file

static const char mimpi_title[] = "Open MIMPI define file";
static const char mimpi_ext[] = "def";
static const char mimpi_filter[] = "MIMPI define file(*.def)\0*.def\0";
static const FILESEL mimpi = {mimpi_title, mimpi_ext, mimpi_filter, 1};

void dlgs_browsemimpidef(HWND hWnd, WORD res) {

	HWND	subwnd;
	char	path[MAX_PATH];
const char	*p;

	subwnd = GetDlgItem(hWnd, res);
	GetWindowText(subwnd, path, sizeof(path));
	if (dlgs_selectfile(hWnd, &mimpi, path, sizeof(path), NULL)) {
		p = path;
	}
	else {
		p = str_null;
	}
	SetWindowText(subwnd, p);
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


// ---- MIDI�f�o�C�X�̃��X�g

void dlgs_setlistmidiout(HWND hWnd, WORD res, const char *defname) {

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

void dlgs_setlistmidiin(HWND hWnd, WORD res, const char *defname) {

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

void dlgs_linex(BYTE *image, int x, int y, int l, int align, BYTE c) {

	image -= y * align;
	while(l--) {
		if (x & 1) {
			image[x/2] &= 0xf0;
			image[x/2] |= c;
		}
		else {
			image[x/2] &= 0x0f;
			image[x/2] |= (c << 4);
		}
		x++;
	}
}

void dlgs_liney(BYTE *image, int x, int y, int l, int align, BYTE c) {

	image += (x / 2) - y * align;
	if (x & 1) {
		while(l--) {
			*image &= 0xf0;
			*image |= c;
			image -= align;
		}
	}
	else {
		c <<= 4;
		while(l--) {
			*image &= 0x0f;
			*image |= c;
			image -= align;
		}
	}
}


// ---- jumper

void dlgs_setjumperx(BYTE *image, int x, int y, int align) {

	int		i;

	x *= 9;
	y *= 9;
	for (i=0; i<2; i++) {
		dlgs_linex(image, x, y+0+i, 19, align, 0);
		dlgs_linex(image, x, y+8+i, 19, align, 0);
		dlgs_liney(image, x+ 0+i, y, 9, align, 0);
		dlgs_liney(image, x+17+i, y, 9, align, 0);
	}
}

void dlgs_setjumpery(BYTE *image, int x, int y, int align) {

	int		i;

	x *= 9;
	y *= 9;
	for (i=0; i<2; i++) {
		dlgs_linex(image, x, y+ 0+i, 9, align, 0);
		dlgs_linex(image, x, y+17+i, 9, align, 0);
		dlgs_liney(image, x+0+i, y, 19, align, 0);
		dlgs_liney(image, x+8+i, y, 19, align, 0);
	}
}

