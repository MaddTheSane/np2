#include	"compiler.h"


int milstr_cmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	while(1) {
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
		if (!s) {
			break;
		}
	}
	return(0);

mscp_err:
	return((s > c)?1:-1);
}

BOOL milstr_memcmp(const char *str, const char *cmp) {

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
	return(1);
}

BOOL milstr_extendcmp(const char *str, const char *cmp) {

	char	c, s = '\0';

	while(*cmp) {
		while(*cmp) {
			s = *cmp++;
			if (!s) {
				return(0);
			}
			if ((s >= '0') && (s <= '9')) {
				break;
			}
			s &= 0xdf;
			if ((s >= 'A') && (s <= 'Z')) {
				break;
			}
		}
		while(1) {
			c = *str++;
			if (!c) {
				return(1);
			}
			if ((c >= '0') && (c <= '9')) {
				break;
			}
			c &= 0xdf;
			if ((c >= 'A') && (c <= 'Z')) {
				break;
			}
		}
		if (c != s) {
			return(1);
		}
	}
	return(0);
}

int milstr_kanji1st(const char *str, int pos) {

	int		ret;

	ret = 0;
	while((pos >= 0) &&
		((((str[pos--] ^ 0x20) - 0xa1) & 0xff) < 0x3c)) {
		ret ^= 1;
	}
	return(ret);
}

int milstr_kanji2nd(const char *str, int pos) {

	int		ret = 0;

	while((pos) && ((((str[--pos] ^ 0x20) - 0xa1) & 0xff) < 0x3c)) {
		ret ^= 1;
	}
	return(ret);
}

void milstr_ncpy(char *dst, const char *src, int maxlen) {

	int		i;

	if (maxlen--) {
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		if (i) {
			if (milstr_kanji1st(src, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

void milstr_ncat(char *dst, const char *src, int maxlen) {

	int		i;
	int		j;

	if (maxlen--) {
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) break;
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		if ((i) && (j)) {
			if (milstr_kanji1st(dst, i-1)) {
				i--;
			}
		}
		dst[i] = '\0';
	}
}

int milstr_getarg(char *str, char *arg[], int maxarg) {

	int		ret = 0;
	char	*p;
	BOOL	quot;

	while(maxarg--) {
		quot = FALSE;
		while((*str) && (((BYTE)*str) <= 0x20)) {
			str++;
		}
		if (*str == '\0') {
			break;
		}
		arg[ret++] = str;
		p = str;
		while(*str) {
			if (*str == 0x22) {
				quot = !quot;
			}
			else if ((((BYTE)*str) > 0x20) || (quot)) {
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

long milstr_solveHEX(const char *str) {

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

long milstr_solveINT(const char *str) {

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

