#include	"compiler.h"
#include	"strres.h"
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

