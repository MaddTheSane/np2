#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"textfile.h"


enum {
	TFMODE_READ		= 0x01,
	TFMODE_WRITE	= 0x02
};

typedef struct {
	UINT8	mode;
	UINT8	access;
	UINT8	srcwidth;
	UINT8	xendian;
	FILEH	fh;
	long	fpos;
	void	*buf;
	UINT	bufsize;
	UINT	bufpos;
	UINT	bufrem;
} _TEXTFILE, *TEXTFILE;


static TEXTFILEH registfile(FILEH fh, UINT buffersize,
											const UINT8 *hdr, UINT hdrsize) {

	long		fpos;
	UINT8		srcwidth;
	UINT8		xendian;
	TEXTFILE	ret;

	buffersize = buffersize & (~3);
	if (buffersize < 256) {
		buffersize = 256;
	}
	fpos = 0;
	srcwidth = 1;
	xendian = FALSE;
	if ((hdrsize >= 3) &&
		(hdr[0] == 0xef) && (hdr[1] == 0xbb) && (hdr[2] == 0xbf)) {
		// UTF-8
		fpos = 3;
	}
	else if ((hdrsize >= 2) && (hdr[0] == 0xff) && (hdr[1] == 0xfe)) {
		// UCSLE
		fpos = 2;
		srcwidth = 2;
#if defined(BYTESEX_BIG)
		xendian = TRUE;
#endif
	}
	else if ((hdrsize >= 2) && (hdr[0] == 0xfe) && (hdr[1] == 0xff)) {
		// UCS2BE
		fpos = 2;
		srcwidth = 2;
#if defined(BYTESEX_LITTLE)
		xendian = TRUE;
#endif
	}

	if (srcwidth != sizeof(OEMCHAR)) {
		return(NULL);
	}
	buffersize = buffersize * sizeof(OEMCHAR);

	ret = (TEXTFILE)_MALLOC(sizeof(_TEXTFILE) + buffersize, "TEXTFILE");
	if (ret == NULL) {
		return(NULL);
	}
	ZeroMemory(ret, sizeof(_TEXTFILE));
//	ret->mode = 0;
	ret->srcwidth = srcwidth;
	ret->xendian = xendian;
	ret->fh = fh;
	ret->fpos = fpos;
	ret->buf = (UINT8 *)(ret + 1);
	ret->bufsize = buffersize;
	return((TEXTFILEH)ret);
}

