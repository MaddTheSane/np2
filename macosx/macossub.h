
enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

typedef struct {
	char	path[MAX_PATH];
	UINT32	size;
	UINT32	attr;
} FLDATA;

#ifdef __cplusplus
extern "C" {
#endif

void macossub_init(void);
void macossub_term(void);

UINT32 macos_gettick(void);

void mkstr255(Str255 dst, const char *src);
void mkcstr(char *dst, int size, const Str255 src);

void *file_list1st(const char *dir, FLDATA *fl);
BOOL file_listnext(void *hdl, FLDATA *fl);
void file_listclose(void *hdl);

#ifdef __cplusplus
}
#endif

