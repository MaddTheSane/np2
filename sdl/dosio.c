#include	"compiler.h"
#include	<sys/stat.h>
#include	<time.h>
#include	"dosio.h"
#if defined(WIN32)
#include	<direct.h>
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

/* ファイル操作 */
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


/* カレントファイル操作 */
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
			maxlen = 0;
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

