
#define	IDEIO_MULTIPLE_MAX	0

typedef struct {
	UINT8	sxsidrv;
	UINT8	wp;
	UINT8	dr;
	UINT8	hd;
	UINT8	sc;
	UINT8	sn;
	UINT16	cy;

	UINT8	cmd;
	UINT8	status;
	UINT8	error;
	UINT8	ctrl;

	UINT8	device;
	UINT8	surfaces;
	UINT8	sectors;
	UINT8	bufdir;

	UINT8	mulcnt;
	UINT8	multhr;
	UINT8	mulmode;
	UINT8	dmy;

	UINT	bufpos;
	UINT	bufsize;
	UINT8	buf[512];
} _IDEDRV, *IDEDRV;

typedef struct {
	_IDEDRV	drv[2];
	UINT	drivesel;
} _IDEDEV, *IDEDEV;

typedef struct {
	UINT8	bank[2];
	UINT8	padding[2];
	_IDEDEV	dev[2];
} IDEIO;


enum {
	IDE_IRQ				= 0x09,

	IDETYPE_NONE		= 0,
	IDETYPE_HDD			= 1,
	IDETYPE_CDROM		= 2,

	IDEDIR_NONE			= 0,
	IDEDIR_OUT			= 1,
	IDEDIR_IN			= 2
};

// error
//	bit7: Bad Block detected
//  bit6: Data ECC error
//  bit4: ID Not Found
//  bit2: Aborted command
//  bit1: Track0 Error
//  bit0: Address Mark Not Found

enum {
	IDEERR_BBK			= 0x80,
	IDEERR_UNC			= 0x40,
	IDEERR_IDNF			= 0x10,
	IDEERR_ABRT			= 0x04,
	IDEERR_TR0			= 0x02,
	IDEERR_AMNF			= 0x01
};

enum {
	IDEINTR_IO			= 0x02,
	IDEINTR_CD			= 0x01
};

// status
//	bit7: Busy
//  bit6: Drive Ready
//  bit5: Drive Write Fault
//  bit4: Drive Seek Complete
//  bit3: Data Request
//  bit2: Corrected data
//  bit1: Index
//  bit0: Error

enum {
	IDESTAT_BSY			= 0x80,
	IDESTAT_DRDY		= 0x40,
	IDESTAT_DWF			= 0x20,
	IDESTAT_DSC			= 0x10,
	IDESTAT_DRQ			= 0x08,
	IDESTAT_CORR		= 0x04,
	IDESTAT_INDX		= 0x02,
	IDESTAT_ERR			= 0x01
};

enum {
	IDEDEV_LBA			= 0x40,
	IDEDEV_DEV			= 0x10
};

// control
//	bit2: Software Reset
//  bit1: ~Interrupt Enable

enum {
	IDECTRL_SRST		= 0x04,
	IDECTRL_NIEN		= 0x02
};



#ifdef __cplusplus
extern "C" {
#endif

extern	IDEIO	ideio;

void IOOUTCALL ideio_w16(UINT port, REG16 value);
REG16 IOINPCALL ideio_r16(UINT port);

void ideio_reset(void);
void ideio_bind(void);

#ifdef __cplusplus
}
#endif

