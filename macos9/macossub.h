
enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

#define		FLISTH				void *
#define		FLISTH_INVALID		((FLISTH)0)

typedef struct {
	char	path[MAX_PATH];
	UINT32	size;
	UINT32	attr;
} FLINFO;

#ifdef __cplusplus
extern "C" {
#endif

void macossub_init(void);
void macossub_term(void);

UINT32 macos_gettick(void);

void mkstr255(Str255 dst, const char *src);
void mkcstr(char *dst, int size, const Str255 src);

FLISTH file_list1st(const char *dir, FLINFO *fli);
BOOL file_listnext(FLISTH hdl, FLINFO *fli);
void file_listclose(FLISTH hdl);

#ifdef __cplusplus
}
#endif

