/*	$Id: dosio.c,v 1.11 2004/02/08 07:31:47 monaka Exp $	*/

#include "compiler.h"

#include <sys/stat.h>
#include <time.h>

#include "codecnv.h"
#include "dosio.h"


static char curpath[MAX_PATH];
static char *curfilep = curpath;

#define ISKANJI(c)	((((c) - 0xa1) & 0xff) < 0x5c)


void
dosio_init(void)
{

	/* nothing to do */
}

void
dosio_term(void)
{

	/* nothing to do */
}

/* ファイル操作 */
FILEH
file_open(const char *path)
{
	FILEH fh;

	fh = fopen(path, "rb+");
	if (fh)
		return fh;
	return fopen(path, "rb");
}

FILEH
file_open_rb(const char *path)
{

	return fopen(path, "rb");
}

FILEH
file_create(const char *path)
{

	return fopen(path, "wb+");
}

long
file_seek(FILEH handle, long pointer, int method)
{

	fseek(handle, pointer, method);
	return ftell(handle);
}

UINT
file_read(FILEH handle, void *data, UINT length)
{

	return (UINT)fread(data, 1, length, handle);
}

UINT
file_write(FILEH handle, const void *data, UINT length)
{

	return (UINT)fwrite(data, 1, length, handle);
}

short
file_close(FILEH handle)
{

	fclose(handle);
	return 0;
}

UINT
file_getsize(FILEH handle)
{
	struct stat sb;

	if (fstat(fileno(handle), &sb) == 0)
		return sb.st_size;
	return 0;
}

short
file_attr(const char *path)
{
	struct stat sb;
	short attr;

	if (stat(path, &sb) == 0) {
		if (S_ISDIR(sb.st_mode)) {
			return FILEATTR_DIRECTORY;
		}
		attr = 0;
		if (!(sb.st_mode & S_IWUSR)) {
			attr |= FILEATTR_READONLY;
		}
		return attr;
	}
	return -1;
}

static BOOL
cnvdatetime(struct stat *sb, DOSDATE *dosdate, DOSTIME *dostime)
{
	struct tm *ftime;

	ftime = localtime(&sb->st_mtime);
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
		return SUCCESS;
	}
	return FAILURE;
}

short
file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime)
{
	struct stat sb;

	if ((fstat(fileno(handle), &sb) == 0)
	 && (cnvdatetime(&sb, dosdate, dostime)))
		return 0;
	return -1;
}

short
file_delete(const char *path)
{

	return (short)unlink(path);
}

short
file_dircreate(const char *path)
{

	return (short)mkdir(path, 0777);
}


