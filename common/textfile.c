#include	"compiler.h"
#include	"dosio.h"
#include	"textfile.h"


typedef struct {
	UINT	mode;
	FILEH	fh;
	BOOL	xendian;
} _TEXTFH, *TEXTFH;

typedef struct {
	_TEXTFH	tf;
	UINT8	*buf;
	UINT	bufsize;
	UINT	bufpos;
	UINT	bufrem;
} _TEXTREAD, *TEXTREAD;


// ---- A

static UINT fillbufferA(TEXTREAD tr) {

	UINT	rsize;

	if (tr->bufrem == 0) {
		rsize = file_read(tr->tf.fh, tr->buf, tr->bufsize);
		tr->bufpos = 0;
		tr->bufrem = rsize;
	}
	return(tr->bufrem);
}

static BRESULT readlineA(TEXTREAD tr, void *buffer, UINT size) {

	UINT8	*dst;
	BOOL	crlf;
	BRESULT	ret;
	UINT8	c;
const UINT8	*src;
	UINT	pos;

	if (size == 0) {
		dst = NULL;
		size = 0;
	}
	else {
		dst = (UINT8 *)buffer;
		size--;
	}

	crlf = FALSE;
	ret = FAILURE;
	c = 0;
	do {
		if (fillbufferA(tr) == 0) {
			break;
		}
		ret = SUCCESS;
		src = tr->buf;
		src += tr->bufpos;
		pos = 0;
		while(pos<tr->bufrem) {
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
		tr->bufpos += pos;
		tr->bufrem -= pos;
	} while(!crlf);
	if ((crlf) && (c == 0x0d)) {
		if (fillbufferA(tr) != 0) {
			src = tr->buf;
			src += tr->bufpos;
			if (*src == 0x0a) {
				tr->bufpos++;
				tr->bufrem--;
			}
		}
	}
	if (dst) {
		*dst = '\0';
	}
	return(ret);
}


// ---- W

static UINT fillbufferW(TEXTREAD tr) {

	UINT	rsize;
	UINT8	*buf;
	UINT8	tmp;

	if (tr->bufrem == 0) {
		buf = tr->buf;
		rsize = file_read(tr->tf.fh, buf, tr->bufsize) / 2;
		tr->bufpos = 0;
		tr->bufrem = rsize / 2;
		if (tr->tf.xendian) {
			while(rsize) {
				tmp = buf[0];
				buf[0] = buf[1];
				buf[1] = tmp;
				buf += 2;
				rsize--;
			}
		}
	}
	return(tr->bufrem);
}

static BRESULT readlineW(TEXTREAD tr, void *buffer, UINT size) {

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
		if (fillbufferW(tr) == 0) {
			break;
		}
		ret = SUCCESS;
		src = (UINT16 *)tr->buf;
		src += tr->bufpos;
		pos = 0;
		while(pos<tr->bufrem) {
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
		tr->bufpos += pos;
		tr->bufrem -= pos;
	} while(!crlf);
	if ((crlf) && (c == 0x0d)) {
		if (fillbufferW(tr) != 0) {
			src = (UINT16 *)tr->buf;
			src += tr->bufpos;
			if (*src == 0x0a) {
				tr->bufpos++;
				tr->bufrem--;
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
	long		fpos;
	UINT		srcwidth;
	BOOL		xendian;
	TEXTREAD	ret;

	buffersize = buffersize & (~3);
	if (buffersize < 256) {
		buffersize = 256;
	}
	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto tfo_err1;
	}
	hdrsize = file_read(fh, hdr, 4);
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
		goto tfo_err2;
	}
	buffersize = buffersize * sizeof(OEMCHAR);

	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto tfo_err2;
	}

	ret = (TEXTREAD)_MALLOC(sizeof(_TEXTREAD) + buffersize, filename);
	if (ret == NULL) {
		goto tfo_err2;
	}
	ZeroMemory(ret, sizeof(_TEXTREAD));
	ret->tf.mode = 0;
	ret->tf.fh = fh;
	ret->tf.xendian = xendian;
	ret->buf = (UINT8 *)(ret + 1);
	ret->bufsize = buffersize;
	return((TEXTFILEH)ret);

tfo_err2:
	file_close(fh);

tfo_err1:
	return(NULL);
}

void textfile_close(TEXTFILEH tfh) {

	if (tfh) {
		file_close(((TEXTFH)tfh)->fh);
		_MFREE(tfh);
	}
}

BRESULT textfile_read(TEXTFILEH tfh, OEMCHAR *buffer, UINT size) {

	TEXTREAD	tr;

	tr = (TEXTREAD)tfh;
	if (tr) {
		if (sizeof(OEMCHAR) == 1) {
			return(readlineA(tr, buffer, size));
		}
		else if (sizeof(OEMCHAR) == 2) {
			return(readlineW(tr, buffer, size));
		}
	}
	return(FAILURE);
}

