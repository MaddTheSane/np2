
#define	DIRMAX_DEPTH		8

typedef struct {
	char	fcbname[12];
	UINT32	size;
	UINT32	attr;
} HDRVDIR;

typedef struct {
	HDRVDIR	di;
	char	realname[MAX_PATH];
} _HDRVLST, *HDRVLST;

typedef struct {
	HDRVDIR	di;
	char	path[MAX_PATH];
} HDRVPATH;

enum {
	HDFMODE_READ		= 0x01,
	HDFMODE_WRITE		= 0x02,
	HDFMODE_DELETE		= 0x04
};

typedef struct {
	long	hdl;
	UINT	mode;
	char	path[MAX_PATH];
} _HDRVFILE, *HDRVFILE;

typedef struct {
	UINT8	is_mount;
	UINT8	drive_no;
	UINT8	dosver_major;
	UINT8	dosver_minor;
	UINT16	sda_off;
	UINT16	sda_seg;
	UINT	flistpos;

//	LISTARRAY	cache[DIRMAX_DEPTH];
	LISTARRAY	fhdl;
	LISTARRAY	flist;
} HOSTDRV;


#ifdef __cplusplus
extern "C" {
#endif

extern	HOSTDRV		hostdrv;

void hostdrv_initialize(void);
void hostdrv_deinitialize(void);
void hostdrv_reset(void);
// void save_hostdrv(void);
// void load_hostdrv(void);

BOOL hostdrv_mount(void);
void hostdrv_unmount(void);
void hostdrv_intr(void);

#ifdef __cplusplus
}
#endif

