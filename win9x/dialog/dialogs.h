
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


const char *dlgs_selectfile(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size, int *ro);
const char *dlgs_selectwritefile(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size);
const char *dlgs_selectwritenum(HWND hWnd, const FILESEL *item,
					const char *defname, char *folder, UINT size);

void dlgs_browsemimpidef(HWND hWnd, WORD res);

void dlgs_setliststr(HWND hWnd, WORD res, const char **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, WORD res, const UINT32 *item, UINT items);

void dlgs_setlistmidiout(HWND hWnd, WORD res, const char *defname);
void dlgs_setlistmidiin(HWND hWnd, WORD res, const char *defname);

void dlgs_linex(BYTE *image, int x, int y, int l, int align, BYTE c);
void dlgs_liney(BYTE *image, int x, int y, int l, int align, BYTE c);

void dlgs_setjumperx(BYTE *image, int x, int y, int align);
void dlgs_setjumpery(BYTE *image, int x, int y, int align);

