
struct _pathlist;
typedef	struct _pathlist	_PATHLIST;
typedef	struct _pathlist	*PATHLIST;

struct _pathlist {
	PATHLIST	next;
	char		path[MAX_PATH];
};

enum {
	TONECFG_EXIST		= 0x01,
	TONECFG_NOLOOP		= 0x02,
	TONECFG_NOENV		= 0x04,
	TONECFG_KEEPENV		= 0x08,
	TONECFG_NOTAIL		= 0x10,

	TONECFG_AUTOAMP		= -1,
	TONECFG_VARIABLE	= 0xff
};

typedef struct {
	char	*name;
	int		amp;
	BYTE	flag;
	BYTE	pan;
	BYTE	note;
} _TONECFG, *TONECFG;


#ifdef __cplusplus
extern "C" {
#endif

BOOL cfgfile_getfile(MIDIMOD mod, const char *filename,
													char *path, int size);
BOOL cfgfile_load(MIDIMOD mod, const char *filename, int depth);

#ifdef __cplusplus
}
#endif

