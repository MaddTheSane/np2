#define		DCLOCK_X	56
#define		DCLOCK_Y	12

typedef struct {
	BYTE	dclock_flm[8];
	BYTE	dclock_now[8];
	BYTE	dclock_bak[8];
	WORD	dclock_drawing;
} DCLOCK_T;


typedef struct {
	BYTE	*dclock_pos;
	WORD	dclock_mask;
	BYTE	dclock_rolbit;
	BYTE	reserved;
} DCLOCK_POS;


typedef struct {
	BYTE		*dclock_fnt;
	DCLOCK_POS	*dclock_put;
	void		(*init)(void);
} DCLOCK_FNT;


#ifdef __cplusplus
extern "C" {
#endif

extern RGB32 dclock_pal[4];

void dclock_init(void);
void dclock_init8(void);
void dclock_init16(void);
void dclock_reset(void);
void dclock_callback(void);
void dclock_redraw(void);
BOOL dclock_disp(void);
void __fastcall dclock_cntdown(BYTE value);
void dclock_make(void);
void __fastcall dclock_out8(void *ptr, DWORD width);
void __fastcall dclock_out16(void *ptr, DWORD width);

#ifdef __cplusplus
}
#endif

