#include	"compiler.h"
#include	"dosio.h"


static	OEMCHAR	curpath[MAX_PATH];
static	OEMCHAR	*curfilep = curpath;


// ----

void dosio_init(void) { }
void dosio_term(void) { }

											// �t�@�C������
FILEH DOSIOCALL file_open(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		if ((ret = CreateFile(path, GENERIC_READ,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
			return(FILEH_INVALID);
		}
	}
	return(ret);
}

FILEH DOSIOCALL file_open_rb(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
}

FILEH DOSIOCALL file_create(const OEMCHAR *path) {

	FILEH	ret;

	if ((ret = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
						 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
											== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
}

long DOSIOCALL file_seek(FILEH handle, long pointer, int method) {

	return(SetFilePointer(handle, pointer, 0, method));
}

UINT DOSIOCALL file_read(FILEH handle, void *data, UINT length) {

	DWORD	readsize;

	if (!ReadFile(handle, data, length, &readsize, NULL)) {
		return(0);
	}
	return(readsize);
}

UINT DOSIOCALL file_write(FILEH handle, const void *data, UINT length) {

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

short DOSIOCALL file_close(FILEH handle) {

	CloseHandle(handle);
	return(0);
}

UINT DOSIOCALL file_getsize(FILEH handle) {

	return(GetFileSize(handle, NULL));
}

static BRESULT DOSIOCALL cnvdatetime(FILETIME *file, DOSDATE *dosdate, DOSTIME *dostime) {

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

short DOSIOCALL file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

	FILETIME	lastwrite;

	if ((GetFileTime(handle, NULL, NULL, &lastwrite) == 0) ||
		(cnvdatetime(&lastwrite, dosdate, dostime) != SUCCESS)) {
		return(-1);
	}
	return(0);
}

short DOSIOCALL file_delete(const OEMCHAR *path) {

	return(DeleteFile(path)?0:-1);
}

short DOSIOCALL file_attr(const OEMCHAR *path) {

	return((short)GetFileAttributes(path));
}

short DOSIOCALL file_dircreate(const OEMCHAR *path) {

	return(CreateDirectory(path, NULL)?0:-1);
}


											// �J�����g�t�@�C������
void DOSIOCALL file_setcd(const OEMCHAR *exepath) {

	file_cpyname(curpath, exepath, NELEMENTS(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

OEMCHAR * DOSIOCALL file_getcd(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(curpath);
}

FILEH DOSIOCALL file_open_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_open(curpath));
}

FILEH DOSIOCALL file_open_rb_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_open_rb(curpath));
}

FILEH DOSIOCALL file_create_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_create(curpath));
}

short DOSIOCALL file_delete_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_delete(curpath));
}

short DOSIOCALL file_attr_c(const OEMCHAR *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (curfilep - curpath));
	return(file_attr(curpath));
}


static BRESULT DOSIOCALL setflist(WIN32_FIND_DATA *w32fd, FLINFO *fli) {

#if !defined(_WIN32_WCE)
	if ((w32fd->dwFileAttributes & FILEATTR_DIRECTORY) &&
		((!file_cmpname(w32fd->cFileName, OEMTEXT("."))) ||
		(!file_cmpname(w32fd->cFileName, OEMTEXT(".."))))) {
		return(FAILURE);
	}
#endif
	fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
	fli->size = w32fd->nFileSizeLow;
	fli->attr = w32fd->dwFileAttributes;
	cnvdatetime(&w32fd->ftLastWriteTime, &fli->date, &fli->time);
	milstr_ncpy(fli->path, w32fd->cFileName, NELEMENTS(fli->path));
	return(SUCCESS);
}

FLISTH DOSIOCALL file_list1st(const OEMCHAR *dir, FLINFO *fli) {

	OEMCHAR			path[MAX_PATH];
	HANDLE			hdl;
	WIN32_FIND_DATA	w32fd;

	milstr_ncpy(path, dir, NELEMENTS(path));
	file_setseparator(path, NELEMENTS(path));
	milstr_ncat(path, OEMTEXT("*.*"), NELEMENTS(path));
	TRACEOUT(("file_list1st %s", path));
	hdl = FindFirstFile(path, &w32fd);
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

BRESULT DOSIOCALL file_listnext(FLISTH hdl, FLINFO *fli) {

	WIN32_FIND_DATA	w32fd;

	while(FindNextFile(hdl, &w32fd)) {
		if (setflist(&w32fd, fli) == SUCCESS) {
			return(SUCCESS);
		}
	}
	return(FAILURE);
}

void DOSIOCALL file_listclose(FLISTH hdl) {

	FindClose(hdl);
}


OEMCHAR * DOSIOCALL file_getname(const OEMCHAR *path) {

const OEMCHAR	*ret;
	int			csize;

	ret = path;
	while((csize = milstr_charsize(path)) != 0) {
		if (csize == 1) {
			if ((*path == '\\') || (*path == '/') || (*path == ':')) {
				ret = path + 1;
			}
		}
		path += csize;
	}
	return((OEMCHAR *)ret);
}

void DOSIOCALL file_cutname(OEMCHAR *path) {

	OEMCHAR	*p;

	p = file_getname(path);
	p[0] = '\0';
}

OEMCHAR * DOSIOCALL file_getext(const OEMCHAR *path) {

const OEMCHAR	*p;
const OEMCHAR	*q;
	int			csize;

	p = file_getname(path);
	q = NULL;
	while((csize = milstr_charsize(p)) != 0) {
		if ((csize == 1) && (*p == '.')) {
			q = p + 1;
		}
		p += csize;
	}
	if (q == NULL) {
		q = p;
	}
	return((OEMCHAR *)q);
}

void DOSIOCALL file_cutext(OEMCHAR *path) {

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

void DOSIOCALL file_cutseparator(OEMCHAR *path) {

	int		pos;

	pos = OEMSTRLEN(path) - 1;
	if ((pos > 0) &&							// 2�����ȏ�Ł[
		(path[pos] == '\\') &&					// �P�c�� \ �Ł[
		(!milstr_kanji2nd(path, pos)) &&		// ������2�o�C�g�ڂ���Ȃ��ā[
		((pos != 1) || (path[0] != '\\')) &&	// '\\' �ł͂Ȃ��ā[
		((pos != 2) || (path[1] != ':'))) {		// '?:\' �ł͂Ȃ�������
		path[pos] = '\0';
	}
}

void DOSIOCALL file_setseparator(OEMCHAR *path, int maxlen) {

	int		pos;

	pos = OEMSTRLEN(path) - 1;
	if ((pos < 0) ||
		((pos == 1) && (path[1] == ':')) ||
		((path[pos] == '\\') && (!milstr_kanji2nd(path, pos))) ||
		((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = '\\';
	path[++pos] = '\0';
}

