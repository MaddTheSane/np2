
typedef union {
	struct {
		BYTE	pl;
		BYTE	bl;
		BYTE	cl;
		BYTE	ssl;
		BYTE	sur;
		BYTE	sdr;
	} reg;
	BYTE	b[6];
} _CRTC, *CRTC;

typedef union {
	BYTE	b[2];
	UINT16	w;
} PAIR16;

typedef struct {
	UINT32	counter;
	UINT16	mode;
	BYTE	modereg;
	BYTE	padding;
	PAIR16	tile[4];
	UINT32	gdcwithgrcg;
	BYTE	chip;
} _GRCG, *GRCG;


#ifdef __cplusplus
extern "C" {
#endif

void crtc_reset(void);
void crtc_bind(void);

#ifdef __cplusplus
}
#endif

