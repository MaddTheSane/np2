
#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

typedef struct {
const OEMCHAR	*title;
const OEMCHAR	*ext;
const OEMCHAR	*filter;
	int			defindex;
} FILESEL;

extern const OEMCHAR str_nc[];
extern const OEMCHAR str_int0[];
extern const OEMCHAR str_int1[];
extern const OEMCHAR str_int2[];
extern const OEMCHAR str_int4[];
extern const OEMCHAR str_int5[];
extern const OEMCHAR str_int6[];

#define	SetDlgItemCheck(a, b, c)	\
			SendDlgItemMessage((a), (b), BM_SETCHECK, (c), 0)

#define	GetDlgItemCheck(a, b)		\
			(((int)SendDlgItemMessage((a), (b), BM_GETCHECK, 0, 0))?1:0)

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define	SETLISTSTR(a, b, c)			\
			dlgs_setliststr((a), (b), (c), sizeof((c))/sizeof(OEMCHAR *))

#define	SETnLISTSTR(a, b, c, d)		\
			dlgs_setliststr((a), (b), (c), (d))

#define	SETLISTUINT32(a, b, c)		\
			dlgs_setlistuint32((a), (b), (c), sizeof((c))/sizeof(UINT32))


BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
											OEMCHAR *path, UINT size, int *ro);
BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
													OEMCHAR *path, UINT size);
BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
													OEMCHAR *path, UINT size);

void dlgs_browsemimpidef(HWND hWnd, UINT16 res);

void dlgs_setliststr(HWND hWnd, UINT16 res, const OEMCHAR **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, UINT16 res, const UINT32 *item, UINT items);

void dlgs_setlistmidiout(HWND hWnd, UINT16 res, const OEMCHAR *defname);
void dlgs_setlistmidiin(HWND hWnd, UINT16 res, const OEMCHAR *defname);

void dlgs_drawbmp(HDC hdc, UINT8 *bmp);

