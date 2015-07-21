/**
 * @file	profile.c
 * @brief	Implementation of the profiler
 */

#include "compiler.h"
#include "strres.h"
#include "profile.h"
#include "dosio.h"
#include "textfile.h"
#if defined(SUPPORT_TEXTCNV)
#include "textcnv.h"
#endif

/**
 * End of line style
 */
static const OEMCHAR s_eol[] =
{
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
	'\r',
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
	'\n',
#endif
};

/**
 * Trims space
 * @param[in] lpString The pointer to a string
 * @param[in, out] pcchString The size, in characters
 * @return The start of the string
 */
static OEMCHAR* TrimSpace(const OEMCHAR *lpString, UINT *pcchString)
{
	UINT cchString;

	cchString = *pcchString;
	while ((cchString > 0) && (lpString[0] == ' '))
	{
		lpString++;
		cchString--;
	}
	while ((cchString > 0) && (lpString[cchString - 1] == ' '))
	{
		cchString--;
	}
	*pcchString = cchString;
	return (OEMCHAR *)lpString;
}

/**
 * Parses line
 * @param[in] lpString The pointer to a string
 * @param[in,out] lpcchString The pointer to a length
 * @param[out] lppData The pointer to data
 * @param[out] lpcchData The pointer to data-length
 * @return The start of the string
 */
static OEMCHAR *ParseLine(const OEMCHAR *lpString, UINT *lpcchString, OEMCHAR **lppData, UINT *lpcchData)
{
	UINT cchString;
	UINT nIndex;
	const OEMCHAR *lpData = NULL;
	UINT cchData = 0;

	cchString = *lpcchString;
	lpString = TrimSpace(lpString, &cchString);

	if ((cchString >= 2) && (lpString[0] == '[') && (lpString[cchString - 1] == ']'))
	{
		lpString++;
		cchString -= 2;
	}
	else
	{
		for (nIndex = 0; nIndex < cchString; nIndex++)
		{
			if (lpString[nIndex] == '=')
			{
				break;
			}
		}
		if (nIndex >= cchString)
		{
			return NULL;
		}
		lpData = lpString + (nIndex + 1);
		cchData = cchString - (nIndex + 1);
		cchString = nIndex;
	}

	lpString = TrimSpace(lpString, &cchString);
	lpData = TrimSpace(lpData, &cchData);
	if ((cchData >= 2) && (lpData[0] == '\"') && (lpData[cchData - 1] == '\"'))
	{
		lpData++;
		cchData -= 2;
		lpData = TrimSpace(lpData, &cchData);
	}

	*lpcchString = cchString;
	if (lppData)
	{
		*lppData = (OEMCHAR*)lpData;
	}
	if (lpcchData)
	{
		*lpcchData = cchData;
	}
	return (OEMCHAR*)lpString;
}

/**
 * Retrieves a string from the specified section in an initialization file
 * @param[in] lpFileName The name of the initialization file
 * @param[in] lpParam An application-defined value to be passed to the callback function.
 * @param[in] lpFunc A pointer to an application-defined callback function
 * @retval SUCCESS If the function succeeds
 * @retval FAILIURE If the function fails
 */
BRESULT profile_enum(const OEMCHAR *lpFileName, void *lpParam, PROFILEENUMPROC lpFunc)
{
	TEXTFILEH fh;
	BRESULT r;
	OEMCHAR szAppName[256];
	OEMCHAR szBuffer[512];
	UINT cchBuffer;
	OEMCHAR *lpKeyName;
	OEMCHAR *lpString;
	UINT cchString;

	if (lpFunc == NULL)
	{
		return SUCCESS;
	}
	fh = textfile_open(lpFileName, 0x800);
	if (fh == NULL)
	{
		return SUCCESS;
	}

	r = SUCCESS;
	szAppName[0] = '\0';
	while (textfile_read(fh, szBuffer, NELEMENTS(szBuffer)) == SUCCESS)
	{
		cchBuffer = (UINT)OEMSTRLEN(szBuffer);
		lpKeyName = ParseLine(szBuffer, &cchBuffer, &lpString, &cchString);
		if (lpKeyName)
		{
			lpKeyName[cchBuffer] = '\0';
			if (lpString == NULL)
			{
				milstr_ncpy(szAppName, lpKeyName, NELEMENTS(szAppName));
			}
			else
			{
				lpString[cchString] = '\0';
				r = (*lpFunc)(lpParam, szAppName, lpKeyName, lpString);
				if (r != SUCCESS)
				{
					break;
				}
			}
		}
	}
	textfile_close(fh);
	return r;
}


