#include	"compiler.h"
#include	"dosio.h"
#include	"textfile.h"


static BOOL getnextstrings(TEXTFILEH fh) {

	UINT	rsize;

	if (!fh) {
		return(FAILURE);
	}
	if (file_seek((FILEH)fh->fh, fh->fhpos, 0) != fh->fhpos) {
		return(FAILURE);
	}
	rsize = file_read((FILEH)fh->fh, fh + 1, fh->buffersize);
	if (rsize == (UINT)-1) {
		return(FAILURE);
	}
	fh->fhpos += rsize;
	fh->pos = 0;
	fh->remain = rsize;
	return(SUCCESS);
}

TEXTFILEH textfile_open(const char *filename, UINT buffersize) {

	FILEH		fh;
	TEXTFILEH	ret;

	if (buffersize < 256) {
		buffersize = 256;
	}
	fh = file_open(filename);
	if (fh == FILEH_INVALID) {
		goto tfo_err1;
	}
	ret = (TEXTFILEH)_MALLOC(sizeof(_TEXTFILE) + buffersize, filename);
	if (ret == NULL) {
		goto tfo_err2;
	}
	ZeroMemory(ret, sizeof(_TEXTFILE) + buffersize);
	ret->fh = (long)fh;
	ret->buffersize = buffersize;
#if defined(OSLANG_UTF8)
	getnextstrings(ret);
	if (ret->remain >= 3) {
		char *ptr;
		ptr = ((char *)(ret + 1)) + ret->pos;
		if ((ptr[0] == (char)0xef) &&
			(ptr[1] == (char)0xbb) &&
			(ptr[2] == (char)0xbf)) {
			ret->pos += 3;
			ret->remain -= 3;
		}
	}
#endif
	return(ret);

tfo_err2:
	file_close(fh);

tfo_err1:
	return(NULL);
}

void textfile_close(TEXTFILEH fh) {

	if (fh) {
		file_close((FILEH)fh->fh);
		_MFREE(fh);
	}
}

BOOL textfile_read(TEXTFILEH fh, char *buffer, UINT size) {

	char	c = '\0';
	char	*p;
	BOOL	crlf;
	BOOL	ret = FAILURE;

	if ((fh) && (size > 0)) {
		size--;
		crlf = FALSE;
		do {
			if ((!fh->remain) && (getnextstrings(fh))) {
				return(FAILURE);
			}
			if (!fh->remain) {
				break;
			}
			ret = SUCCESS;
			p = (char *)(fh + 1);
			p += fh->pos;
			while((fh->remain) && (size)) {
				c = *p++;
				fh->pos++;
				fh->remain--;
				if ((c == 0x0d) || (c == 0x0a)) {
					crlf = TRUE;
					break;
				}
				*buffer++ = c;
				size--;
			}
			if (!crlf) {
				while((fh->remain) && (size)) {
					c = *p++;
					fh->pos++;
					fh->remain--;
					if ((c == 0x0d) || (c == 0x0a)) {
						crlf = TRUE;
						break;
					}
				}
			}
		} while(!crlf);
		if ((crlf) && (c == 0x0d)) {
			if ((!fh->remain) && (getnextstrings(fh))) {
				return(FAILURE);
			}
			if (fh->remain) {
				p = (char *)(fh + 1);
				p += fh->pos;
				if (*p == 0x0a) {
					fh->pos++;
					fh->remain--;
				}
			}
		}
		*buffer = '\0';
	}
	return(ret);
}

