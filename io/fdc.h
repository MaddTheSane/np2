
enum {
	FDCEVENT_NEUTRAL,
	FDCEVENT_CMDRECV,
	FDCEVENT_DATSEND,
	FDCEVENT_BUFRECV,
	FDCEVENT_BUFSEND,
	FDCEVENT_BUFSEND2,
	FDCEVENT_NEXTDATA,
	FDCEVENT_BUSY
};

enum {
	LENGTH_GAP0		= 80,
	LENGTH_SYNC		= 12,
	LENGTH_IM		= 4,
	LENGTH_GAP1		= 50,
	LENGTH_IAM		= 4,
	LENGTH_ID		= 4,
	LENGTH_CRC		= 2,
	LENGTH_GAP2		= 22,
	LENGTH_DAM		= 4,

	LENGTH_PRIAMP	= LENGTH_GAP0 + LENGTH_SYNC + LENGTH_IM + LENGTH_GAP1,
	LENGTH_SECTOR	= LENGTH_SYNC + LENGTH_IAM + LENGTH_ID + LENGTH_CRC +
						LENGTH_GAP2 + LENGTH_SYNC + LENGTH_DAM + LENGTH_CRC,

	LENGTH_TRACK	= 10600							// normal: (+-2%)
};

// real sector size = LENGTH_SECTOR + (128 << n) + gap3

enum {
	FDCRLT_NR		= 0x000008,
	FDCRLT_EC		= 0x000010,
	FDCRLT_SE		= 0x000020,
	FDCRLT_IC0		= 0x000040,
	FDCRLT_IC1		= 0x000080,

	FDCRLT_MA		= 0x000100,
	FDCRLT_NW		= 0x000200,
	FDCRLT_ND		= 0x000400,
	FDCRLT_OR		= 0x001000,
	FDCRLT_DE		= 0x002000,
	FDCRLT_EN		= 0x008000,

	FDCRLT_MD		= 0x010000,
	FDCRLT_BC		= 0x020000,
	FDCRLT_SN		= 0x040000,
	FDCRLT_SH		= 0x080000,
	FDCRLT_NC		= 0x100000,
	FDCRLT_DD		= 0x200000,
	FDCRLT_CM		= 0x400000,

	FDCRLT_EXT		= 0x80000000,

	FDCSTAT_CB		= 0x10,
	FDCSTAT_NDM		= 0x20,
	FDCSTAT_DIO		= 0x40,
	FDCSTAT_RQM		= 0x80
};

typedef struct {
	BYTE	us, hd;
	BYTE	mt, mf, sk;
	BYTE	eot, gpl, dtl;
	BYTE	C, H, R, N;
	BYTE	srt, hut, hlt, nd;
	BYTE	stp, ncn, sc, d;

	BYTE	status;
	BYTE	intreq;
	BYTE	lastdata;
	BYTE	tc;

	UINT32	stat[4];
	BYTE	treg[4];

	int		event;
	int		cmdp;
	int		cmdcnt;
	int		datp;
	int		datcnt;
	int		bufp;
	int		bufcnt;

	BYTE	cmd;
	BYTE	cmds[15];
	BYTE	data[16];
	BYTE	buf[0x8000];

	BYTE	ctrlfd;
	BYTE	crcn;

	BYTE	ctrlreg;
	BYTE	busy;

	BYTE	chgreg;
	BYTE	rpm;													// 1.44
} _FDC, *FDC;


#ifdef __cplusplus
extern "C" {
#endif

#define	CTRL_FDMEDIA	fdc.ctrlfd

void fdc_reset(void);
void fdc_bind(void);
void fdc_interrupt(void);

void DMACCALL fdc_DataRegWrite(BYTE data);
BYTE DMACCALL fdc_DataRegRead(void);
BYTE DMACCALL fdc_dmafunc(BYTE func);

void fdcsend_error7(void);
void fdcsend_success7(void);

void fdcbusy_error7(NEVENTITEM item);

#ifdef __cplusplus
}
#endif

