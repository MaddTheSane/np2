
#ifdef __cplusplus
extern "C" {
#endif

BOOL profile_enum(const char *filename, void *arg,
							BOOL (*proc)(void *arg, const char *para,
									const char *key, const char *data));
const char *profile_getarg(const char *str, char *buf, UINT leng);




// ---- ‚Ü‚¾ƒeƒXƒg

enum {
	PFILEH_READONLY		= 0x0001,
	PFILEH_MODIFY		= 0x0002
};

typedef struct {
	char	*buffer;
	UINT	buffers;
	UINT	size;
	UINT	flag;
	char	path[MAX_PATH];
} _PFILEH, *PFILEH;

PFILEH profile_open(const char *filename, UINT flag);
void profile_close(PFILEH hdl);
BOOL profile_read(const char *app, const char *key, const char *def,
										char *ret, UINT size, PFILEH hdl);
BOOL profile_write(const char *app, const char *key,
											const char *data, PFILEH hdl);


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
	PFTYPE_BYTE3,
	PFTYPE_USER,
	PFITYPE_MASK		= 0xff,

	PFFLAG_RO			= 0x0100,
	PFFLAG_MAX			= 0x0200,
	PFFLAG_AND			= 0x0400
};

typedef struct {
	char	item[10];
	UINT16	itemtype;
	void	*value;
	UINT32	arg;
} PFTBL;

typedef void (*PFREAD)(const PFTBL *item, const char *string);
typedef char *(*PFWRITE)(const PFTBL *item, char *string, UINT size);

void profile_iniread(const char *path, const char *app,
								const PFTBL *tbl, UINT count, PFREAD cb);
void profile_iniwrite(const char *path, const char *app,
								const PFTBL *tbl, UINT count, PFWRITE cb);

#ifdef __cplusplus
}
#endif

