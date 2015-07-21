/**
 * @file	profile.h
 * @brief	Interface of the profiler
 */

#if !defined(NP2_PROFILE_H__)
#define	NP2_PROFILE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * An application-defined callback function used with the profile_enum
 */
typedef BRESULT (*PROFILEENUMPROC)(void *lpParam, const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName, const OEMCHAR *lpString);

BRESULT profile_enum(const OEMCHAR *lpFileName, void *lpParam, PROFILEENUMPROC lpFunc);


/* profiler */

/**
 * mode
 */
enum
{
	PFILEH_READONLY		= 0x0001,		/*!< Readonly */
	PFILEH_MODIFY		= 0x0002		/*!< Modified */
};

struct tagProfileHandle;
typedef struct tagProfileHandle* PFILEH;	/*!< defines handle */

PFILEH profile_open(const OEMCHAR *lpFileName, UINT nFlags);
void profile_close(PFILEH hdl);
UINT profile_getsectionnames(OEMCHAR *lpBuffer, UINT cchBuffer, PFILEH hdl);
BRESULT profile_read(const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName, const OEMCHAR *lpDefault, OEMCHAR *lpReturnedString, UINT nSize, PFILEH hdl);
BRESULT profile_write(const OEMCHAR *lpAppName, const OEMCHAR *lpKeyName, const OEMCHAR *lpString, PFILEH hdl);


enum {
	PFTYPE_STR			= 0,
	PFTYPE_BOOL,
	PFTYPE_BITMAP,
	PFTYPE_BIN,
	PFTYPE_SINT8,
	PFTYPE_SINT16,
	PFTYPE_SINT32,
	PFTYPE_UINT8,
	PFTYPE_UINT16,
	PFTYPE_UINT32,
	PFTYPE_HEX8,
	PFTYPE_HEX16,
	PFTYPE_HEX32,
	PFTYPE_USER,
	PFTYPE_MASK			= 0xff,

	PFFLAG_RO			= 0x0100,
	PFFLAG_MAX			= 0x0200,
	PFFLAG_AND			= 0x0400
};

typedef struct {
	OEMCHAR	item[12];
	UINT	itemtype;
	void	*value;
	UINT32	arg;
} PFTBL;

#define	PFSTR(k, f, a)		{OEMTEXT(k), f, a, NELEMENTS(a)}
#define	PFVAL(k, f, a)		{OEMTEXT(k), f, a, 0}
#define	PFMAX(k, f, a, v)	{OEMTEXT(k), f | PFFLAG_MAX, a, v}
#define	PFAND(k, f, a, v)	{OEMTEXT(k), f | PFFLAG_AND, a, v}
#define	PFEXT(k, f, a, v)	{OEMTEXT(k), f, a, v}

typedef void (*PFREAD)(const PFTBL *item, const OEMCHAR *string);
typedef OEMCHAR *(*PFWRITE)(const PFTBL *item, OEMCHAR *string, UINT size);

void profile_iniread(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFREAD cb);
void profile_iniwrite(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFWRITE cb);

#ifdef __cplusplus
}
#endif

#endif	// defined(NP2_PROFILE_H__)

