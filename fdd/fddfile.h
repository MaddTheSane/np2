
#define	MAX_FDDFILE		4							// ver0.31

#include	"d88head.h"

enum {
	FDDFILE_MF			= 0x80,
	FDDFILE_FM			= 0x20,

	FDDFILE_AM			= 0x08,
	FDDFILE_DDAM		= 0x02,

	FDDFILE_NONE		= 0x00,
	FDDCHECK_FM			= (FDDFILE_MF | FDDFILE_FM),
	FDDCHECK_MFM		= FDDFILE_MF,
	FDDCHECK_DAM		= FDDFILE_AM,
	FDDCHECK_DDAM		= (FDDFILE_AM | FDDFILE_DDAM),

	FDDWRITE_FM			= 0x40,
	FDDWRITE_DDAM		= 0x04
};

enum {
	DISKTYPE_NOTREADY	= 0,
	DISKTYPE_BETA,
	DISKTYPE_D88
};

enum {
	DISKTYPE_2D			= 0,
	DISKTYPE_2DD,
	DISKTYPE_2HD
};

typedef struct {
	BYTE	tracks;
	BYTE	sectors;
	BYTE	n;
	BYTE	meida;
	BYTE	rpm;
} _XDFINFO, *XDFINFO;

typedef struct {
	UINT		fdtype_major;
	UINT		fdtype_minor;
	UINT32		fd_size;
	UINT32		ptr[164];
	_D88HEAD	head;
} _D88INFO, *D88INFO;

typedef struct {
	char	fname[MAX_PATH];
	BYTE	type;
	BYTE	num;
	BYTE	protect;
	union {
		_XDFINFO	xdf;
		_D88INFO	d88;
	} inf;
} _FDDFILE, *FDDFILE;


#ifdef __cplusplus
extern "C" {
#endif

extern	_FDDFILE	fddfile[MAX_FDDFILE];
extern	BYTE		fddlasterror;

// ãNìÆéûÇ…àÍâÒÇæÇØèâä˙âª
void fddfile_init(void);

void fddfile_reset2dmode(void);

const char *fdd_diskname(BYTE drv);
BOOL fdd_diskready(BYTE drv);
BOOL fdd_diskprotect(BYTE drv);

BOOL fdd_set(BYTE drv, const char *fname, UINT ftype, int ro);
BOOL fdd_eject(BYTE drv);

BOOL fdd_diskaccess(void);
BOOL fdd_seek(void);
BOOL fdd_seeksector(void);
BOOL fdd_read(void);
BOOL fdd_write(void);
BOOL fdd_diagread(void);
BOOL fdd_readid(void);
BOOL fdd_writeid(void);

BOOL fdd_formatinit(void);
BOOL fdd_formating(const BYTE *ID);
BOOL fdd_isformating(void);

#ifdef __cplusplus
}
#endif