static BRESULT flushfile(TEXTFILE tf) {

	if (tf->mode & TFMODE_READ) {
		tf->fpos -= tf->bufrem * tf->srcwidth;
	}
	tf->mode = 0;
	tf->bufpos = 0;
	tf->bufrem = 0;
	if (file_seek(tf->fh, tf->fpos, FSEEK_SET) == tf->fpos) {
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}


// ---- A

static UINT fillbufferA(TEXTFILE tf) {

	UINT	rsize;

	if (tf->bufrem == 0) {
		rsize = file_read(tf->fh, tf->buf, tf->bufsize);
		rsize = rsize / sizeof(char);
		tf->fpos += rsize * sizeof(char);
		tf->bufpos = 0;
		tf->bufrem = rsize;
	}
	return(tf->bufrem);
}

static BRESULT readlineA(TEXTFILE tf, void *buffer, UINT size) {

	char	*dst;
	BOOL	crlf;
	BRESULT	ret;
	char	c;
const char	*src;
	UINT	pos;

	if (size == 0) {
		dst = NULL;
		size = 0;
	}
	else {
		dst = (char *)buffer;
		size--;
	}

	crlf = FALSE;
	ret = FAILURE;
	c = 0;
	do {
		if (fillbufferA(tf) == 0) {
			break;
		}
		ret = SUCCESS;
		src = (char *)tf->buf;
		src += tf->bufpos;
		pos = 0;
		while(pos<tf->bufrem) {
			c = src[pos];
			pos++;
			if ((c == 0x0d) || (c == 0x0a)) {
				crlf = TRUE;
				break;
			}
			if (size) {
				size--;
				*dst++ = c;
			}
		}
		tf->bufpos += pos;
		tf->bufrem -= pos;
	} while(!crlf);
	if ((crlf) && (c == 0x0d)) {
		if (fillbufferA(tf) != 0) {
			src = (char *)tf->buf;
			src += tf->bufpos;
			if (*src == 0x0a) {
				tf->bufpos++;
				tf->bufrem--;
			}
		}
	}
	if (dst) {
		*dst = '\0';
	}
	return(ret);
}


// ---- W

static UINT fillbufferW(TEXTFILE tf) {

	UINT	rsize;
	UINT8	*buf;
	UINT8	tmp;

	if (tf->bufrem == 0) {
		buf = tf->buf;
		rsize = file_read(tf->fh, buf, tf->bufsize);
		rsize = rsize / sizeof(UINT16);
		tf->fpos += rsize * sizeof(UINT16);
		tf->bufpos = 0;
		tf->bufrem = rsize;
		if (tf->xendian) {
			while(rsize) {
				tmp = buf[0];
				buf[0] = buf[1];
				buf[1] = tmp;
				buf += 2;
				rsize--;
			}
		}
	}
	return(tf->bufrem);
}

static BRESULT readlineW(TEXTFILE tf, void *buffer, UINT size) {

	UINT16		*dst;
	BOOL		crlf;
	BRESULT		ret;
	UINT16		c;
const UINT16	*src;
	UINT		pos;

	if (size == 0) {
		dst = NULL;
		size = 0;
	}
	else {
		dst = (UINT16 *)buffer;
		size--;
	}
	crlf = FALSE;
	ret = FAILURE;
	c = 0;
	do {
		if (fillbufferW(tf) == 0) {
			break;
		}
		ret = SUCCESS;
		src = (UINT16 *)tf->buf;
		src += tf->bufpos;
		pos = 0;
		while(pos<tf->bufrem) {
			c = src[pos];
			pos++;
			if ((c == 0x0d) || (c == 0x0a)) {
				crlf = TRUE;
				break;
			}
			if (size) {
				size--;
				*dst++ = c;
			}
		}
		tf->bufpos += pos;
		tf->bufrem -= pos;
	} while(!crlf);
	if ((crlf) && (c == 0x0d)) {
		if (fillbufferW(tf) != 0) {
			src = (UINT16 *)tf->buf;
			src += tf->bufpos;
			if (*src == 0x0a) {
				tf->bufpos++;
				tf->bufrem--;
			}
		}
	}
	if (dst) {
		*dst = '\0';
	}
	return(ret);
}


// ----

TEXTFILEH textfile_open(const OEMCHAR *filename, UINT buffersize) {

	FILEH		fh;
	UINT8		hdr[4];
	UINT		hdrsize;
	TEXTFILEH	ret;

	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto tfo_err;
	}
	hdrsize = file_read(fh, hdr, sizeof(hdr));
	ret = registfile(fh, buffersize, hdr, hdrsize);
	if (ret) {
		return(ret);
	}
	file_close(fh);

tfo_err:
	return(NULL);
}

TEXTFILEH textfile_create(const OEMCHAR *filename, UINT buffersize) {

	FILEH		fh;
const UINT8		*hdr;
	UINT		hdrsize;
	TEXTFILEH	ret;

	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto tfc_err1;
	}
#if defined(OSLANG_UTF8)
	hdr = str_utf8;
	hdrsize = sizeof(str_utf8);
#elif defined(OSLANG_UCS2) 
	hdr = (UINT8 *)str_ucs2;
	hdrsize = sizeof(str_ucs2);
#else
	hdr = NULL;
	hdrsize = 0;
#endif
	if ((hdrsize) && (file_write(fh, hdr, hdrsize) != hdrsize)) {
		goto tfc_err2;
	}
	ret = registfile(fh, buffersize, hdr, hdrsize);
	if (ret) {
		return(ret);
	}

tfc_err2:
	file_close(fh);

tfc_err1:
	return(NULL);
}

BRESULT textfile_read(TEXTFILEH tfh, OEMCHAR *buffer, UINT size) {

	TEXTFILE	tf;

	tf = (TEXTFILE)tfh;
	if (tf) {
		if (!(tf->mode & TFMODE_READ)) {
			flushfile(tf);
			tf->mode = TFMODE_READ;
		}
		if (sizeof(OEMCHAR) == 1) {
			return(readlineA(tf, buffer, size));
		}
		else if (sizeof(OEMCHAR) == 2) {
			return(readlineW(tf, buffer, size));
		}
	}
	return(FAILURE);
}

BRESULT textfile_write(TEXTFILEH tfh, const OEMCHAR *buffer) {

	TEXTFILE	tf;
	UINT		leng;
	UINT		wsize;

	tf = (TEXTFILE)tfh;
	if (tf) {
		if (!(tf->mode & TFMODE_WRITE)) {
			flushfile(tf);
			tf->mode = TFMODE_WRITE;
		}
		leng = OEMSTRLEN(buffer);
		leng = leng * sizeof(OEMCHAR);
		wsize = file_write(tf->fh, buffer, leng);
		tf->fpos += wsize;
		if (wsize == leng) {
			return(SUCCESS);
		}
	}
	return(FAILURE);
}

void textfile_close(TEXTFILEH tfh) {

	if (tfh) {
		file_close(((TEXTFILE)tfh)->fh);
		_MFREE(tfh);
	}
}

