#include "compiler.h"
#include <sys/stat.h>
#include <time.h>
#if defined(WIN32) && defined(OSLANG_UTF8)
#include "codecnv/codecnv.h"
#endif
#include "dosio.h"
#if defined(WIN32)
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

static	char	curpath[MAX_PATH] = "./";
static	char	*curfilep = curpath + 2;

/* ファイル操作 */
FILEH file_open(const char *path) {

	return(fopen(path, "rb+"));
}

FILEH file_open_rb(const char *path) {

	return(fopen(path, "rb"));
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

	if (fstat(fileno(handle), &sb) == 0)
	{
		return (UINT)sb.st_size;
	}
	return(0);
}

short file_attr(const char *path) {

struct stat	sb;
	short	attr;

	if (stat(path, &sb) == 0)
	{
		if (S_ISDIR(sb.st_mode)) {
			return(FILEATTR_DIRECTORY);
		}
		attr = 0;
		if (!(sb.st_mode & S_IWUSR)) {
			attr |= FILEATTR_READONLY;
		}
		return(attr);
	}
	return(-1);
}

static BRESULT cnv_sttime(time_t *t, DOSDATE *dosdate, DOSTIME *dostime) {

struct tm	*ftime;

	ftime = localtime(t);
	if (ftime == NULL) {
		return(FAILURE);
	}
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
	return(SUCCESS);
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

struct stat sb;

	if (fstat(fileno(handle), &sb) == 0) {
		if (cnv_sttime(&sb.st_mtime, dosdate, dostime) == SUCCESS) {
			return(0);
		}
	}
	return(-1);
}

short file_delete(const char *path) {

	return(remove(path));
}

short file_rename(const char *existpath, const char *newpath) {

	return((short)rename(existpath, newpath));
}

short file_dircreate(const char *path) {

	return((short)mkdir(path, 0777));
}

short file_dirdelete(const char *path) {

	return((short)rmdir(path));
}


/* カレントファイル操作 */
void file_setcd(const char *exepath) {

	file_cpyname(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(file_create(curpath));
}

short file_delete_c(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(file_delete(curpath));
}

short file_attr_c(const char *path) {

	file_cpyname(curfilep, path, NELEMENTS(curpath) - (UINT)(curfilep - curpath));
	return(file_attr(curpath));
}

FLISTH file_list1st(const char *dir, FLINFO *fli) {

	DIR		*ret;

	ret = opendir(dir);
	if (ret == NULL) {
		goto ff1_err;
	}
	if (file_listnext((FLISTH)ret, fli) == SUCCESS) {
		return((FLISTH)ret);
	}
	closedir(ret);

ff1_err:
	return(FLISTH_INVALID);
}

BRESULT file_listnext(FLISTH hdl, FLINFO *fli) {

struct dirent	*de;
struct stat		sb;

	de = readdir((DIR *)hdl);
	if (de == NULL) {
		return(FAILURE);
	}
	if (fli) {
		memset(fli, 0, sizeof(*fli));
		fli->caps = FLICAPS_ATTR;
		fli->attr = (de->d_type & DT_DIR) ? FILEATTR_DIRECTORY : 0;

		if (stat(de->d_name, &sb) == 0) {
			fli->caps |= FLICAPS_SIZE;
			fli->size = (UINT)sb.st_size;
			if (!(sb.st_mode & S_IWUSR)) {
				fli->attr |= FILEATTR_READONLY;
			}
			if (cnv_sttime(&sb.st_mtime, &fli->date, &fli->time) == SUCCESS) {
				fli->caps |= FLICAPS_DATE | FLICAPS_TIME;
			}
		}
		milstr_ncpy(fli->path, de->d_name, sizeof(fli->path));
	}
	return(SUCCESS);
}

void file_listclose(FLISTH hdl) {

	closedir((DIR *)hdl);
}

void file_catname(char *path, const char *name, int maxlen) {

	int		csize;

	while(maxlen > 0) {
		if (*path == '\0') {
			break;
		}
		path++;
		maxlen--;
	}
	file_cpyname(path, name, maxlen);
	while((csize = milstr_charsize(path)) != 0) {
		if ((csize == 1) && (*path == '\\')) {
			*path = '/';
		}
		path += csize;
	}
}

char *file_getname(const char *path) {

const char	*ret;
	int		csize;

	ret = path;
	while((csize = milstr_charsize(path)) != 0) {
		if ((csize == 1) && (*path == '/')) {
			ret = path + 1;
		}
		path += csize;
	}
	return((char *)ret);
}

void file_cutname(char *path) {

	char	*p;

	p = file_getname(path);
	*p = '\0';
}

char *file_getext(const char *path) {

const char	*p;
const char	*q;

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
	return((char *)q);
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

	pos = (int)strlen(path) - 1;
	if ((pos > 0) &&							// 2文字以上でー
		(path[pos] == '/') &&					// ケツが \ でー
		((pos != 1) || (path[0] != '.'))) {		// './' ではなかったら
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = (int)strlen(path);
	if ((pos) && (path[pos-1] != '/') && ((pos + 2) < maxlen)) {
		path[pos++] = '/';
		path[pos] = '\0';
	}
}

static void char2str(char *dst, int size, const UniChar *uni, int unicnt) {
	
	CFStringRef	cfsr;
	
	cfsr = CFStringCreateWithCharacters(NULL, uni, unicnt);
	if (!CFStringGetFileSystemRepresentation(cfsr, dst, size)) {
		CFStringGetCString(cfsr, dst, size, CFStringGetSystemEncoding());
	}
	CFRelease(cfsr);
}

BOOL getLongFileName(char *dst, const char *path) {
	
	FSSpec			fss;
	Str255			fname;
	FSRef			fref;
	HFSUniStr255	name;
	
	if (*path == '\0') {
		return(false);
	}
	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpMakeFSRef(&fss, &fref);
	if (FSGetCatalogInfo(&fref, kFSCatInfoNone, NULL, &name, NULL, NULL)
		!= noErr) {
		return(false);
	}
	char2str(dst, 512, name.unicode, name.length);
	if (!dst) {
		return(false);
	}
	return(true);
}

BOOL getLongFileNameCFStr(CFStringRef *dst, const char *path) {
	
	FSSpec			fss;
	Str255			fname;
	FSRef			fref;
	HFSUniStr255	name;
	
	if (*path == '\0' || dst == NULL) {
		return(false);
	}
	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpMakeFSRef(&fss, &fref);
	if (FSGetCatalogInfo(&fref, kFSCatInfoNone, NULL, &name, NULL, NULL)
		!= noErr) {
		return(false);
	}
	*dst = CFStringCreateWithCharacters(NULL, name.unicode, name.length);
	if (!(*dst)) {
		return(false);
	}
	return(true);
}

