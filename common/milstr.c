#include	"compiler.h"


// ---- ANK / UCS2 / UCS4

#if defined(SUPPORT_ANK)
int milank_charsize(const OEMCHAR *str) {

	return((str[0] != '\0')?1:0);
}

int milank_cmp(const OEMCHAR *str, const OEMCHAR *cmp) {

	int		s;
	int		c;

	do {
		s = *str++;
		if ((s >= 'a') && (s <= 'z')) {
			s -= 0x20;
		}
		c = *cmp++;
		if ((c >= 'a') && (c <= 'z')) {
			c -= 0x20;
		}
		if (s != c) {
			return((s > c)?1:-1);
		}
	} while(s);
	return(0);
}

int milank_memcmp(const OEMCHAR *str, const OEMCHAR *cmp) {

	int		s;
	int		c;

	do {
		c = *cmp++;
		if (c == 0) {
			return(0);
		}
		if ((c >= 'a') && (c <= 'z')) {
			c -= 0x20;
		}
		s = *str++;
		if ((s >= 'a') && (s <= 'z')) {
			s -= 0x20;
		}
	} while(s == c);
	return((s > c)?1:-1);
}

void milank_ncpy(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {

	int		i;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		dst[i] = '\0';
	}
}

void milank_ncat(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {

	int		i;
	int		j;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) {
				break;
			}
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		dst[i] = '\0';
	}
}

OEMCHAR *milank_chr(const OEMCHAR *str, int c) {

	int		s;

	if (str) {
		do {
			s = *str;
			if (s == c) {
				return((OEMCHAR *)str);
			}
			str++;
		} while(s);
	}
	return(NULL);
}
#endif


// ---- Shift-JIS