/* profiler */

/**
 * @brief the structure of profiler's handle
 */
struct tagProfileHandle
{
	OEMCHAR	*buffer;
	UINT	buffers;
	UINT	size;
	UINT8	hdr[4];
	UINT	hdrsize;
	UINT	flag;
	OEMCHAR	path[MAX_PATH];
};
typedef struct tagProfileHandle _PFILEH;	/*!< defines handle */

/**
 * @brief
 */
struct tagProfilePos
{
	UINT cchAppName;
	UINT cchKeyName;
	UINT		pos;
	UINT		size;
	UINT		apphit;
	const OEMCHAR *lpString;
	UINT cchString;
};
typedef struct tagProfilePos	PFPOS;		/*!< defines the structure of position */

#define	PFBUFSIZE	(1 << 8)

static BRESULT SearchKey(PFILEH hdl, PFPOS *pfp, const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName)
{
	PFPOS ret;
	const OEMCHAR *lpProfile;
	UINT cchProfile;
	UINT nIndex;
	UINT nSize;
	UINT cchLeft;
	const OEMCHAR *lpLeft;
	OEMCHAR *lpRight;
	UINT cchRight;

	if ((hdl == NULL) || (lpAppName == NULL) || (lpKeyName == NULL))
	{
		return FAILURE;
	}
	memset(&ret, 0, sizeof(ret));
	ret.cchAppName = (UINT)OEMSTRLEN(lpAppName);
	ret.cchKeyName = (UINT)OEMSTRLEN(lpKeyName);
	if ((ret.cchAppName == 0) || (ret.cchKeyName == 0))
	{
		return FAILURE;
	}

	lpProfile = hdl->buffer;
	cchProfile = hdl->size;
	while (cchProfile > 0)
	{
		nIndex = 0;
		while ((nIndex < cchProfile) && (lpProfile[nIndex] != '\r') && (lpProfile[nIndex] != '\n'))
		{
			nIndex++;
		}
		lpLeft = lpProfile;
		cchLeft = nIndex;

		nSize = nIndex;
		if ((nSize < cchProfile) && (lpProfile[nSize] == '\r'))
		{
			nSize++;
		}
		if ((nSize < cchProfile) && (lpProfile[nSize] == '\n'))
		{
			nSize++;
		}

		lpLeft = ParseLine(lpLeft, &cchLeft, &lpRight, &cchRight);
		if (lpLeft)
		{
			if (lpRight == NULL)
			{
				if (ret.apphit)
				{
					break;
				}
				if ((cchLeft == ret.cchAppName) && (!milstr_memcmp(lpLeft, lpAppName)))
				{
					ret.apphit = 1;
				}
			}
			else if ((ret.apphit) && (cchLeft == ret.cchKeyName) && (!milstr_memcmp(lpLeft, lpKeyName)))
			{
				ret.pos = (UINT)(lpProfile - hdl->buffer);
				ret.size = nSize;

				ret.lpString = lpRight;
				ret.cchString = cchRight;
				break;
			}
		}

		lpProfile += nSize;
		cchProfile -= nSize;

		if (nIndex)
		{
			ret.pos = (UINT)(lpProfile - hdl->buffer);
			ret.size = 0;
		}
	}
	if (pfp)
	{
		*pfp = ret;
	}
	return SUCCESS;
}

