#include	"compiler.h"
#include	<time.h>
#ifndef NP2GCC
#ifdef TARGET_API_MAC_CARBON
#include	<CFString.h>
#endif
#endif


void macossub_init(void) {
}


void macossub_term(void) {
}


UINT32 macos_gettick(void) {

	UnsignedWide current;

	Microseconds(&current);
	return((UINT32)(current.lo / 1000));
}


// ---- code

void mkstr255(Str255 dst, const char *src) {

	int		len;

	len = strlen(src);
	if (len >= 255) {
		len = 255;
	}
	dst[0] = (BYTE)len;
	if (len) {
		CopyMemory((char *)dst+1, src, len);
	}
}

void mkcstr(char *dst, int size, const Str255 src) {

	if ((src != NULL) && (dst != NULL) && (size > 0)) {
		size--;
		size = min(size, src[0]);
		if (size) {
			CopyMemory(dst, src + 1, size);
		}
		dst[size] = '\0';
	}
}


// ---- file list

#ifdef TARGET_API_MAC_CARBON

typedef struct {
	BOOL			eoff;
	FSIterator		fsi;
	FSCatalogInfo	fsci;
	HFSUniStr255	name;
} _FLHDL, *FLHDL;

static void char2str(char *dst, int size, const UniChar *uni, int unicnt) {

	CFStringRef	cfsr;

	cfsr = CFStringCreateWithCharacters(NULL, uni, unicnt);
	CFStringGetCString(cfsr, dst, size, CFStringGetSystemEncoding());
	CFRelease(cfsr);
}

void *file_list1st(const char *dir, FLDATA *fl) {

	void		*ret;
	Str255		fname;
	FSSpec		fss;
	FSRef		fsr;
	FSIterator	fsi;

	mkstr255(fname, dir);
	if ((FSMakeFSSpec(0, 0, fname, &fss) != noErr) ||
		(FSpMakeFSRef(&fss, &fsr) != noErr) ||
		(FSOpenIterator(&fsr, kFSIterateFlat, &fsi) != noErr)) {
		goto ff1_err1;
	}
	ret = _MALLOC(sizeof(_FLHDL), dir);
	if (ret == NULL) {
		goto ff1_err2;
	}
	((FLHDL)ret)->eoff = FALSE;
	((FLHDL)ret)->fsi = fsi;
	if (file_listnext(ret, fl) == SUCCESS) {
		return(ret);
	}

ff1_err2:
	FSCloseIterator(fsi);

ff1_err1:
	return(NULL);
}

BOOL file_listnext(void *hdl, FLDATA *fl) {

	FLHDL		flhdl;
	ItemCount	count;
	OSStatus	r;

	flhdl = (FLHDL)hdl;
	if ((flhdl == NULL) || (flhdl->eoff)) {
		goto ffn_err;
	}
	r = FSGetCatalogInfoBulk(flhdl->fsi, 1, &count, NULL,
						kFSCatInfoNodeFlags | kFSCatInfoDataSizes,
						&flhdl->fsci, NULL, NULL, &flhdl->name);
	if (r != noErr) {
		flhdl->eoff = TRUE;
		if (r != errFSNoMoreItems) {
			goto ffn_err;
		}
	}
	if (count != 1) {
		flhdl->eoff = TRUE;
		goto ffn_err;
	}
	if (fl) {
		char2str(fl->path, sizeof(fl->path),
								flhdl->name.unicode, flhdl->name.length);
		fl->size = (UINT32)flhdl->fsci.dataLogicalSize;
		if (flhdl->fsci.nodeFlags & kFSNodeIsDirectoryMask) {
			fl->attr = FILEATTR_DIRECTORY;
		}
		else {
			fl->attr = FILEATTR_ARCHIVE;
		}
	}
	return(SUCCESS);

ffn_err:
	return(FAILURE);
}

void file_listclose(void *hdl) {

	if (hdl) {
		FSCloseIterator(((FLHDL)hdl)->fsi);
		_MFREE(hdl);
	}
}

#else

typedef struct {
	BOOL		eoff;
	short		index;
	CInfoPBRec	pb;
	long		tagid;
} _FLHDL, *FLHDL;

void *file_list1st(const char *dir, FLDATA *fl) {

	Str255	fname;
	FSSpec	fss;
	FLHDL	ret;

	mkstr255(fname, dir);
	FSMakeFSSpec(0, 0, fname, &fss);
	ret = (FLHDL)_MALLOC(sizeof(_FLHDL), dir);
	if (ret == NULL) {
		goto ff1_err1;
	}
	ret->pb.dirInfo.ioNamePtr = fss.name;
	ret->pb.dirInfo.ioVRefNum = fss.vRefNum;
	ret->pb.dirInfo.ioDrDirID = fss.parID;
	if (fss.name[0] == 0) {
		ret->pb.dirInfo.ioFDirIndex = -1;
	}
	else {
		ret->pb.dirInfo.ioFDirIndex = 0;
	}
	if (PBGetCatInfo(&ret->pb, false) != noErr) {
		goto ff1_err2;
	}
	if (ret->pb.hFileInfo.ioFlAttrib & 0x10) {
		ret->tagid = ret->pb.dirInfo.ioDrDirID;
	}
	else {
		ret->tagid = ret->pb.hFileInfo.ioFlParID;
	}
	ret->eoff = FALSE;
	ret->index = 1;
	if (file_listnext((void *)ret, fl) == SUCCESS) {
		return((void *)ret);
	}

ff1_err2:
	_MFREE(ret);

ff1_err1:
	return(NULL);
}

BOOL file_listnext(void *hdl, FLDATA *fl) {

	FLHDL	flhdl;
	Str255	fname;

	flhdl = (FLHDL)hdl;
	if ((flhdl == NULL) || (flhdl->eoff)) {
		goto ffn_err;
	}
	fname[0] = 0;
	flhdl->pb.dirInfo.ioNamePtr = fname;
	flhdl->pb.dirInfo.ioDrDirID = flhdl->tagid;
	flhdl->pb.dirInfo.ioFDirIndex = flhdl->index;
	flhdl->pb.dirInfo.ioACUser = 0;
	if (PBGetCatInfo(&flhdl->pb, false) != noErr) {
		flhdl->eoff = TRUE;
		goto ffn_err;
	}
	flhdl->index++;
	if (fl) {
		mkcstr(fl->path, sizeof(fl->path), fname);
		fl->size = 0;
		if (flhdl->pb.hFileInfo.ioFlAttrib & 0x10) {
			fl->attr = FILEATTR_DIRECTORY;
		}
		else {
			fl->attr = FILEATTR_ARCHIVE;
		}
	}
	return(SUCCESS);

ffn_err:
	return(FAILURE);
}

void file_listclose(void *hdl) {

	if (hdl) {
		_MFREE(hdl);
	}
}

#endif

