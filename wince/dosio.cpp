#include	"compiler.h"
#if defined(UNICODE) && defined(OSLANG_UTF8)
#include	"codecnv.h"
#endif
#include	"dosio.h"


static	OEMCHAR	curpath[MAX_PATH];
static	OEMCHAR	*curfilep = curpath;


// ----

void dosio_init(void) { }
void dosio_term(void) { }


											// ファイル操作
FILEH file_open(const OEMCHAR *path) {

	FILEH	ret;

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif

	ret = CreateFile(oempath, GENERIC_READ | GENERIC_WRITE,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ret == INVALID_HANDLE_VALUE) {
		ret = CreateFile(oempath, GENERIC_READ,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ret == INVALID_HANDLE_VALUE) {
			return(FILEH_INVALID);
		}
	}
	return(ret);
}

FILEH file_open_rb(const OEMCHAR *path) {

	FILEH	ret;

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif

	ret = CreateFile(oempath, GENERIC_READ, FILE_SHARE_READ, 0,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ret != INVALID_HANDLE_VALUE) {
		return(ret);
	}
	else {
		return(FILEH_INVALID);
	}
}

FILEH file_create(const OEMCHAR *path) {

	FILEH	ret;

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif
	ret = CreateFile(oempath, GENERIC_READ | GENERIC_WRITE,
						 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ret != INVALID_HANDLE_VALUE) {
		return(ret);
	}
	else {
		return(FILEH_INVALID);
	}
}

long file_seek(FILEH handle, long pointer, int method) {

	return(SetFilePointer(handle, pointer, 0, method));
}

UINT file_read(FILEH handle, void *data, UINT length) {

	DWORD	readsize;

	if (!ReadFile(handle, data, length, &readsize, NULL)) {
		return(0);
	}
	return(readsize);
}

UINT file_write(FILEH handle, const void *data, UINT length) {

	DWORD	writesize;

	if (length) {
		if (WriteFile(handle, data, length, &writesize, NULL)) {
			return(writesize);
		}
	}
	else {
		SetEndOfFile(handle);
	}
	return(0);
}

short file_close(FILEH handle) {

	CloseHandle(handle);
	return(0);
}

UINT file_getsize(FILEH handle) {

	return(GetFileSize(handle, NULL));
}

static BOOL cnvdatetime(FILETIME *file, DOSDATE *dosdate, DOSTIME *dostime) {

	FILETIME	localtime;
	SYSTEMTIME	systime;

	if ((FileTimeToLocalFileTime(file, &localtime) == 0) ||
		(FileTimeToSystemTime(&localtime, &systime) == 0)) {
		return(FAILURE);
	}
	if (dosdate) {
		dosdate->year = (UINT16)systime.wYear;
		dosdate->month = (UINT8)systime.wMonth;
		dosdate->day = (UINT8)systime.wDay;
	}
	if (dostime) {
		dostime->hour = (UINT8)systime.wHour;
		dostime->minute = (UINT8)systime.wMinute;
		dostime->second = (UINT8)systime.wSecond;
	}
	return(SUCCESS);
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

	FILETIME	lastwrite;

	if ((GetFileTime(handle, NULL, NULL, &lastwrite) == 0) ||
		(cnvdatetime(&lastwrite, dosdate, dostime) != SUCCESS)) {
		return(-1);
	}
	return(0);
}

short file_delete(const OEMCHAR *path) {

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif
	return(DeleteFile(oempath)?0:-1);
}

short file_attr(const OEMCHAR *path) {

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif
	return((short)GetFileAttributes(oempath));
}

short file_dircreate(const OEMCHAR *path) {

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	oempath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
												oempath, NELEMENTS(oempath));
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	oempath[MAX_PATH];
	codecnv_utf8toucs2(oempath, NELEMENTS(oempath), path, (UINT)-1);
#else
const OEMCHAR *oempath;
	oempath = path;
#endif
	return(CreateDirectory(oempath, NULL)?0:-1);
}


											// カレントファイル操作
void file_setcd(const OEMCHAR *exepath) {

	file_cpyname(curpath, exepath, NELEMENTS(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(curpath);
}

FILEH file_open_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_create(curpath));
}

short file_delete_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_delete(curpath));
}

short file_attr_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_attr(curpath));
}


// ----

#if !defined(_WIN32_WCE)
static const OEMCHAR str_selfdir[] = OEMTEXT(".");
static const OEMCHAR str_parentdir[] = OEMTEXT("..");
#endif
static const OEMCHAR str_wildcard[] = OEMTEXT("*.*");

