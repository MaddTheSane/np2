
// ‚Ë‚±ê—pƒwƒbƒ_

enum {
	MPU98_MAXTIMEBASE	= (192 / 24),
	MPU98_EXCVBUFS		= 512,
	MPU98_RECVBUFS		= (1 << 7)
};

#define	MPUCHDATAS	4

typedef struct {
	UINT8	step;
	UINT8	datas;
	UINT8	remain;
	UINT8	rstat;
	UINT8	recv;
	UINT8	padding[2];
	BYTE	data[MPUCHDATAS];
} MPUCH;

typedef struct {
	UINT8	step;
	UINT8	cmd;
	UINT8	padding[2];
	UINT16	remain;
	UINT16	datas;
	BYTE	data[MPU98_EXCVBUFS];
} MPUF9CH;

typedef struct {
	int		cnt;
	int		pos;
	BYTE	buf[MPU98_RECVBUFS];
} MPURECV;

typedef struct {
	UINT16	port;
	UINT8	irqnum;
	UINT8	data;

	UINT32	xferclock;
	SINT32	stepclock;

	UINT8	intch;
	UINT8	intreq;
	UINT8	fd_remain;
	UINT8	fd_cnt;
	UINT8	fd_step[4];

	UINT8	status;
	UINT8	mode;
	UINT8	cmd;
	UINT8	timebase;

	UINT8	tempo;
	UINT8	tempos;
	UINT8	recvevent;
	UINT8	remainstep;

	UINT8	timing;
	UINT8	avail;
	UINT8	bar;
	UINT8	flag1;

	MPURECV	r;

	MPUCH	ch[8];
	MPUF9CH	f9;
} _MPU98II, *MPU98II;


#ifdef __cplusplus
extern "C" {
#endif

extern _MPU98II mpu98;

void midiint(NEVENTITEM item);
void midiwaitout(NEVENTITEM item);

void mpu98ii_construct(void);
void mpu98ii_destruct(void);

void mpu98ii_reset(void);
void mpu98ii_bind(void);

void mpu98ii_callback(void);
void mpu98ii_midipanic(void);

#ifdef __cplusplus
}
#endif

