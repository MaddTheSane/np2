
#if defined(SUPPORT_SCSI)
enum {
	SASIHDD_MAX		= 4,
	SCSIHDD_MAX		= 8
};
#else
enum {
	SASIHDD_MAX		= 4,
	SCSIHDD_MAX		= 0
};
#endif

enum {
	SXSIDRV_UNITMASK	= 0x0f,
	SXSIDRV_SASI		= 0x00,
	SXSIDRV_SCSI		= 0x20,
	SXSIDRV_IFMASK		= 0x20,

	SXSIDEV_NC			= 0x00,
	SXSIDEV_HDD			= 0x01,
	SXSIDEV_CDROM		= 0x02,
	SXSIDEV_MO			= 0x03,
	SXSIDEV_SCANNER		= 0x04,

	SXSIFLAG_READY		= 0x01
};


struct _sxsidev;
typedef struct _sxsidev		_SXSIDEV;
typedef struct _sxsidev		*SXSIDEV;

#include	"sxsihdd.h"
#include	"sxsicd.h"

struct _sxsidev {
	UINT8	drv;
	UINT8	devtype;
	UINT8	flag;
	UINT8	__caps;

	INTPTR	fh;
	REG8	(*read)(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size);
	REG8	(*write)(SXSIDEV sxsi, long pos, const UINT8 *buf, UINT size);
	REG8	(*format)(SXSIDEV sxsi, long pos);

	long	totals;
	UINT16	cylinders;
	UINT16	size;
	UINT8	sectors;
	UINT8	surfaces;
	UINT8	mediatype;
	UINT8	padding;
	UINT32	headersize;

	OEMCHAR	fname[MAX_PATH];
	UINT	ftype;
};


#ifdef __cplusplus
extern "C" {
#endif

void sxsi_initialize(void);
void sxsi_allflash(void);
void sxsi_alltrash(void);
BOOL sxsi_isconnect(SXSIDEV sxsi);
BRESULT sxsi_prepare(SXSIDEV sxsi);

SXSIDEV sxsi_getptr(REG8 drv);
const OEMCHAR *sxsi_getfilename(REG8 drv);
BRESULT sxsi_setdevtype(REG8 drv, UINT8 dev);
UINT8 sxsi_getdevtype(REG8 drv);
BRESULT sxsi_devopen(REG8 drv, const OEMCHAR *file);
void sxsi_devclose(REG8 drv);
REG8 sxsi_read(REG8 drv, long pos, UINT8 *buf, UINT size);
REG8 sxsi_write(REG8 drv, long pos, const UINT8 *buf, UINT size);
REG8 sxsi_format(REG8 drv, long pos);

BOOL sxsi_issasi(void);
BOOL sxsi_isscsi(void);
BOOL sxsi_iside(void);

#ifdef __cplusplus
}
#endif

