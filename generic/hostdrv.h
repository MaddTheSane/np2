
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

typedef struct {
	long	hdl;
	UINT32	mode;
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

#if 0
typedef struct {
	char		filename[MAX_PATH];
	UINT32		mode;
} HOSTDRVFILE;
#endif

extern	HOSTDRV		hostdrv;
// extern	HOSTDRVFILE	hostdrv_files[];





#if 0
#pragma pack(1)

typedef union {
	LPVOID		ptr;
	DWORD		addr;
	struct {
		WORD	offset;
		WORD	seg;
	};
} PTR;

typedef struct {
	BYTE		drive_no;		// 作成したドライブ番号(A:=0)
	struct {
		BYTE		major;
		BYTE		minor;
	}			dos_ver;		// DOSのバージョン
	PTR			sda_ptr;		// DOS上のSDAのアドレス
} IF4DOS;

typedef struct {
	IF4DOS		if4dos;
	int			find_drive_no;
	BOOL		is_mount;
} HOSTDRV;

typedef struct {
	char		filename[MAX_PATH];
	DWORD		mode;
} HOSTDRVFILE;

#pragma pack()

extern	HOSTDRV		hostdrv;
extern	HOSTDRVFILE	hostdrv_files[];
#endif


void hostdrv_initialize(void);
void hostdrv_deinitialize(void);
void hostdrv_reset(void);
// void save_hostdrv(void);
// void load_hostdrv(void);

BOOL hostdrv_mount(void);
void hostdrv_unmount(void);
void hostdrv_intr(void);

