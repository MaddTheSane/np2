
typedef union {
	BYTE	b[2];
	UINT16	w;
} WORD_P;

typedef union {
	BYTE	b[4][2];
	UINT16	w[4];
	UINT32	d[2];
} QWORD_P;

typedef struct {
	UINT16		access;
	UINT16		fgbg;
	UINT16		ope;
	UINT16		fg;
	WORD_P		mask;
	UINT16		bg;
	UINT16		sft;
	UINT16		leng;
	QWORD_P		lastvram;
	QWORD_P		patreg;
	QWORD_P		fgc;
	QWORD_P		bgc;

	int			func;
	UINT		remain;
	UINT		stack;
	UINT		padding_b[4];
	BYTE		buf[4096/8 + 4*4];
	UINT		padding_a[4];
	BYTE		*inptr;
	BYTE		*outptr;
	WORD_P		mask2;
	WORD_P		srcmask;
	BYTE		srcbit;
	BYTE		dstbit;
	BYTE		sft8bitl;
	BYTE		sft8bitr;
} _EGC, *EGC;


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 maskword[16][2];

void egc_reset(void);
void egc_bind(void);
void IOOUTCALL egc_w16(UINT port, UINT16 value);

#ifdef __cplusplus
}
#endif