/* カレントファイル操作 */
void
file_setcd(const char *exepath)
{

	milstr_ncpy(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *
file_getcd(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return curpath;
}

FILEH
file_open_c(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return file_open(curpath);
}

FILEH
file_open_rb_c(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return file_open_rb(curpath);
}

FILEH
file_create_c(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return file_create(curpath);
}

short
file_delete_c(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return file_delete(curpath);
}

short
file_attr_c(const char *sjis)
{

	*curfilep = '\0';
	file_catname(curpath, sjis, sizeof(curpath));
	return file_attr_c(curpath);
}

FLISTH
file_list1st(const char *dir, FLINFO *fli)
{
	FLISTH ret;

	ret = (FLISTH)malloc(sizeof(_FLISTH));
	if (ret == NULL) {
		VERBOSE(("file_list1st: couldn't alloc memory (size = %d)", sizeof(_FLISTH)));
		return FLISTH_INVALID;
	}

	mileuc_ncpy(ret->path, dir, sizeof(ret->path));
	file_setseparator(ret->path, sizeof(ret->path));
	ret->hdl = opendir(ret->path);
	VERBOSE(("file_list1st: opendir(%s)", ret->path));
	if (ret->hdl == NULL) {
		VERBOSE(("file_list1st: opendir failure"));
		free(ret);
		return FLISTH_INVALID;
	}
	if (file_listnext((FLISTH)ret, fli) == SUCCESS) {
		return (FLISTH)ret;
	}
	VERBOSE(("file_list1st: file_listnext failure"));
	closedir(ret->hdl);
	free(ret);
	return FLISTH_INVALID;
}

BOOL
file_listnext(FLISTH hdl, FLINFO *fli)
{
	char buf[MAX_PATH];
	struct dirent *de;
	struct stat sb;

	de = readdir(hdl->hdl);
	if (de == NULL) {
		VERBOSE(("file_listnext: readdir failure"));
		return FAILURE;
	}

	milstr_ncpy(buf, hdl->path, sizeof(buf));
	mileuc_ncat(buf, de->d_name, sizeof(buf));
	if (stat(buf, &sb) != 0) {
		VERBOSE(("file_listnext: stat failure. (path = %s)", buf));
		return FAILURE;
	}

	fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
	fli->size = sb.st_size;
	fli->attr = 0;
	if (S_ISDIR(sb.st_mode)) {
		fli->attr |= FILEATTR_DIRECTORY;
	}
	if (!(sb.st_mode & S_IWUSR)) {
		fli->attr |= FILEATTR_READONLY;
	}
	cnvdatetime(&sb, &fli->date, &fli->time);
	mileuc_ncpy(fli->path, de->d_name, sizeof(fli->path));
	VERBOSE(("file_listnext: success"));
	return SUCCESS;
}

void
file_listclose(FLISTH hdl)
{

	if (hdl) {
		closedir(hdl->hdl);
		free(hdl);
	}
}

static int
euckanji1st(const char *str, int pos)
{
	int ret;

	ret = 0;
	while ((pos >= 0) && (((str[pos--] - 0xa1) & 0xff) < 0x5d)) {
		ret ^= 1;
	}
	return ret;
}

void
file_cpyname(char *dst, const char *src, int maxlen)
{
	int i;

	if (maxlen--) {
		for (i = 0; i < maxlen && src[i] != '\0'; i++) {
			dst[i] = src[i];
		}
		if (i > 0) {
			if (euckanji1st(src, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

void
file_catname(char *path, const char *sjis, int maxlen)
{

	while (maxlen) {
		if (*path == '\0') {
			break;
		}
		path++;
		maxlen--;
	}
	if (maxlen) {
		codecnv_sjis2euc(path, maxlen, sjis, (UINT)-1);
		for (; path[0] != '\0'; path++) {
			if (!ISKANJI(path[0])) {
				if (path[1] == '\0') {
					break;
				}
				path++;
			} else if ((((path[0]) - 0x41) & 0xff) < 26) {
				path[0] |= 0x20;
			} else if (path[0] == '\\') {
				path[0] = '/';
			}
		}
	}
}

BOOL
file_cmpname(const char *path, const char *sjis)
{
	char euc[MAX_PATH];

	codecnv_sjis2euc(euc, sizeof(euc), sjis, (UINT)-1);
	return strcmp(path, euc);
}

char *
file_getname(char *path)
{
	char *ret;

	for (ret = path; path[0] != '\0'; path++) {
		if (ISKANJI(path[0])) {
			if (path[1] == '\0') {
				break;
			}
			path++;
		} else if (path[0] == '/') {
			ret = path + 1;
		}
	}
	return ret;
}

void
file_cutname(char *path)
{
	char *p;

	p = file_getname(path);
	*p = '\0';
}

char *
file_getext(char *path)
{
	char *p;
	char *q;

	p = file_getname(path);
	q = NULL;
	while (*p != '\0') {
		if (*p == '.') {
			q = p + 1;
		}
		p++;
	}
	if (q == NULL) {
		q = p;
	}
	return q;
}

void
file_cutext(char *path)
{
	char *p;
	char *q;

	p = file_getname(path);
	q = NULL;
	while (*p != '\0') {
		if (*p == '.') {
			q = p;
		}
		p++;
	}
	if (q != NULL) {
		*q = '\0';
	}
}

void
file_cutseparator(char *path)
{
	int pos;

	pos = strlen(path) - 1;
	if ((pos > 0) && (path[pos] == '/')) {
		path[pos] = '\0';
	}
}

void
file_setseparator(char *path, int maxlen)
{
	int pos;

	pos = strlen(path);
	if ((pos) && (path[pos-1] != '/') && ((pos + 2) < maxlen)) {
		path[pos++] = '/';
		path[pos] = '\0';
	}
}
