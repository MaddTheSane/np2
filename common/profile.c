#include	"compiler.h"
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


// ----

typedef struct {
	UINT8	num;
	char	str[7];
} KEYNAME;

#include	"pf_key.tbl"

static const char *getarg(const char *str, char *buf, UINT leng) {

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

static const KEYNAME *searchkeynum(const char *str) {

const KEYNAME	*n;
const KEYNAME	*nterm;

	n = keyname;
	nterm = keyname + (sizeof(keyname) / sizeof(KEYNAME));
	while(n < nterm) {
		if (!milstr_cmp(str, n->str)) {
			return(n);
		}
		n++;
	}
	return(NULL);
}

static const KEYNAME *searchkeystr(UINT8 num) {

const KEYNAME	*n;
const KEYNAME	*nterm;

	n = keyname;
	nterm = keyname + (sizeof(keyname) / sizeof(KEYNAME));
	while(n < nterm) {
		if (n->num == num) {
			return(n);
		}
		n++;
	}
	return(NULL);
}

UINT profile_setkeys(const char *str, UINT8 *key, UINT keymax) {

	UINT		ret;
	char		work[7];
const KEYNAME	*k;

	ret = 0;
	while(ret < keymax) {
		str = getarg(str, work, sizeof(work));
		if (str == NULL) {
			break;
		}
		k = searchkeynum(work);
		if (k) {
			key[ret] = k->num;
			ret++;
		}
	}
	return(ret);
}

void profile_getkeys(char *str, UINT strmax, const UINT8 *key, UINT keys) {

	UINT		space;
	UINT		i;
const KEYNAME	*k;
	UINT		len;

	if ((str == NULL) || (strmax == 0)) {
		return;
	}
	strmax--;
	space = 0;
	for (i=0; i<keys; i++) {
		k = searchkeystr(key[i]);
		if (k) {
			len = strlen(k->str);
			if ((len + space) > strmax) {
				break;
			}
			if (space) {
				*str++ = ' ';
			}
			CopyMemory(str, k->str, len);
			str += len;
			space = 1;
		}
	}
	str[0] = '\0';
}

