#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"textfile.h"
#include	"profile.h"


static void strdelspace(char **buf, int *size) {

	char	*p;
	int		len;

	p = *buf;
	len = *size;
	while((len > 0) && (p[0] == ' ')) {
		p++;
		len--;
	}
	while((len > 0) && (p[len - 1] == ' ')) {
		len--;
	}
	*buf = p;
	*size = len;
}

static char *profana(char *buf, char **data) {

	int		len;
	char	*buf2;
	int		l;

	len = strlen(buf);
	strdelspace(&buf, &len);
	if ((len >= 2) && (buf[0] == '[') && (buf[len - 1] == ']')) {
		buf++;
		len -= 2;
		strdelspace(&buf, &len);
		buf[len] = '\0';
		*data = NULL;
		return(buf);
	}
	for (l=0; l<len; l++) {
		if (buf[l] == '=') {
			break;
		}
	}
	if (l < len) {
		len -= (l + 1);
		buf2 = buf + (l + 1);
		strdelspace(&buf, &l);
		buf[l] = '\0';
		strdelspace(&buf2, &len);
		if ((len >= 2) && (buf2[0] == '\"') && (buf2[len - 1] == '\"')) {
			buf2++;
			len -= 2;
			strdelspace(&buf2, &len);
		}
		buf2[len] = '\0';
		*data = buf2;
		return(buf);
	}
	return(NULL);
}

BOOL profile_enum(const char *filename, void *arg,
							BOOL (*proc)(void *arg, const char *para,
									const char *key, const char *data)) {
	TEXTFILEH	fh;
	BOOL		r;
	char		buf[0x200];
	char		para[0x100];
	char		*key;
	char		*data;

	r = FALSE;
	if (proc == NULL) {
		goto gden_err0;
	}
	fh = textfile_open(filename, 0x800);
	if (fh == NULL) {
		goto gden_err0;
	}
	para[0] = '\0';
	while(1) {
		if (textfile_read(fh, buf, sizeof(buf)) != SUCCESS) {
			break;
		}
		key = profana(buf, &data);
		if (key) {
			if (data == NULL) {
				milstr_ncpy(para, key, sizeof(para));
			}
			else {
				r = proc(arg, para, key, data);
				if (r != SUCCESS) {
					break;
				}
			}
		}
	}
	textfile_close(fh);

gden_err0:
	return(r);
}


// ----

const char *profile_getarg(const char *str, char *buf, UINT leng) {

	UINT8	c;

	if (leng) {
		leng--;
	}
	else {
		buf = NULL;
	}
	if (str) {
		c = (UINT8)*str;
		while(((c - 1) & 0xff) < 0x20) {
			str++;
			c = (UINT8)*str;
		}
		if (c == 0) {
			str = NULL;
		}
	}
	if (str) {
		c = (UINT8)*str;
		while(c > 0x20) {
			if (leng) {
				*buf++ = c;
				leng--;
			}
			str++;
			c = (UINT8)*str;
		}
	}
	if (buf) {
		buf[0] = '\0';
	}
	return(str);
}



// ---- ‚Ü‚¾ƒeƒXƒg

typedef struct {
	UINT	applen;
	UINT	keylen;
	UINT	pos;
	UINT	size;
	UINT	apphit;
const char	*data;
	UINT	datasize;
} PFPOS;

static char *delspace(const char *buf, UINT *len) {

	UINT	l;

	if ((buf != NULL) && (len != NULL)) {
		l = *len;
		while((l) && (buf[0] == ' ')) {
			l--;
			buf++;
		}
		while((l) && (buf[l - 1] == ' ')) {
			l--;
		}
		*len = l;
	}
	return((char *)buf);
}

static BOOL seakey(PFILEH hdl, PFPOS *pfp, const char *app, const char *key) {

	PFPOS	ret;
	UINT	pos;
	UINT	size;
	char	*buf;
	UINT	len;
	UINT	cnt;

	if ((hdl == NULL) || (app == NULL) || (key == NULL)) {
		return(FAILURE);
	}
	ZeroMemory(&ret, sizeof(ret));
	ret.applen = strlen(app);
	ret.keylen = strlen(key);
	if ((ret.applen == 0) || (ret.keylen == 0)) {
		return(FAILURE);
	}

	pos = 0;
	size = hdl->size;
	while(size) {
		buf = hdl->buffer + pos;
		len = 0;
		cnt = 0;
		do {
			if (buf[len] == '\r') {
				if (((len + 1) < size) && (buf[len + 1] == '\n')) {
					cnt = 2;
				}
				else {
					cnt = 1;
				}
				break;
			}
			if (buf[len] == '\n') {
				cnt = 1;
				break;
			}
			len++;
		} while(len < size);
		cnt += len;
		buf = delspace(buf, &len);
		if ((len >= 2) && (buf[0] == '[') && (buf[len - 1] == ']')) {
			if (ret.apphit) {
				break;
			}
			buf++;
			len -= 2;
			buf = delspace(buf, &len);
			if ((len == ret.applen) && (!milstr_memcmp(buf, app))) {
				ret.apphit = 1;
			}
		}
		else if ((ret.apphit) && (len > ret.keylen) &&
				(!milstr_memcmp(buf, key))) {
			buf += ret.keylen;
			len -= ret.keylen;
			buf = delspace(buf, &len);
			if ((len) && (buf[0] == '=')) {
				buf++;
				len--;
				buf = delspace(buf, &len);
				ret.pos = pos;
				ret.size = cnt;
				ret.data = buf;
				ret.datasize = len;
				break;
			}
		}
		if (len) {
			ret.pos = pos + cnt;
			ret.size = 0;
		}
		pos += cnt;
		size -= cnt;
	}
	if (pfp) {
		*pfp = ret;
	}
	return(SUCCESS);
}

