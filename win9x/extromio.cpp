#include	"compiler.h"
#include	"dosio.h"
#include	"extromio.h"


extern	HINSTANCE	hInst;

static const char str_extrom[] = "EXTROM";


EXTROMH extromio_open(const char *filename, UINT type) {

	EXTROMH	ret;
	HRSRC	hrsrc;

	ret = (EXTROMH)_MALLOC(sizeof(_EXTROMH), filename);
	if (ret == NULL) {
		goto erope_err1;
	}
	ret->type = type;
	if (type == EXTROMIO_FILE) {
		ret->fh = (void *)file_open_c(filename);
		if ((FILEH)ret->fh != FILEH_INVALID) {
			return(ret);
		}
	}
	else if (type == EXTROMIO_RES) {
		hrsrc = FindResource(hInst, filename, str_extrom);
		if (hrsrc) {
			ret->fh = (void *)LoadResource(hInst, hrsrc);
			ret->pos = 0;
			ret->size = SizeofResource(hInst, hrsrc);
			return(ret);
		}
	}
	_MFREE(ret);

erope_err1:
	return(NULL);
}

UINT extromio_read(EXTROMH erh, void *buf, UINT size) {

const char	*p;

	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			return(file_read((FILEH)erh->fh, buf, size));
		}
		else if (erh->type == EXTROMIO_RES) {
			size = min(size, (UINT)(erh->size - erh->pos));
			if (size) {
				p = (char *)LockResource((HGLOBAL)erh->fh);
				CopyMemory(buf, p + erh->pos, size);
				erh->pos += size;
				UnlockResource((HGLOBAL)erh->fh);
			}
			return(size);
		}
	}
	return(0);
}

long extromio_seek(EXTROMH erh, long pos, int method) {

	long	ret;

	ret = 0;
	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			ret = file_seek((FILEH)erh->fh, pos, method);
		}
		else if (erh->type == EXTROMIO_RES) {
			if (method == ERSEEK_CUR) {
				ret = erh->pos;
			}
			else if (method == ERSEEK_END) {
				ret = erh->size;
			}
			ret += pos;
			if (ret < 0) {
				ret = 0;
			}
			else if ((UINT)ret > erh->size) {
				ret = erh->size;
			}
			erh->pos = ret;
		}
	}
	return(ret);
}

void extromio_close(EXTROMH erh) {

	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			file_close((FILEH)erh->fh);
		}
		else if (erh->type == EXTROMIO_RES) {
			FreeResource((HGLOBAL)erh->fh);
		}
		_MFREE(erh);
	}
}

