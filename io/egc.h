
typedef union {
	BYTE	_b[2];
	UINT16	w;
} EGCWORD;

typedef union {
	BYTE	_b[4][2];
	UINT16	w[4];
	UINT32	d[2];
} EGCQUAD;

typedef struct {
	UINT16	access;
	UINT16	fgbg;
	UINT16	ope;
	UINT16	fg;
	EGCWORD	mask;
	UINT16	bg;
	UINT16	sft;
	UINT16	leng;
	EGCQUAD	lastvram;
	EGCQUAD	patreg;
	EGCQUAD	fgc;
	EGCQUAD	bgc;

	int		func;
	UINT	remain;
	UINT	stack;
	UINT	padding_b[4];
	BYTE	buf[4096/8 + 4*4];
	UINT	padding_a[4];
	BYTE	*inptr;
	BYTE	*outptr;
	EGCWORD	mask2;
	EGCWORD	srcmask;
	BYTE	srcbit;
	BYTE	dstbit;
	BYTE	sft8bitl;
	BYTE	sft8bitr;
} _EGC, *EGC;


#ifdef __cplusplus
extern "C" {
#endif

void egc_reset(void);
void egc_bind(void);
void IOOUTCALL egc_w16(UINT port, UINT16 value);

#ifdef __cplusplus
}
#endif

