#include	"compiler.h"
#include	<sys/stat.h>
#include	<time.h>
#include	"dosio.h"
#if defined(WIN32)
#include	<direct.h>
#else
#include	<dirent.h>
#endif
#if 0
#include <sys/param.h>
#include <unistd.h>
#endif

static	char	curpath[MAX_PATH];
static	char	*curfilep = curpath;


void dosio_init(void) {
}

void dosio_term(void) {
}

/* �t�@�C������ */
FILEH file_open(const char *path) {

	return(fopen(path, "rb+"));
}

FILEH file_open_rb(const char *path) {

	return(fopen(path, "rb+"));
}

FILEH file_create(const char *path) {

	return(fopen(path, "wb+"));
}

long file_seek(FILEH handle, long pointer, int method) {

	fseek(handle, pointer, method);
	return(ftell(handle));
}

UINT file_read(FILEH handle, void *data, UINT length) {

	return((UINT)fread(data, 1, length, handle));
}

UINT file_write(FILEH handle, const void *data, UINT length) {

	return((UINT)fwrite(data, 1, length, handle));
}

short file_close(FILEH handle) {

	fclose(handle);
	return(0);
}

UINT file_getsize(FILEH handle) {

	struct stat sb;

	if (fstat(fileno(handle), &sb) == 0) {
		return(sb.st_size);
	}
	return(0);
}

short file_attr(const char *path) {

struct stat	sb;
	short	attr;

	if (stat(path, &sb) == 0) {
#if defined(WIN32)
		if (sb.st_mode & _S_IFDIR) {
			attr = FILEATTR_DIRECTORY;
		}
		else {
			attr = 0;
		}
		if (!(sb.st_mode & S_IWRITE)) {
			attr |= FILEATTR_READONLY;
		}
#else
		if (S_ISDIR(sb.st_mode)) {
			return(FILEATTR_DIRECTORY);
		}
		attr = 0;
		if (!(sb.st_mode & S_IWUSR)) {
			attr |= FILEATTR_READONLY;
		}
#endif
		return(attr);
	}
	return(-1);
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

struct stat sb;
struct tm	*ftime;

	if (fstat(fileno(handle), &sb) == 0) {
		ftime = localtime(&sb.st_mtime);
		if (ftime) {
			if (dosdate) {
				dosdate->year = ftime->tm_year + 1900;
				dosdate->month = ftime->tm_mon + 1;
				dosdate->day = ftime->tm_mday;
			}
			if (dostime) {
				dostime->hour = ftime->tm_hour;
				dostime->minute = ftime->tm_min;
				dostime->second = ftime->tm_sec;
			}
			return(0);
		}
	}
	return(-1);
}

short file_delete(const char *path) {

	return(unlink(path));
}

short file_dircreate(const char *path) {

#if defined(WIN32)
	return((short)mkdir(path));
#else
	return((short)mkdir(path, 0777));
#endif
}


/* �J�����g�t�@�C������ */
void file_setcd(const char *exepath) {

	milstr_ncpy(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(file_create(curpath));
}

short file_delete_c(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(file_delete(curpath));
}

short file_attr_c(const char *path) {

	milstr_ncpy(curfilep, path, sizeof(curpath) - (curfilep - curpath));
	return(file_attr_c(curpath));
}

#if defined(WIN32)
FILEFINDH file_find1st(const char *dir, FILEFINDT *fft) {

	char			path[MAX_PATH];
	HANDLE			hdl;
	WIN32_FIND_DATA	w32fd;

	milsjis_ncpy(path, dir, sizeof(path));
	file_setseparator(path, sizeof(path));
	milsjis_ncat(path, "*.*", sizeof(path));
	hdl = FindFirstFile(path, &w32fd);
	if (hdl == INVALID_HANDLE_VALUE) {
		return(FILEFINDH_INVALID);
	}
	if (fft) {
		milsjis_ncpy(fft->path, w32fd.cFileName, sizeof(fft->path));
		fft->size = w32fd.nFileSizeLow;
		fft->attr = w32fd.dwFileAttributes;
	}
	return((FILEFINDH)hdl);
}

BOOL file_findnext(FILEFINDH hdl, FILEFINDT *fft) {

	WIN32_FIND_DATA	w32fd;

	if (!FindNextFile((HANDLE)hdl, &w32fd)) {
		return(FAILURE);
	}
	if (fft) {
		milsjis_ncpy(fft->path, w32fd.cFileName, sizeof(fft->path));
		fft->size = w32fd.nFileSizeLow;
		fft->attr = w32fd.dwFileAttributes;
	}
	return(SUCCESS);
}

void file_findclose(FILEFINDH hdl) {

	FindClose((HANDLE)hdl);
}
#else
FILEFINDH file_find1st(const char *dir, FILEFINDT *fft) {

	DIR		*ret;

	ret = opendir(dir);
	if (ret == NULL) {
		goto ff1_err;
	}
	if (file_findnext((FILEFINDH)ret, fft) == SUCCESS) {
		return((FILEFINDH)ret);
	}
	closedir(ret);

ff1_err:
	return(FILEFINDH_INVALID);
}

BOOL file_findnext(FILEFINDH hdl, FILEFINDT *fft) {

struct dirent	*de;
struct stat		sb;
	UINT32		attr;
	UINT32		size;

	de = readdir((DIR *)hdl);
	if (de == NULL) {
		return(FAILURE);
	}
	if (fft) {
		mileuc_ncpy(fft->path, de->d_name, sizeof(fft->path));
		size = 0;
		attr = 0;
		if (stat(de->d_name, &sb) == 0) {
			size = sb.st_size;
			if (S_ISDIR(sb.st_mode)) {
				attr = FILEATTR_DIRECTORY;
			}
			else if (!(sb.st_mode & S_IWUSR)) {
				attr = FILEATTR_READONLY;
			}
		}
		fft->size = size;
		fft->attr = attr;
	}
	return(SUCCESS);
}

void file_findclose(FILEFINDH hdl) {

	closedir((DIR *)hdl);
}
#endif

void file_catname(char *path, const char *name, int maxlen) {

	char	c;

	while(maxlen > 0) {
		if (*path == '\0') {
			break;
		}
		path++;
		maxlen--;
	}
	if (maxlen > 0) {
		maxlen--;
		while(maxlen > 0) {
			maxlen--;
			c = *name++;
			if (ISKANJI1ST(c)) {
				if ((maxlen == 0) || (*name == '\0')) {
					break;
				}
				*path++ = c;
				*path++ = *name++;
			}
			else if (c == '\\') {
				*path++ = '/';
			}
			else if (c) {
				*path++ = c;
			}
			else {
				break;
			}
		}
		*path = '\0';
	}
}

char *file_getname(char *path) {

	char	*ret;

	ret = path;
	while(1) {
		if (ISKANJI1ST(*path)) {
			if (*(path+1) == '\0') {
				break;
			}
			path++;
		}
		else if (*path == '/') {
			ret = path + 1;
		}
		else if (*path == '\0') {
			break;
		}
		path++;
	}
	return(ret);
}

void file_cutname(char *path) {

	char	*p;

	p = file_getname(path);
	*p = '\0';
}

char *file_getext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;
	while(*p != '\0') {
		if (*p == '.') {
			q = p + 1;
		}
		p++;
	}
	if (q == NULL) {
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
		if (*p == '.') {
			q = p;
		}
		p++;
	}
	if (q != NULL) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos > 0) && (path[pos] == '/')) {
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = strlen(path);
	if ((pos) && (path[pos-1] != '/') && ((pos + 2) < maxlen)) {
		path[pos++] = '/';
		path[pos] = '\0';
	}
}

