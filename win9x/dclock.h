
enum {
	DCLOCK_X		= 56,
	DCLOCK_Y		= 12
};

typedef struct {
	BYTE	*pos;
	UINT16	mask;
	BYTE	rolbit;
	BYTE	reserved;
} DCPOS;

typedef struct {
const BYTE	*fnt;
const DCPOS	*pos;
	BYTE	flm[8];
	BYTE	now[8];
	BYTE	bak[8];
	UINT16	drawing;
	BYTE	clk_x;
	BYTE	_padding;
	BYTE	dat[(DCLOCK_X * DCLOCK_Y / 8) + 4];
} _DCLOCK, *DCLOCK;

typedef struct {
	RGB32	pal32[4];
	RGB16	pal16[4];
	UINT32	pal8[4][16];
} DCLOCKPAL;


#ifdef __cplusplus
extern "C" {
#endif

extern	_DCLOCK		dclock;
extern	DCLOCKPAL	dclockpal;

void dclock_init(void);
void dclock_init8(void);
void dclock_init16(void);
void dclock_reset(void);
void dclock_callback(void);
void dclock_redraw(void);
BOOL dclock_disp(void);
void dclock_cntdown(BYTE value);
void dclock_make(void);
void dclock_out8(void *ptr, UINT width);
void dclock_out16(void *ptr, UINT width);

#ifdef __cplusplus
}
#endif

