
#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

typedef struct {
const TCHAR	*title;
const TCHAR	*ext;
const TCHAR	*filter;
	int		defindex;
} FILESEL;

extern const TCHAR str_nc[];
extern const TCHAR str_int0[];
extern const TCHAR str_int1[];
extern const TCHAR str_int2[];
extern const TCHAR str_int4[];
extern const TCHAR str_int5[];
extern const TCHAR str_int6[];

#define	SetDlgItemCheck(a, b, c)	\
			SendDlgItemMessage((a), (b), BM_SETCHECK, (c), 0)

#define	GetDlgItemCheck(a, b)		\
			(((int)SendDlgItemMessage((a), (b), BM_GETCHECK, 0, 0)) != 0)

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define	SETLISTSTR(a, b, c)			\
			dlgs_setliststr((a), (b), (c), NELEMENTS((c)))

#define	SETnLISTSTR(a, b, c, d)		\
			dlgs_setliststr((a), (b), (c), (d))

#define	SETLISTUINT32(a, b, c)		\
			dlgs_setlistuint32((a), (b), (c), NELEMENTS((c)))


BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
											OEMCHAR *path, UINT size, int *ro);
BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
													OEMCHAR *path, UINT size);
BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
													OEMCHAR *path, UINT size);

void dlgs_browsemimpidef(HWND hWnd, UINT16 res);

void dlgs_setliststr(HWND hWnd, UINT16 res, const TCHAR **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, UINT16 res, const UINT32 *item, UINT items);

void dlgs_setlistmidiout(HWND hWnd, UINT16 res, const OEMCHAR *defname);
void dlgs_setlistmidiin(HWND hWnd, UINT16 res, const OEMCHAR *defname);

void dlgs_drawbmp(HDC hdc, UINT8 *bmp);