static BRESULT setflist(WIN32_FIND_DATA *w32fd, FLINFO *fli) {

#if defined(UNICODE) && defined(OSLANG_SJIS)
	WideCharToMultiByte(CP_ACP, 0, w32fd->cFileName, -1,
								fli->path, NELEMENTS(fli->path), NULL, NULL);
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	codecnv_ucs2toutf8(fli->path, NELEMENTS(fli->path), w32fd->cFileName, -1);
#else
	file_cpyname(fli->path, w32fd->cFileName, NELEMENTS(fli->path));
#endif
#if !defined(_WIN32_WCE)
	if ((w32fd->dwFileAttributes & FILEATTR_DIRECTORY) &&
		((!file_cmpname(fli->path, str_selfdir)) ||
		(!file_cmpname(fli->path, str_parentdir)))) {
		return(FAILURE);
	}
#endif
	fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
	fli->size = w32fd->nFileSizeLow;
	fli->attr = w32fd->dwFileAttributes;
	cnvdatetime(&w32fd->ftLastWriteTime, &fli->date, &fli->time);
	return(SUCCESS);
}

FLISTH file_list1st(const OEMCHAR *dir, FLINFO *fli) {

	OEMCHAR			path[MAX_PATH];
	HANDLE			hdl;
	WIN32_FIND_DATA	w32fd;

	file_cpyname(path, dir, NELEMENTS(path));
	file_setseparator(path, NELEMENTS(path));
	file_catname(path, str_wildcard, NELEMENTS(path));
	TRACEOUT(("file_list1st %s", path));

#if defined(UNICODE) && defined(OSLANG_SJIS)
	UINT16	ucs2[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1,
													ucs2, NELEMENTS(ucs2));
	hdl = FindFirstFile(ucs2, &w32fd);
#elif defined(UNICODE) && defined(OSLANG_UTF8)
	UINT16	ucs2[MAX_PATH];
	codecnv_utf8toucs2(ucs2, NELEMENTS(ucs2), path, (UINT)-1);
	hdl = FindFirstFile(ucs2, &w32fd);
#else
	hdl = FindFirstFile(path, &w32fd);
#endif
	if (hdl != INVALID_HANDLE_VALUE) {
		do {
			if (setflist(&w32fd, fli) == SUCCESS) {
				return(hdl);
			}
		} while(FindNextFile(hdl, &w32fd));
		FindClose(hdl);
	}
	return(FLISTH_INVALID);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	WIN32_FIND_DATA	w32fd;

	while(FindNextFile(hdl, &w32fd)) {
		if (setflist(&w32fd, fli) == SUCCESS) {
			return(SUCCESS);
		}
	}
	return(FAILURE);
}

void file_listclose(FLISTH hdl) {

	FindClose(hdl);
}


OEMCHAR *file_getname(const OEMCHAR *path) {

	int			csize;
const OEMCHAR	*ret;

	ret = path;
	while((csize = milstr_charsize(path)) != 0) {
		if ((csize == 1) &&
			((*path == '\\') || (*path == '/') || (*path == ':'))) {
			ret = path + 1;
		}
		path += csize;
	}
	return((OEMCHAR *)ret);
}

void file_cutname(OEMCHAR *path) {

	OEMCHAR	*p;

	p = file_getname(path);
	p[0] = '\0';
}

OEMCHAR *file_getext(const OEMCHAR *path) {

	OEMCHAR	*p;
	OEMCHAR	*q;
	int		csize;

	p = file_getname(path);
	q = NULL;
	while((csize = milstr_charsize(p)) != 0) {
		if ((csize == 1) && (*p == '.')) {
			q = p + 1;
		}
		p += csize;
	}
	if (!q) {
		q = p;
	}
	return((OEMCHAR *)q);
}

void file_cutext(OEMCHAR *path) {

	OEMCHAR	*p;
	OEMCHAR	*q;
	int		csize;

	p = file_getname(path);
	q = NULL;
	while((csize = milstr_charsize(p)) != 0) {
		if ((csize == 1) && (*p == '.')) {
			q = p;
		}
		p += csize;
	}
	if (q) {
		*q = '\0';
	}
}

void file_cutseparator(OEMCHAR *path) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos > 0) &&							// 2文字以上でー
		(path[pos] == '\\') &&					// ケツが \ でー
		(!milstr_kanji2nd(path, pos)) &&		// 漢字の2バイト目ぢゃなくてー
		((pos != 1) || (path[0] != '\\')) &&	// '\\' ではなくてー
		((pos != 2) || (path[1] != ':'))) {		// '?:\' ではなかったら
		path[pos] = '\0';
	}
}

void file_setseparator(OEMCHAR *path, int maxlen) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos < 0) ||
		((pos == 1) && (path[1] == ':')) ||
		((path[pos] == '\\') && (!milstr_kanji2nd(path, pos))) ||
		((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = '\\';
	path[++pos] = '\0';
}

