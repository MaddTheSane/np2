
enum {
	SASIHDD_MAX		= 2,
	SCSIHDD_MAX		= 4
};

enum {
	SXSITYPE_NONE		= 0x0000,

	SXSITYPE_HDD		= 0x0001,
	SXSITYPE_CDROM		= 0x0002,
	SXSITYPE_MO			= 0x0003,
	SXSITYPE_SCANNER	= 0x0004,
	SXSITYPE_DEVMASK	= 0x000f,

	SXSITYPE_REMOVE		= 0x0010,

	SXSITYPE_SASIMASK	= 0x0700,

	SXSITYPE_SASI		= 0x1000,
	SXSITYPE_IDE		= 0x2000,
	SXSITYPE_SCSI		= 0x3000,
	SXSITYPE_IFMASK		= 0xf000
};


typedef struct {
	UINT8	sectors;
	UINT8	surfaces;
	UINT16	cylinders;
} SASIHDD;

typedef struct {
	BYTE	cylinders[2];
} THDHDR;

typedef struct {
	char	sig[16];
	char	comment[0x100];
	BYTE	headersize[4];
	BYTE	cylinders[4];
	BYTE	surfaces[2];
	BYTE	sectors[2];
	BYTE	sectorsize[2];
	BYTE	reserved[0xe2];
} NHDHDR;

typedef struct {
	BYTE	dummy[4];
	BYTE	hddtype[4];
	BYTE	headersize[4];
	BYTE	hddsize[4];
	BYTE	sectorsize[4];
	BYTE	sectors[4];
	BYTE	surfaces[4];
	BYTE	cylinders[4];
} HDIHDR;

typedef struct {
	char	sig[3];
	char	ver[4];
	char	delimita;
	char	comment[128];
	BYTE	padding1[4];
	BYTE	mbsize[2];
	BYTE	sectorsize[2];
	BYTE	sectors;
	BYTE	surfaces;
	BYTE	cylinders[2];
	BYTE	totals[4];
	BYTE	padding2[0x44];
} VHDHDR;

typedef struct {
	long	totals;
	UINT16	cylinders;
	UINT16	size;
	UINT8	sectors;
	UINT8	surfaces;
	UINT16	type;
	UINT32	headersize;
	long	fh;
	char	fname[MAX_PATH];
} _SXSIDEV, *SXSIDEV;


#ifdef __cplusplus
extern "C" {
#endif

extern const char sig_vhd[8];
extern const char sig_nhd[15];
extern const SASIHDD sasihdd[7];

void sxsi_initialize(void);

SXSIDEV sxsi_getptr(REG8 drv);
const char *sxsi_getname(REG8 drv);
BOOL sxsi_hddopen(REG8 drv, const char *file);

void sxsi_open(void);
void sxsi_flash(void);
void sxsi_trash(void);

BOOL sxsi_issasi(void);
BOOL sxsi_isscsi(void);
BOOL sxsi_iside(void);

REG8 sxsi_read(REG8 drv, long pos, BYTE *buf, UINT size);
REG8 sxsi_write(REG8 drv, long pos, const BYTE *buf, UINT size);
REG8 sxsi_format(REG8 drv, long pos);

#ifdef __cplusplus
}
#endif

