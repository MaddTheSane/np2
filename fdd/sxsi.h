
#if defined(SUPPORT_SCSI)
enum {
	SASIHDD_MAX		= 2,
	SCSIHDD_MAX		= 4
};
#else
enum {
	SASIHDD_MAX		= 2,
	SCSIHDD_MAX		= 0
};
#endif

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
	UINT8	cylinders[2];
} THDHDR;

typedef struct {
	char	sig[16];
	char	comment[0x100];
	UINT8	headersize[4];
	UINT8	cylinders[4];
	UINT8	surfaces[2];
	UINT8	sectors[2];
	UINT8	sectorsize[2];
	UINT8	reserved[0xe2];
} NHDHDR;

typedef struct {
	UINT8	dummy[4];
	UINT8	hddtype[4];
	UINT8	headersize[4];
	UINT8	hddsize[4];
	UINT8	sectorsize[4];
	UINT8	sectors[4];
	UINT8	surfaces[4];
	UINT8	cylinders[4];
} HDIHDR;

typedef struct {
	char	sig[3];
	char	ver[4];
	char	delimita;
	char	comment[128];
	UINT8	padding1[4];
	UINT8	mbsize[2];
	UINT8	sectorsize[2];
	UINT8	sectors;
	UINT8	surfaces;
	UINT8	cylinders[2];
	UINT8	totals[4];
	UINT8	padding2[0x44];
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
	OEMCHAR	fname[MAX_PATH];
} _SXSIDEV, *SXSIDEV;


#ifdef __cplusplus
extern "C" {
#endif

extern const char sig_vhd[8];
extern const char sig_nhd[15];
extern const SASIHDD sasihdd[7];

void sxsi_initialize(void);

SXSIDEV sxsi_getptr(REG8 drv);
const OEMCHAR *sxsi_getname(REG8 drv);
BRESULT sxsi_hddopen(REG8 drv, const OEMCHAR *file);

void sxsi_open(void);
void sxsi_flash(void);
void sxsi_trash(void);

BOOL sxsi_issasi(void);
BOOL sxsi_isscsi(void);
BOOL sxsi_iside(void);

REG8 sxsi_read(REG8 drv, long pos, UINT8 *buf, UINT size);
REG8 sxsi_write(REG8 drv, long pos, const UINT8 *buf, UINT size);
REG8 sxsi_format(REG8 drv, long pos);

#ifdef __cplusplus
}
#endif

