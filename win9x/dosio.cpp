#include	"compiler.h"
#include	"dosio.h"


static	char	curpath[MAX_PATH];
static	char	*curfilep = curpath;

#define ISKANJI(c)	(((((c) ^ 0x20) - 0xa1) & 0xff) < 0x3c)

#if defined(UNICODE)

static HANDLE CreateFile_A(LPCSTR lpFileName,
					DWORD dwDesiredAccess,
					DWORD dwShareMode,
					LPSECURITY_ATTRIBUTES lpSecurityAttributes,
					DWORD dwCreationDisposition,
					DWORD dwFlagsAndAttributes,
					HANDLE hTemplateFile) {

	TCHAR	FileNameW[MAX_PATH*2];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpFileName, -1,
							FileNameW, sizeof(FileNameW)/sizeof(TCHAR));
	return(CreateFile(FileNameW, dwDesiredAccess, dwShareMode,
						lpSecurityAttributes, dwCreationDisposition,
						dwFlagsAndAttributes, hTemplateFile));
}

static inline BOOL DeleteFile_A(LPCSTR lpFileName) {

	TCHAR	FileNameW[MAX_PATH*2];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpFileName, -1,
							FileNameW, sizeof(FileNameW)/sizeof(TCHAR));
	return(DeleteFile(FileNameW));
}

static inline DWORD GetFileAttributes_A(LPCSTR lpFileName) {

	TCHAR	FileNameW[MAX_PATH*2];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpFileName, -1,
							FileNameW, sizeof(FileNameW)/sizeof(TCHAR));
	return(GetFileAttributes(FileNameW));
}

static inline BOOL CreateDirectory_A(LPCSTR lpFileName,
												LPSECURITY_ATTRIBUTES atr) {

	TCHAR	FileNameW[MAX_PATH*2];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpFileName, -1,
							FileNameW, sizeof(FileNameW)/sizeof(TCHAR));
	return(CreateDirectory(FileNameW, atr));
}

static inline HANDLE FindFirstFile_A(LPCSTR lpFileName,
													WIN32_FIND_DATA	*w32fd) {

	TCHAR	FileNameW[MAX_PATH*2];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpFileName, -1,
							FileNameW, sizeof(FileNameW)/sizeof(TCHAR));
	return(FindFirstFile(FileNameW, w32fd));
}
#else

#define	CreateFile_A(a, b, c, d, e, f, g)	\
								CreateFile(a, b, c, d, e, f, g)
#define DeleteFile_A(a)			DeleteFile(a)
#define	GetFileAttributes_A(a)	GetFileAttributes(a)
#define	CreateDirectory_A(a, b)	CreateDirectory(a, b)
#define FindFirstFile_A(a, b)	FindFirstFile(a, b)

#endif


// ----

void dosio_init(void) { }
void dosio_term(void) { }

											// ファイル操作
FILEH file_open(const char *path) {

	FILEH	ret;

	if ((ret = CreateFile_A(path, GENERIC_READ | GENERIC_WRITE,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		if ((ret = CreateFile_A(path, GENERIC_READ,
						0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
			return(FILEH_INVALID);
		}
	}
	return(ret);
}

FILEH file_open_rb(const char *path) {

	FILEH	ret;

	if ((ret = CreateFile_A(path, GENERIC_READ, FILE_SHARE_READ, 0,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
													== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
}

FILEH file_create(const char *path) {

	FILEH	ret;

	if ((ret = CreateFile_A(path, GENERIC_READ | GENERIC_WRITE,
						 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
											== INVALID_HANDLE_VALUE) {
		return(FILEH_INVALID);
	}
	return(ret);
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

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

	FILETIME	lastwrite;
	FILETIME	localwrite;
	SYSTEMTIME	syswrite;

	if ((GetFileTime(handle, NULL, NULL, &lastwrite) == 0) ||
		(FileTimeToLocalFileTime(&lastwrite, &localwrite) == 0) ||
		(FileTimeToSystemTime(&localwrite, &syswrite) == 0)) {
		return(-1);
	}
	if (dosdate) {
		dosdate->year = (WORD)syswrite.wYear;
		dosdate->month = (BYTE)syswrite.wMonth;
		dosdate->day = (BYTE)syswrite.wDay;
	}
	if (dostime) {
		dostime->hour = (BYTE)syswrite.wHour;
		dostime->minute = (BYTE)syswrite.wMinute;
		dostime->second = (BYTE)syswrite.wSecond;
	}
	return(0);
}

short file_delete(const char *path) {

	return(DeleteFile_A(path)?0:-1);
}

short file_attr(const char *path) {

	return((short)GetFileAttributes_A(path));
}

short file_dircreate(const char *path) {

	return(CreateDirectory_A(path, NULL)?0:-1);
}


											// カレントファイル操作
void file_setcd(const char *exepath) {

	file_cpyname(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_create(curpath));
}

short file_delete_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_delete(curpath));
}

short file_attr_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_attr(curpath));
}


FLISTH file_list1st(const char *dir, FLINFO *fli) {

	char			path[MAX_PATH];
	HANDLE			hdl;
	WIN32_FIND_DATA	w32fd;

	milsjis_ncpy(path, dir, sizeof(path));
	file_setseparator(path, sizeof(path));
	milsjis_ncat(path, "*.*", sizeof(path));
	hdl = FindFirstFile_A(path, &w32fd);
	if ((hdl != INVALID_HANDLE_VALUE) && (fli)) {
#if defined(UNICODE)
		WideCharToMultiByte(CP_ACP, 0, w32fd.cFileName, -1,
								fli->path, sizeof(fli->path), NULL, NULL);
#else
		milstr_ncpy(fli->path, w32fd.cFileName, sizeof(fli->path));
#endif
		fli->size = w32fd.nFileSizeLow;
		fli->attr = w32fd.dwFileAttributes;
	}
	return(hdl);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	WIN32_FIND_DATA	w32fd;

	if (!FindNextFile(hdl, &w32fd)) {
		return(FAILURE);
	}
	if (fli) {
#if defined(UNICODE)
		WideCharToMultiByte(CP_ACP, 0, w32fd.cFileName, -1,
								fli->path, sizeof(fli->path), NULL, NULL);
#else
		milstr_ncpy(fli->path, w32fd.cFileName, sizeof(fli->path));
#endif
		fli->size = w32fd.nFileSizeLow;
		fli->attr = w32fd.dwFileAttributes;
	}
	return(SUCCESS);
}

void file_listclose(FLISTH hdl) {

	FindClose(hdl);
}


char *file_getname(char *path) {

	char	*ret;

	ret = path;
	while(*path != '\0') {
		if (!ISKANJI(*path)) {
			if ((*path == '\\') || (*path == '/') || (*path == ':')) {
				ret = path + 1;
			}
		}
		else {
			if (path[1]) {
				path++;
			}
		}
		path++;
	}
	return(ret);
}

void file_cutname(char *path) {

	char 	*p;

	p = file_getname(path);
	p[0] = '\0';
}

char *file_getext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (!ISKANJI(*p)) {
			if (*p == '.') {
				q = p + 1;
			}
		}
		else {
			if (p[1]) {
				p++;
			}
		}
		p++;
	}
	if (!q) {
		q = p;
	}
	return(q);
}

void file_cutext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (!ISKANJI(*p)) {
			if (*p == '.') {
				q = p;
			}
		}
		else {
			if (p[1]) {
				p++;
			}
		}
		p++;
	}
	if (q) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

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

void file_setseparator(char *path, int maxlen) {

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