static BOOL replace(PFILEH hdl, UINT pos, UINT size1, UINT size2) {

	UINT	cnt;
	UINT	size;
	char	*p;
	char	*q;

	size1 += pos;
	size2 += pos;
	if (size1 > hdl->size) {
		return(FAILURE);
	}
	cnt = hdl->size - size1;
	if (size1 < size2) {
		size = size2 - size1;
		if ((hdl->size + size) > hdl->buffers) {
			return(FAILURE);
		}
		hdl->size += size;
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				--cnt;
				q[cnt] = p[cnt];
			} while(cnt);
		}
	}
	else if (size1 > size2) {
		hdl->size -= (size1 - size2);
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				*q++ = *p++;
			} while(--cnt);
		}
	}
	hdl->flag |= PFILEH_MODIFY;
	return(SUCCESS);
}

PFILEH profile_open(const char *filename, UINT flag) {

	FILEH	fh;
	UINT	filesize;
	UINT	size;
	PFILEH	ret;

	if (filename == NULL) {
		goto pfore_err1;
	}
	fh = file_open(filename);
	filesize = 0;
	if (fh != FILEH_INVALID) {
		filesize = file_getsize(fh);
	}
	else if (flag & PFILEH_READONLY) {
		goto pfore_err1;
	}
	else {
		fh = file_create(filename);
		if (fh == FILEH_INVALID) {
			goto pfore_err1;
		}
	}
	size = filesize + 0x2000;
	ret = (PFILEH)_MALLOC(sizeof(_PFILEH) + size, filename);
	if (ret == NULL) {
		goto pfore_err2;
	}
	if (filesize) {
		if (file_read(fh, ret + 1, filesize) != filesize) {
			goto pfore_err3;
		}
	}
	file_close(fh);
	ret->buffer = (BYTE *)(ret + 1);
	ret->buffers = size;
	ret->size = filesize;
	ret->flag = flag;
	file_cpyname(ret->path, filename, sizeof(ret->path));
	return(ret);

pfore_err3:
	_MFREE(ret);

pfore_err2:
	file_close(fh);

pfore_err1:
	return(NULL);
}

void profile_close(PFILEH hdl) {

	FILEH	fh;

	if (hdl) {
		if (hdl->flag & PFILEH_MODIFY) {
			fh = file_create(hdl->path);
			if (fh != FILEH_INVALID) {
				file_write(fh, hdl->buffer, hdl->size);
				file_close(fh);
			}
		}
		_MFREE(hdl);
	}
}

BOOL profile_read(const char *app, const char *key, const char *def,
										char *ret, UINT size, PFILEH hdl) {

	PFPOS	pfp;

	if ((seakey(hdl, &pfp, app, key) != SUCCESS) || (pfp.data == NULL)) {
		if (def == NULL) {
			def = str_null;
		}
		milstr_ncpy(ret, def, size);
		return(FAILURE);
	}
	else {
		size = min(size, pfp.datasize + 1);
		milstr_ncpy(ret, pfp.data, size);
		return(SUCCESS);
	}
}

BOOL profile_write(const char *app, const char *key,
											const char *data, PFILEH hdl) {

	PFPOS	pfp;
	UINT	newsize;
	UINT	datalen;
	char	*buf;

	if ((hdl == NULL) || (hdl->flag & PFILEH_READONLY) ||
		(data == NULL) || (seakey(hdl, &pfp, app, key) != SUCCESS)) {
		return(FAILURE);
	}
	if (!pfp.apphit) {
		newsize = pfp.applen + 2 + 2;
		if (replace(hdl, pfp.pos, 0, newsize) != SUCCESS) {
			return(FAILURE);
		}
		buf = hdl->buffer + pfp.pos;
		*buf++ = '[';
		CopyMemory(buf, app, pfp.applen);
		buf += pfp.applen;
		buf[0] = ']';
		buf[1] = '\r';
		buf[2] = '\n';
		pfp.pos += newsize;
	}
	datalen = strlen(data);
	newsize = pfp.keylen + 1 + datalen + 2;
	if (replace(hdl, pfp.pos, pfp.size, newsize) != SUCCESS) {
		return(FAILURE);
	}
	buf = hdl->buffer + pfp.pos;
	CopyMemory(buf, key, pfp.keylen);
	buf += pfp.keylen;
	*buf++ = '=';
	CopyMemory(buf, data, datalen);
	buf += datalen;
	buf[0] = '\r';
	buf[1] = '\n';
	return(SUCCESS);
}


// ----

void profile_iniread(const char *path, const char *app,
											const PFTBL *tbl, UINT count) {

	PFILEH	pfh;
const PFTBL	*p;
const PFTBL	*pterm;
	char	work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (profile_read(app, p->item, NULL, work, sizeof(work), pfh)
																== SUCCESS) {
			switch(p->itemtype) {
				case PFTYPE_SINT32:
				case PFTYPE_UINT32:
				*(UINT32 *)p->value = (UINT32)milstr_solveINT(work);
				break;
			}
		}
		p++;
	}
	profile_close(pfh);
}

void profile_iniwrite(const char *path, const char *app,
											const PFTBL *tbl, UINT count) {

	PFILEH	pfh;
const PFTBL	*p;
const PFTBL	*pterm;
const char	*set;
	char	work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		work[0] = '\0';
		set = work;
		switch(p->itemtype) {
			case PFTYPE_SINT32:
				SPRINTF(work, str_d, *((SINT32 *)p->value));
				break;

			default:
				set = NULL;
				break;
		}
		if (set) {
			profile_write(app, p->item, set, pfh);
		}
		p++;
	}
	profile_close(pfh);
}

