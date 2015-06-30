
#if defined(SUPPORT_HOSTDRV)

#define	DIRMAX_DEPTH		8

enum {
	HDFMODE_READ		= 0x01,
	HDFMODE_WRITE		= 0x02,
	HDFMODE_DELETE		= 0x04
};

typedef struct {
	INTPTR	hdl;
	UINT	mode;
	OEMCHAR	path[MAX_PATH];
} _HDRVFILE, *HDRVFILE;

typedef struct {
	struct {
		UINT8	is_mount;
		UINT8	drive_no;
		UINT8	dosver_major;
		UINT8	dosver_minor;
		UINT16	sda_off;
		UINT16	sda_seg;
		UINT	flistpos;
	}			stat;

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

void hostdrv_mount(const void *arg1, long arg2);
void hostdrv_unmount(const void *arg1, long arg2);
void hostdrv_intr(const void *arg1, long arg2);

int hostdrv_sfsave(STFLAGH sfh, const SFENTRY *tbl);
int hostdrv_sfload(STFLAGH sfh, const SFENTRY *tbl);

#ifdef __cplusplus
}
#endif

#endif