#if defined(SUPPORT_SJIS)
int milsjis_charsize(const char *str) {

	int		pos;

	pos = ((((str[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c)?1:0;
	return((str[pos] != '\0')?(pos+1):0);
}

int milsjis_cmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		s = (BYTE)*str++;
		if ((((s ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			c = (BYTE)*cmp++;
			if (s != c) {
				goto mscp_err;
			}
			s = (BYTE)*str++;
			c = (BYTE)*cmp++;
		}
		else {
			if (((s - 'a') & 0xff) < 26) {
				s -= 0x20;
			}
			c = (BYTE)*cmp++;
			if (((c - 'a') & 0xff) < 26) {
				c -= 0x20;
			}
		}
		if (s != c) {
			goto mscp_err;
		}
	} while(s);
	return(0);

mscp_err:
	return((s > c)?1:-1);
}

int milsjis_memcmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		c = (BYTE)*cmp++;
		if ((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			s = (BYTE)*str++;
			if (c != s) {
				break;
			}
			c = (BYTE)*cmp++;
			s = (BYTE)*str++;
		}
		else if (c) {
			if (((c - 'a') & 0xff) < 26) {
				c &= ~0x20;
			}
			s = (BYTE)*str++;
			if (((s - 'a') & 0xff) < 26) {
				s &= ~0x20;
			}
		}
		else {
			return(0);
		}
	} while(s == c);
	return((s > c)?1:-1);
}

int milsjis_kanji1st(const char *str, int pos) {

	int		ret;

	ret = 0;
	while((pos >= 0) &&
		((((str[pos--] ^ 0x20) - 0xa1) & 0xff) < 0x3c)) {
		ret ^= 1;
	}
	return(ret);
}

int milsjis_kanji2nd(const char *str, int pos) {

	int		ret = 0;

	while((pos > 0) && ((((str[--pos] ^ 0x20) - 0xa1) & 0xff) < 0x3c)) {
		ret ^= 1;
	}
	return(ret);
}

void milsjis_ncpy(char *dst, const char *src, int maxlen) {

	int		i;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		if (i) {
			if (milsjis_kanji1st(src, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

void milsjis_ncat(char *dst, const char *src, int maxlen) {

	int		i;
	int		j;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) {
				break;
			}
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		if ((i > 0) && (j > 0)) {
			if (milsjis_kanji1st(dst, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

char *milsjis_chr(const char *str, int c) {

	int		s;

	if (str) {
		do {
			s = *str;
			if (s == c) {
				return((char *)str);
			}
			if ((((s ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
				str++;
				s = *str;
			}
			str++;
		} while(s);
	}
	return(NULL);
}
#endif


// ---- EUC

#if defined(SUPPORT_EUC)		// ‚ ‚ê ”¼ŠpƒJƒi–Y‚ê‚Ä‚é‚¼H
int mileuc_charsize(const char *str) {

	int		pos;

	pos = (((str[0] - 0xa1) & 0xff) < 0x5d)?1:0;
	return((str[pos] != '\0')?(pos+1):0);
}

int mileuc_cmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		s = (BYTE)*str++;
		if (((s - 0xa1) & 0xff) < 0x5d) {
			c = (BYTE)*cmp++;
			if (s != c) {
				goto mscp_err;
			}
			s = (BYTE)*str++;
			c = (BYTE)*cmp++;
		}
		else {
			if (((s - 'a') & 0xff) < 26) {
				s -= 0x20;
			}
			c = (BYTE)*cmp++;
			if (((c - 'a') & 0xff) < 26) {
				c -= 0x20;
			}
		}
		if (s != c) {
			goto mscp_err;
		}
	} while(s);
	return(0);

mscp_err:
	return((s > c)?1:-1);
}

int mileuc_memcmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		c = (BYTE)*cmp++;
		if (((c - 0xa1) & 0xff) < 0x5d) {
			s = (BYTE)*str++;
			if (c != s) {
				break;
			}
			c = (BYTE)*cmp++;
			s = (BYTE)*str++;
		}
		else if (c) {
			if (((c - 'a') & 0xff) < 26) {
				c -= 0x20;
			}
			s = (BYTE)*str++;
			if (((s - 'a') & 0xff) < 26) {
				s -= 0x20;
			}
		}
		else {
			return(0);
		}
	} while(s == c);
	return((s > c)?1:-1);
}

int mileuc_kanji1st(const char *str, int pos) {

	int		ret;

	ret = 0;
	while((pos >= 0) &&
		(((str[pos--] - 0xa1) & 0xff) < 0x5d)) {
		ret ^= 1;
	}
	return(ret);
}

int mileuc_kanji2nd(const char *str, int pos) {

	int		ret = 0;

	while((pos > 0) && (((str[--pos] - 0xa1) & 0xff) < 0x5d)) {
		ret ^= 1;
	}
	return(ret);
}

void mileuc_ncpy(char *dst, const char *src, int maxlen) {

	int		i;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		if (i) {
			if (mileuc_kanji1st(src, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

void mileuc_ncat(char *dst, const char *src, int maxlen) {

	int		i;
	int		j;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) {
				break;
			}
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		if ((i > 0) && (j > 0)) {
			if (mileuc_kanji1st(dst, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

char *mileuc_chr(const char *str, int c) {

	int		s;

	if (str) {
		do {
			s = *str;
			if (s == c) {
				return((char *)str);
			}
			if (((s - 0xa1) & 0xff) < 0x5d) {
				str++;
				s = *str;
			}
			str++;
		} while(s);
	}
	return(NULL);
}
#endif


// ---- UTF8

#if defined(SUPPORT_UTF8)
int milutf8_charsize(const char *str) {

	if (str[0] == '\0') {
		return(0);
	}
	else if (!(str[0] & 0x80)) {
		return(1);
	}
	else if ((str[0] & 0xe0) == 0xc0) {
		if ((str[1] & 0xc0) == 0x80) {
			return(2);
		}
	}
	else if ((str[0] & 0xf0) == 0xe0) {
		if (((str[1] & 0xc0) == 0x80) ||
			((str[2] & 0xc0) == 0x80)) {
			return(3);
		}
	}
	return(0);
}

int milutf8_cmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		s = (UINT8)*str++;
		if (((s - 'a') & 0xff) < 26) {
			s -= 0x20;
		}
		c = (BYTE)*cmp++;
		if (((c - 'a') & 0xff) < 26) {
			c -= 0x20;
		}
		if (s != c) {
			return((s > c)?1:-1);
		}
	} while(s);
	return(0);
}

int milutf8_memcmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		c = (UINT8)*cmp++;
		if (c == 0) {
			return(0);
		}
		if (((c - 'a') & 0xff) < 26) {
			c -= 0x20;
		}
		s = (UINT8)*str++;
		if (((s - 'a') & 0xff) < 26) {
			s -= 0x20;
		}
	} while(s == c);
	return((s > c)?1:-1);
}

int milutf8_kanji1st(const char *str, int pos) {

	return(((str[pos] & 0xc0) >= 0xc0)?1:0);
}

int milutf8_kanji2nd(const char *str, int pos) {

	return(((str[pos] & 0xc0) == 0x80)?1:0);
}

void milutf8_ncpy(char *dst, const char *src, int maxlen) {

	int		i;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		while((i) && ((dst[i] & 0xc0) == 0x80)) {
			i--;
		}
		dst[i] = '\0';
	}
}

void milutf8_ncat(char *dst, const char *src, int maxlen) {

	int		i;
	int		j;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) {
				break;
			}
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		while((i) && ((dst[i] & 0xc0) == 0x80)) {
			i--;
		}
		dst[i] = '\0';
	}
}

char *milutf8_chr(const char *str, int c) {

	int		s;

	if (str) {
		do {
			s = *str;
			if (s == c) {
				return((char *)str);
			}
			str++;
		} while(s);
	}
	return(NULL);
}
#endif


// ---- other

int milstr_extendcmp(const char *str, const char *cmp) {

	int		c;
	int		s;

	do {
		while(1) {
			c = (BYTE)*cmp++;
			if (!c) {
				return(0);
			}
			if (((c - '0') & 0xff) < 10) {
				break;
			}
			c |= 0x20;
			if (((c - 'a') & 0xff) < 26) {
				break;
			}
		}
		while(1) {
			s = *str++;
			if (!s) {
				break;
			}
			if (((s - '0') & 0xff) < 10) {
				break;
			}
			s |= 0x20;
			if (((s - 'a') & 0xff) < 26) {
				break;
			}
		}
	} while(s == c);
	return((s > c)?1:-1);
}

OEMCHAR *milstr_nextword(const OEMCHAR *str) {

	if (str) {
		while((*str > '\0') && (*str <= ' ')) {
			str++;
		}
	}
	return((OEMCHAR *)str);
}

int milstr_getarg(OEMCHAR *str, OEMCHAR *arg[], int maxarg) {

	int		ret = 0;
	OEMCHAR	*p;
	BOOL	quot;

	while(maxarg--) {
		quot = FALSE;
		while((*str > '\0') && (*str <= ' ')) {
			str++;
		}
		if (*str == '\0') {
			break;
		}
		arg[ret++] = str;
		p = str;
		while(*str) {
			if (*str == '\"') {
				quot = !quot;
			}
			else if ((quot) || (*str < '\0') || (*str > ' ')) {
				*p++ = *str;
			}
			else {
				str++;
				break;
			}
			str++;
		}
		*p = '\0';
	}
	return(ret);
}

long milstr_solveHEX(const OEMCHAR *str) {

	long	ret;
	int		i;
	char	c;

	ret = 0;
	for (i=0; i<8; i++) {
		c = *str++;
		if ((c >= '0') && (c <= '9')) {
			c -= '0';
		}
		else if ((c >= 'A') && (c <= 'F')) {
			c -= '7';
		}
		else if ((c >= 'a') && (c <= 'f')) {
			c -= 'W';
		}
		else {
			break;
		}
		ret <<= 4;
		ret += (long)c;
	}
	return(ret);
}

long milstr_solveINT(const OEMCHAR *str) {

	long	ret;
	int		c;
	int		s = 1;

	ret = 0;
	c = *str;
	if (c == '+') {
		str++;
	}
	else if (c == '-') {
		str++;
		s = -1;
	}
	while(1) {
		c = *str++;
		c -= '0';
		if ((unsigned)c < 10) {
			ret *= 10;
			ret += c;
		}
		else {
			break;
		}
	}
	return(ret * s);
}

OEMCHAR *milstr_list(const OEMCHAR *lststr, UINT pos) {

	if (lststr) {
		while(pos) {
			pos--;
			while(*lststr++ != '\0') {
			}
		}
	}
	return((OEMCHAR *)lststr);
}

