
#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

typedef struct {
const char	*title;
const char	*ext;
const char	*filter;
	int		defindex;
} FILESEL;

extern const char str_int0[];
extern const char str_int1[];
extern const char str_int2[];
extern const char str_int4[];
extern const char str_int5[];
extern const char str_int6[];

#define	SetDlgItemCheck(a, b, c)	\
			SendDlgItemMessage((a), (b), BM_SETCHECK, (c), 0)

#define	GetDlgItemCheck(a, b)		\
			(((int)SendDlgItemMessage((a), (b), BM_GETCHECK, 0, 0))?1:0)

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define	SETLISTSTR(a, b, c)			\
			dlgs_setliststr((a), (b), (c), sizeof((c))/sizeof(char *))

#define	SETnLISTSTR(a, b, c, d)		\
			dlgs_setliststr((a), (b), (c), (d))

#define	SETLISTUINT32(a, b, c)		\
			dlgs_setlistuint32((a), (b), (c), sizeof((c))/sizeof(UINT32))


BOOL dlgs_selectfile(HWND hWnd, const FILESEL *item,
											char *path, UINT size, int *ro);
BOOL dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
													char *path, UINT size);
BOOL dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
													char *path, UINT size);

void dlgs_browsemimpidef(HWND hWnd, UINT16 res);

void dlgs_setliststr(HWND hWnd, UINT16 res, const char **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, UINT16 res, const UINT32 *item, UINT items);

void dlgs_setlistmidiout(HWND hWnd, UINT16 res, const char *defname);
void dlgs_setlistmidiin(HWND hWnd, UINT16 res, const char *defname);

void dlgs_drawbmp(HDC hdc, UINT8 *bmp);