static BRESULT replace(PFILEH hdl, UINT pos, UINT size1, UINT size2) {

	UINT	cnt;
	UINT	size;
	UINT	newsize;
	OEMCHAR	*p;
	OEMCHAR	*q;

	size1 += pos;
	size2 += pos;
	if (size1 > hdl->size) {
		return(FAILURE);
	}
	cnt = hdl->size - size1;
	if (size1 < size2) {
		size = hdl->size + size2 - size1;
		if (size > hdl->buffers) {
			newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
			p = (OEMCHAR *)_MALLOC(newsize * sizeof(OEMCHAR), "profile");
			if (p == NULL) {
				return(FAILURE);
			}
			if (hdl->buffer) {
				CopyMemory(p, hdl->buffer, hdl->buffers * sizeof(OEMCHAR));
				_MFREE(hdl->buffer);
			}
			hdl->buffer = p;
			hdl->buffers = newsize;
		}
		hdl->size = size;
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

static PFILEH registfile(FILEH fh) {

	UINT	rsize;
#if defined(SUPPORT_TEXTCNV)
	TCINF	inf;
#endif
	UINT	hdrsize;
	UINT	width;
	UINT8	hdr[4];
	UINT	size;
	UINT	newsize;
	void	*buf;
#if defined(SUPPORT_TEXTCNV)
	void	*buf2;
#endif
	PFILEH	ret;

	rsize = file_read(fh, hdr, sizeof(hdr));
#if defined(SUPPORT_TEXTCNV)
	if (textcnv_getinfo(&inf, hdr, rsize) == 0) {
		goto rf_err1;
	}
	if (!(inf.caps & TEXTCNV_READ)) {
		goto rf_err1;
	}
	if ((inf.width != 1) && (inf.width != 2)) {
		goto rf_err1;
	}
	hdrsize = inf.hdrsize;
	width = inf.width;
#else
	hdrsize = 0;
	width = 1;
	if ((rsize >= 3) &&
		(hdr[0] == 0xef) && (hdr[1] == 0xbb) && (hdr[2] == 0xbf)) {
		// UTF-8
		hdrsize = 3;
	}
	else if ((rsize >= 2) && (hdr[0] == 0xff) && (hdr[1] == 0xfe)) {
		// UCSLE
		hdrsize = 2;
		width = 2;
#if defined(BYTESEX_BIG)
		goto rf_err1;
#endif
	}
	else if ((rsize >= 2) && (hdr[0] == 0xfe) && (hdr[1] == 0xff)) {
		// UCS2BE
		hdrsize = 2;
		width = 2;
#if defined(BYTESEX_LITTLE)
		goto rf_err1;
#endif
	}
	if (width != sizeof(OEMCHAR)) {
		goto rf_err1;
	}
#endif

	size = file_getsize(fh);
	if (size < hdrsize) {
		goto rf_err1;
	}
	if (file_seek(fh, (long)hdrsize, FSEEK_SET) != (long)hdrsize) {
		goto rf_err1;
	}
	size = (size - hdrsize) / width;
	newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
	buf = _MALLOC(newsize * width, "profile");
	if (buf == NULL) {
		goto rf_err1;
	}
	rsize = file_read(fh, buf, newsize * width) / width;
#if defined(SUPPORT_TEXTCNV)
	if (inf.xendian) {
		textcnv_swapendian16(buf, rsize);
	}
	if (inf.tooem) {
		size = (inf.tooem)(NULL, 0, buf, rsize);
		newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
		buf2 = _MALLOC(newsize * sizeof(OEMCHAR), "profile tmp");
		if (buf2 == NULL) {
			goto rf_err2;
		}
		(inf.tooem)((OEMCHAR *)buf2, size, buf, rsize);
		_MFREE(buf);
		buf = buf2;
		rsize = size;
	}
#endif	// defined(SUPPORT_TEXTCNV)

	ret = (PFILEH)_MALLOC(sizeof(_PFILEH), "profile");
	if (ret == NULL) {
		goto rf_err2;
	}
	ZeroMemory(ret, sizeof(_PFILEH));
	ret->buffer = (OEMCHAR *)buf;
	ret->buffers = newsize;
	ret->size = rsize;
	if (hdrsize) {
		CopyMemory(ret->hdr, hdr, hdrsize);
	}
	ret->hdrsize = hdrsize;
	return(ret);

rf_err2:
	_MFREE(buf);

rf_err1:
	return(NULL);
}

static PFILEH registnew(void)
{
	PFILEH ret;
	const UINT8 *lpHeader;
	UINT cchHeader;

	ret = (PFILEH)_MALLOC(sizeof(*ret), "profile");
	if (ret != NULL)
	{
		memset(ret, 0, sizeof(*ret));

#if defined(OSLANG_UTF8)
		lpHeader = str_utf8;
		cchHeader = sizeof(str_utf8);
#elif defined(OSLANG_UCS2) 
		lpHeader = (UINT8 *)str_ucs2;
		cchHeader = sizeof(str_ucs2);
#else
		lpHeader = NULL;
		cchHeader = 0;
#endif
		if (cchHeader)
		{
			memcpy(ret->hdr, lpHeader, cchHeader);
		}
		ret->hdrsize = cchHeader;
	}
	return ret;
}

/**
 * Opens profiler
 * @param[in] lpFileName The name of the initialization file
 * @param[in] nFlags The flag of opening
 * @return The handle
 */
PFILEH profile_open(const OEMCHAR *lpFileName, UINT nFlags)
{
	PFILEH ret;
	FILEH fh;

	ret = NULL;
	if (lpFileName != NULL)
	{
		fh = file_open_rb(lpFileName);
		if (fh != FILEH_INVALID)
		{
			ret = registfile(fh);
			file_close(fh);
		}
		else if (nFlags & PFILEH_READONLY)
		{
		}
		else
		{
			ret = registnew();
		}
	}
	if (ret)
	{
		ret->flag = nFlags;
		file_cpyname(ret->path, lpFileName, NELEMENTS(ret->path));
	}
	return ret;
}

/**
 * Close
 * @param[in] hdl The handle of profiler
 */
void profile_close(PFILEH hdl)
{
	void	*buf;
	UINT	bufsize;
#if defined(SUPPORT_TEXTCNV)
	TCINF	inf;
	void	*buf2;
	UINT	buf2size;
#endif
	UINT	hdrsize;
	UINT	width;
	FILEH	fh;

	if (hdl == NULL) {
		return;
	}
	buf = hdl->buffer;
	bufsize = hdl->size;
	if (hdl->flag & PFILEH_MODIFY) {
#if defined(SUPPORT_TEXTCNV)
		if (textcnv_getinfo(&inf, hdl->hdr, hdl->hdrsize) == 0) {
			goto wf_err1;
		}
		if (!(inf.caps & TEXTCNV_WRITE)) {
			goto wf_err1;
		}
		if ((inf.width != 1) && (inf.width != 2)) {
			goto wf_err1;
		}
		if (inf.fromoem) {
			buf2size = (inf.fromoem)(NULL, 0, (const OEMCHAR *)buf, bufsize);
			buf2 = _MALLOC(buf2size * inf.width, "profile tmp");
			if (buf2 == NULL) {
				goto wf_err1;
			}
			(inf.fromoem)(buf2, buf2size, (const OEMCHAR *)buf, bufsize);
			_MFREE(buf);
			buf = buf2;
			bufsize = buf2size;
		}
		if (inf.xendian) {
			textcnv_swapendian16(buf, bufsize);
		}
		hdrsize = inf.hdrsize;
		width = inf.width;
#else	// defined(SUPPORT_TEXTCNV)
		hdrsize = hdl->hdrsize;
		width = sizeof(OEMCHAR);
#endif	// defined(SUPPORT_TEXTCNV)
		fh = file_create(hdl->path);
		if (fh == FILEH_INVALID) {
			goto wf_err1;
		}
		if (hdrsize) {
			file_write(fh, hdl->hdr, hdrsize);
		}
		file_write(fh, buf, bufsize * width);
		file_close(fh);
	}

wf_err1:
	if (buf) {
		_MFREE(buf);
	}
	_MFREE(hdl);
}

/**
 * Retrieves the names of all sections in an initialization file.
 * @param[out] lpBuffer A pointer to a buffer that receives the section names
 * @param[in] cchBuffer The size of the buffer pointed to by the lpBuffer parameter, in characters.
 * @param[in] hdl The handle of profiler
 * @return The return value specifies the number of characters copied to the specified buffer
 */
UINT profile_getsectionnames(OEMCHAR *lpBuffer, UINT cchBuffer, PFILEH hdl)
{
	UINT cchWritten = 0;
	const OEMCHAR* lpProfile;
	UINT cchProfile;
	UINT nIndex;
	OEMCHAR *lpKeyName;
	UINT cchKeyName;
	OEMCHAR *lpData;
	UINT cchRemain;

	if ((hdl == NULL) || (cchBuffer <= 1))
	{
		return 0;
	}
	lpProfile = hdl->buffer;
	cchProfile = hdl->size;

	cchBuffer--;

	while (cchProfile > 0)
	{
		nIndex = 0;
		while ((nIndex < cchProfile) && (lpProfile[nIndex] != '\r') && (lpProfile[nIndex] != '\n'))
		{
			nIndex++;
		}

		cchKeyName = nIndex;
		lpKeyName = ParseLine(lpProfile, &cchKeyName, &lpData, NULL);
		if ((lpKeyName != NULL) && (lpData == NULL))
		{
			if (lpBuffer)
			{
				cchRemain = cchBuffer - cchWritten;
				if (cchRemain >= (cchKeyName + 1))
				{
					memcpy(lpBuffer + cchWritten, lpKeyName, cchKeyName * sizeof(*lpBuffer));
					cchWritten += cchKeyName;
					lpBuffer[cchWritten] = '\0';
					cchWritten++;
				}
			}
		}

		lpProfile += nIndex;
		cchProfile -= nIndex;

		if ((cchProfile >= 2) && (lpProfile[0] == '\r') && (lpProfile[1] == '\n'))
		{
			lpProfile++;
			cchProfile--;
		}
		if (cchProfile > 0)
		{
			lpProfile++;
			cchProfile--;
		}
	}

	if (lpBuffer)
	{
		lpBuffer[cchWritten] = '\0';
	}
	return cchWritten;
}

/**
 * Retrieves a string from the specified section in an initialization file
 * @param[in] lpAppName The name of the section containing the key name
 * @param[in] lpKeyName The name of the key whose associated string is to be retrieved
 * @param[in] lpDefault A default string
 * @param[out]lpReturnedString A pointer to the buffer that receives the retrieved string
 * @param[in] nSize The size of the buffer pointed to by the lpReturnedString parameter, in characters
 * @param[in] hdl The handle of profiler
 * @retval SUCCESS If the function succeeds
 * @retval FAILIURE If the function fails
 */
BRESULT profile_read(const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName, const OEMCHAR *lpDefault, OEMCHAR *lpReturnedString, UINT nSize, PFILEH hdl)
{
	PFPOS pfp;
	if ((SearchKey(hdl, &pfp, lpAppName, lpKeyName) != SUCCESS) || (pfp.lpString == NULL))
	{
		if (lpDefault == NULL)
		{
			lpDefault = str_null;
		}
		milstr_ncpy(lpReturnedString, lpDefault, nSize);
		return FAILURE;
	}
	else
	{
		nSize = min(nSize, pfp.cchString + 1);
		milstr_ncpy(lpReturnedString, pfp.lpString, nSize);
		return SUCCESS;
	}
}

/**
 * Copies a string into the specified section of an initialization file.
 * @param[in] lpAppName The name of the section to which the string will be copied
 * @param[in] lpKeyName The name of the key to be associated with a string
 * @param[in] lpString A null-terminated string to be written to the file
 * @param[in] hdl The handle of profiler
 * @retval SUCCESS If the function succeeds
 * @retval FAILIURE If the function fails
 */
BRESULT profile_write(const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName, const OEMCHAR *lpString, PFILEH hdl)
{
	PFPOS pfp;
	UINT cchWrite;
	UINT cchString;
	OEMCHAR *lpBuffer;

	if ((hdl == NULL) || (hdl->flag & PFILEH_READONLY) || (lpString == NULL) || (SearchKey(hdl, &pfp, lpAppName, lpKeyName) != SUCCESS))
	{
		return FAILURE;
	}

	if (pfp.pos != 0)
	{
		lpBuffer = hdl->buffer + pfp.pos;
		if ((lpBuffer[-1] != '\r') && (lpBuffer[-1] != '\n'))
		{
			if (replace(hdl, pfp.pos, 0, NELEMENTS(s_eol)) != SUCCESS)
			{
				return FAILURE;
			}
			memcpy(lpBuffer, s_eol, sizeof(s_eol));
			pfp.pos += NELEMENTS(s_eol);
		}
	}

	if (!pfp.apphit)
	{
		cchWrite = pfp.cchAppName + 2 + NELEMENTS(s_eol);
		if (replace(hdl, pfp.pos, 0, cchWrite) != SUCCESS)
		{
			return FAILURE;
		}
		lpBuffer = hdl->buffer + pfp.pos;
		*lpBuffer++ = '[';
		memcpy(lpBuffer, lpAppName, pfp.cchAppName * sizeof(OEMCHAR));
		lpBuffer += pfp.cchAppName;
		*lpBuffer++ = ']';
		memcpy(lpBuffer, s_eol, sizeof(s_eol));
		pfp.pos += cchWrite;
	}

	cchString = (UINT)OEMSTRLEN(lpString);
	cchWrite = pfp.cchKeyName + 1 + cchString + NELEMENTS(s_eol);
	if (replace(hdl, pfp.pos, pfp.size, cchWrite) != SUCCESS)
	{
		return FAILURE;
	}
	lpBuffer = hdl->buffer + pfp.pos;
	memcpy(lpBuffer, lpKeyName, pfp.cchKeyName * sizeof(OEMCHAR));
	lpBuffer += pfp.cchKeyName;
	*lpBuffer++ = '=';
	memcpy(lpBuffer, lpString, cchString * sizeof(OEMCHAR));
	lpBuffer += cchString;
	memcpy(lpBuffer, s_eol, sizeof(s_eol));

	return SUCCESS;
}


// ----

static void bitmapset(UINT8 *ptr, UINT pos, BOOL set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static BOOL bitmapget(const UINT8 *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void binset(UINT8 *bin, UINT binlen, const OEMCHAR *src) {

	UINT	i;
	UINT8	val;
	BOOL	set;
	OEMCHAR	c;

	for (i=0; i<binlen; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		bin[i] = val;
	}
}

static void binget(OEMCHAR *work, int size, const UINT8 *bin, UINT binlen) {

	UINT	i;
	OEMCHAR	tmp[8];

	if (binlen) {
		OEMSPRINTF(tmp, OEMTEXT("%.2x"), bin[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<binlen; i++) {
		OEMSPRINTF(tmp, OEMTEXT(" %.2x"), bin[i]);
		milstr_ncat(work, tmp, size);
	}
}

void profile_iniread(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFREAD cb) {

	PFILEH	pfh;
const PFTBL	*p;
const PFTBL	*pterm;
	OEMCHAR	work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (profile_read(app, p->item, NULL, work, sizeof(work), pfh)
																== SUCCESS) {
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					milstr_ncpy((OEMCHAR *)p->value, work, p->arg);
					break;

				case PFTYPE_BOOL:
					*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
					break;

				case PFTYPE_BITMAP:
					bitmapset((UINT8 *)p->value, p->arg,
									(!milstr_cmp(work, str_true))?TRUE:FALSE);
					break;

				case PFTYPE_BIN:
					binset((UINT8 *)p->value, p->arg, work);
					break;

				case PFTYPE_SINT8:
				case PFTYPE_UINT8:
					*(UINT8 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT16:
				case PFTYPE_UINT16:
					*(UINT16 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT32:
				case PFTYPE_UINT32:
					*(UINT32 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_HEX8:
					*(UINT8 *)p->value = (UINT8)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX16:
					*(UINT16 *)p->value = (UINT16)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX32:
					*(UINT32 *)p->value = (UINT32)milstr_solveHEX(work);
					break;

				default:
					if (cb != NULL) {
						(*cb)(p, work);
					}
					break;
			}
		}
		p++;
	}
	profile_close(pfh);
}

void profile_iniwrite(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFWRITE cb) {

	PFILEH		pfh;
const PFTBL		*p;
const PFTBL		*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & PFFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case PFTYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case PFTYPE_BITMAP:
					set = (bitmapget((UINT8 *)p->value, p->arg))?
														str_true:str_false;
					break;

				case PFTYPE_BIN:
					binget(work, NELEMENTS(work), (UINT8 *)p->value, p->arg);
					break;

				case PFTYPE_SINT8:
					OEMSPRINTF(work, str_d, *((SINT8 *)p->value));
					break;

				case PFTYPE_SINT16:
					OEMSPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case PFTYPE_SINT32:
					OEMSPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case PFTYPE_UINT8:
					OEMSPRINTF(work, str_u, *((UINT8 *)p->value));
					break;

				case PFTYPE_UINT16:
					OEMSPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case PFTYPE_UINT32:
					OEMSPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case PFTYPE_HEX8:
					OEMSPRINTF(work, str_x, *((UINT8 *)p->value));
					break;

				case PFTYPE_HEX16:
					OEMSPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case PFTYPE_HEX32:
					OEMSPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					if (cb != NULL) {
						set = (*cb)(p, work, NELEMENTS(work));
					}
					else {
						set = NULL;
					}
					break;
			}
			if (set) {
				profile_write(app, p->item, set, pfh);
			}
		}
		p++;
	}
	profile_close(pfh);
}

