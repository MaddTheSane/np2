
enum {
	HDDTYPE_SASI,
	HDDTYPE_SCSI,
	HDDTYPE_NONE
};

typedef struct {
	long	totals;
	UINT16	tracks;
	UINT16	size;
	BYTE	sectors;
	BYTE	surfaces;
	UINT16	type;
	UINT32	headersize;
	long	fh;
	char	fname[MAX_PATH];
} _SXSIHDD, *SXSIHDD;


#ifdef __cplusplus
extern "C" {
#endif

extern	_SXSIHDD	sxsi_hd[4];

void sxsi_initialize(void);

SXSIHDD sxsi_getptr(BYTE drv);
const char *sxsi_getname(BYTE drv);
BOOL sxsi_hddopen(BYTE drv, const char *file);

void sxsi_open(void);
void sxsi_flash(void);
void sxsi_trash(void);

BYTE sxsi_read(BYTE drv, long pos, BYTE *buf, UINT16 size);
BYTE sxsi_write(BYTE drv, long pos, const BYTE *buf, UINT16 size);
BYTE sxsi_format(BYTE drv, long pos);

#ifdef __cplusplus
}
#endif

