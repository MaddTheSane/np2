#include	"compiler.h"
#include	"dosio.h"


static	char	curpath[MAX_PATH] = ":";
static	char	*curfilep = curpath + 1;

#define ISKANJI(c)	(((((c) ^ 0x20) - 0xa1) & 0xff) < 0x3c)

void dosio_init(void) { }
void dosio_term(void) { }

											// ファイル操作
FILEH file_open(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if ((FSpOpenDF(&fss, fsRdWrPerm, &ret) == noErr) ||
		(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
		SetFPos(ret, fsFromStart, 0);
		return(ret);
	}
	return(-1);
}

FILEH file_open_rb(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if ((FSpOpenDF(&fss, fsRdWrShPerm, &ret) == noErr) ||
		(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
		SetFPos(ret, fsFromStart, 0);
		return(ret);
	}
	return(-1);
}

FILEH file_create(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;
	OSType	creator = '????';
	OSType	fileType = '????';

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpDelete(&fss);

	if (FSpCreate(&fss, creator, fileType, smSystemScript) == noErr) {
		if ((FSpOpenDF(&fss, fsRdPerm | fsWrPerm, &ret) == noErr) ||
			(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
			SetFPos(ret, fsFromStart, 0);
			return(ret);
		}
	}
	return(-1);
}

long file_seek(FILEH handle, long pointer, int method) {

	SInt32	pos;
	SInt32	setp;

	setp = pointer;
	switch(method) {
		case FSEEK_SET:
			break;
		case FSEEK_CUR:
			if (GetFPos(handle, &pos) != noErr) {
				return(-1);
			}
			setp += pos;
			break;
		case FSEEK_END:
			if (GetEOF(handle, &pos) != noErr) {
				return(-1);
			}
			setp += pos;
			break;
		default:
			return(-1);
	}
	SetFPos(handle, fsFromStart, setp);
	if (GetFPos(handle, &pos) != noErr) {
		return(-1);
	}
	return((long)pos);
}

UINT file_read(FILEH handle, void *data, UINT length) {

	long	size;
	OSErr	err;

	size = length;
	err = FSRead(handle, &size, (char *)data);
	if ((err == noErr) || (err == eofErr)) {
		return(size);
	}
	return(0);
}

UINT file_write(FILEH handle, const void *data, UINT length) {

	if (length) {
		long size = length;
		if (FSWrite(handle, &size, (char *)data) == noErr) {
			return(size);
		}
	}
	else {
		SInt32 pos;
		if (GetFPos(handle, &pos) == noErr) {
			SetEOF(handle, pos);
		}
	}
	return(0);
}

short file_close(FILEH handle) {

	FSClose(handle);
	return(0);
}

UINT file_getsize(FILEH handle) {

	SInt32 pos;

	if (GetEOF(handle, &pos) == noErr) {
		return((UINT)pos);
	}
	else {
		return(0);
	}
}

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

#ifdef TARGET_API_MAC_CARBON

	FSRef			ref;
	FSCatalogInfo	fsci;
	LocalDateTime	ldt;
	DateTimeRec		dtr;

	ZeroMemory(&dtr, sizeof(dtr));
	if ((FSGetForkCBInfo(handle, 0, NULL, NULL, NULL, &ref, NULL) != noErr) ||
		(FSGetCatalogInfo(&ref, kFSCatInfoContentMod, &fsci, NULL, NULL, NULL)
																!= noErr)) {
		return(-1);
	}
	ConvertUTCToLocalDateTime(&fsci.contentModDate, &ldt);
	SecondsToDate(ldt.lowSeconds, &dtr);
	if (dosdate) {
		dosdate->year = dtr.year;
		dosdate->month = (BYTE)dtr.month;
		dosdate->day = (BYTE)dtr.day;
	}
	if (dostime) {
		dostime->hour = (BYTE)dtr.hour;
		dostime->minute = (BYTE)dtr.minute;
		dostime->second = (BYTE)dtr.second;
	}
	return(0);
#else
	(void)handle;
	(void)dosdate;
	(void)dostime;
	return(-1);
#endif
}

short file_delete(const char *path) {

	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpDelete(&fss);
	return(0);
}

short file_attr(const char *path) {

#ifdef TARGET_API_MAC_CARBON
	Str255			fname;
	FSSpec			fss;
	FSRef			fsr;
	FSCatalogInfo	fsci;

	mkstr255(fname, path);
	if ((FSMakeFSSpec(0, 0, fname, &fss) != noErr) ||
		(FSpMakeFSRef(&fss, &fsr) != noErr) ||
		(FSGetCatalogInfo(&fsr, kFSCatInfoNodeFlags, &fsci,
										NULL, NULL, NULL) != noErr)) {
		return(-1);
	}
	else if (fsci.nodeFlags & kFSNodeIsDirectoryMask) {
		return(FILEATTR_DIRECTORY);
	}
	else {
		return(FILEATTR_ARCHIVE);
	}
#else
	Str255		fname;
	FSSpec		fss;
	CInfoPBRec	pb;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	pb.dirInfo.ioNamePtr = fss.name;
	pb.dirInfo.ioVRefNum = fss.vRefNum;
	pb.dirInfo.ioDrDirID = fss.parID;
	if (fss.name[0] == 0) {
		pb.dirInfo.ioFDirIndex = -1;
	}
	else {
		pb.dirInfo.ioFDirIndex = 0;
	}
	if (PBGetCatInfo(&pb, false) != noErr) {
		return(-1);
	}
	if (pb.hFileInfo.ioFlAttrib & 0x10) {
		return(FILEATTR_DIRECTORY);
	}
	else {
		return(FILEATTR_ARCHIVE);
	}
#endif
}

short file_dircreate(const char *path) {

	FSSpec	fss;
	Str255	fname;
	long	ret;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if (FSpDirCreate(&fss, smSystemScript, &ret) == noErr) {
		return(0);
	}
	return(-1);
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

void file_catname(char *path, const char *sjis, int maxlen) {

	char	*p;

	p = path + strlen(path);
	milstr_ncat(path, sjis, maxlen);
	while(1) {
		if (ISKANJI(*p)) {
			if (*(p+1) == '\0') {
				break;
			}
			p++;
		}
		else if ((*p == '/') || (*p == '\\')) {
			*p = ':';
		}
		else if (*p == '\0') {
			break;
		}
		p++;
	}
}

char *file_getname(char *path) {

	char 	*ret;

	ret = path;
	while(*path != '\0') {
		if (*path++ == ':') {
			ret = path;
		}
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
		if (*p++ == '.') {
			q = p;
		}
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
		if (*p == '.') {
			q = p;
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
	if ((pos > 0) && (path[pos] == ':')) {
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos < 0) || (path[pos] == ':') || ((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = ':';
	path[++pos] = '\0';
}

