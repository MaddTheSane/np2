
// ‚Ë‚±ê—pƒwƒbƒ_

enum {
	MPU98_MAXTIMEBASE	= (192 / 24),
	MPU98_EXCVBUFS		= 512,
	MPU98_RECVBUFS		= (1 << 7)
};

#define	MPUCHDATAS	4

typedef struct {
	BYTE	step;
	BYTE	datas;
	BYTE	remain;
	BYTE	rstat;
	BYTE	recv;
	BYTE	padding[2];
	BYTE	data[MPUCHDATAS];
} MPUCH;

typedef struct {
	BYTE	step;
	BYTE	cmd;
	BYTE	padding[2];
	UINT16	remain;
	UINT16	datas;
	BYTE	data[MPU98_EXCVBUFS];
} MPUF9CH;

typedef struct {
	MPUCH	ch[8];
	MPUF9CH	f9;

	BYTE	buf[MPU98_RECVBUFS];

	SINT32	clock;
	int		cnt;
	int		pos;

	UINT16	port;												// ver0.28
	BYTE	irqnum;
	BYTE	data;

	BYTE	intch;
	BYTE	intreq;

	BYTE	fd_remain;
	BYTE	fd_cnt;
	BYTE	fd_step[4];

	BYTE	status;
	BYTE	mode;
	BYTE	cmd;
	BYTE	timebase;
	BYTE	tempo;
	BYTE	tempos;
	BYTE	recvevent;
	BYTE	remainstep;
	BYTE	timing;
	BYTE	avail;
	BYTE	bar;

	BYTE	flag1;
} _MPU98II, *MPU98II;


#ifdef __cplusplus
extern "C" {
#endif

extern _MPU98II mpu98;										// ver0.28

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

