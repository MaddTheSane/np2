#include	"compiler.h"
#include	"parts.h"
#include	"np2.h"
#include	"scrnmng.h"
#include	"timemng.h"
#include	"dclock.h"
#include	"scrndraw.h"
#include	"palettes.h"


	_DCLOCK		dclock;
	DCLOCKPAL	dclockpal;


// ------------------------------------------------------------------ font1

static const UINT8 clockchr1[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78,},
			{0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,},
			{0x78, 0xcc, 0xcc, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0xfc,},
			{0xfc, 0x18, 0x30, 0x70, 0x18, 0x0c, 0x0c, 0xcc, 0x78,},
			{0x18, 0x38, 0x78, 0xd8, 0xd8, 0xfc, 0x18, 0x18, 0x18,},
			{0xfc, 0xc0, 0xc0, 0xf8, 0x0c, 0x0c, 0x0c, 0x8c, 0x78,},
			{0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x78,},
			{0xfc, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x30, 0x30, 0x30,},
			{0x78, 0xcc, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0xcc, 0x78,},
			{0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70,}};

static const DCPOS dclockpos1[6] = {
						{&dclock.dat[0], (WORD)(~0x00fc), 0, 0},
						{&dclock.dat[0], (WORD)(~0xf801), 7, 0},
						{&dclock.dat[2], (WORD)(~0x801f), 3, 0},
						{&dclock.dat[3], (WORD)(~0x003f), 2, 0},
						{&dclock.dat[4], (WORD)(~0xf003), 6, 0},
						{&dclock.dat[5], (WORD)(~0xe007), 5, 0}};

static void resetfont1(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00008001;
		}
		else {
			pat = 0x30008001;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*4+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*5+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*9+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------ font2

// 5x9
static const UINT8 clockchr2[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x30, 0x48, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x10, 0x30, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,},
			{0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8,},
			{0x7c, 0x08, 0x10, 0x30, 0x10, 0x08, 0x08, 0x90, 0x60,},
			{0x20, 0x40, 0x40, 0x88, 0x88, 0x90, 0x78, 0x10, 0x20,},
			{0x3c, 0x20, 0x20, 0x70, 0x08, 0x08, 0x08, 0x90, 0x60,},
			{0x10, 0x10, 0x20, 0x70, 0x48, 0x88, 0x88, 0x90, 0x60,},
			{0x7c, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40,},
			{0x38, 0x44, 0x44, 0x48, 0x30, 0x48, 0x88, 0x88, 0x70,},
			{0x18, 0x24, 0x40, 0x44, 0x48, 0x38, 0x10, 0x20, 0x20,}};

static const DCPOS dclockpos2[6] = {
						{&dclock.dat[0], (WORD)(~0x00fc), 0, 0},
						{&dclock.dat[0], (WORD)(~0xf003), 6, 0},
						{&dclock.dat[2], (WORD)(~0x00fc), 0, 0},
						{&dclock.dat[2], (WORD)(~0xf003), 6, 0},
						{&dclock.dat[4], (WORD)(~0x00fc), 0, 0},
						{&dclock.dat[4], (WORD)(~0xf003), 6, 0}};

static void resetfont2(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000002;
		}
		else {
			pat = 0x00020002;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*4+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*5+1]) = pat;
		pat <<= 1;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*9+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------ font3

// 4x9
static const UINT8 clockchr3[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60,},
			{0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,},
			{0x60, 0x90, 0x90, 0x10, 0x20, 0x40, 0x40, 0x80, 0xf0,},
			{0x60, 0x90, 0x90, 0x10, 0x60, 0x10, 0x90, 0x90, 0x60,},
			{0x20, 0x60, 0x60, 0xa0, 0xa0, 0xa0, 0xf0, 0x20, 0x20,},
			{0xf0, 0x80, 0x80, 0xe0, 0x90, 0x10, 0x90, 0x90, 0x60,},
			{0x60, 0x90, 0x90, 0x80, 0xe0, 0x90, 0x90, 0x90, 0x60,},
			{0xf0, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,},
			{0x60, 0x90, 0x90, 0x90, 0x60, 0x90, 0x90, 0x90, 0x60,},
			{0x60, 0x90, 0x90, 0x90, 0x70, 0x10, 0x90, 0x90, 0x60,}};

static const DCPOS dclockpos3[6] = {
						{&dclock.dat[0], (WORD)(~0x00f0), 0, 0},
						{&dclock.dat[0], (WORD)(~0x8007), 5, 0},
						{&dclock.dat[1], (WORD)(~0xc003), 6, 0},
						{&dclock.dat[2], (WORD)(~0x001e), 3, 0},
						{&dclock.dat[3], (WORD)(~0x000f), 4, 0},
						{&dclock.dat[4], (WORD)(~0x0078), 1, 0}};

static void resetfont3(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000010;
		}
		else {
			pat = 0x00400010;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*4+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*5+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*9+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------ font4

// 5x8
static const UINT8 clockchr4[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70,},
			{0x00, 0x70, 0x88, 0x08, 0x08, 0x30, 0x40, 0x88, 0xf8,},
			{0x00, 0x70, 0x88, 0x08, 0x30, 0x08, 0x08, 0x08, 0xf0,},
			{0x00, 0x10, 0x30, 0x50, 0x50, 0x90, 0xf8, 0x10, 0x10,},
			{0x00, 0x38, 0x40, 0x60, 0x10, 0x08, 0x08, 0x08, 0xf0,},
			{0x00, 0x18, 0x20, 0x40, 0xb0, 0xc8, 0x88, 0x88, 0x70,},
			{0x00, 0x70, 0x88, 0x88, 0x10, 0x10, 0x10, 0x20, 0x20,},
			{0x00, 0x70, 0x88, 0x88, 0x70, 0x50, 0x88, 0x88, 0x70,},
			{0x00, 0x70, 0x88, 0x88, 0x88, 0x78, 0x10, 0x20, 0xc0,}};

static void resetfont4(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000004;
		}
		else {
			pat = 0x00040004;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*5+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*6+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*9+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------ font5

static const UINT8 clockchr5[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20,},
			{0x00, 0x00, 0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xf8,},
			{0x00, 0x00, 0xf8, 0x10, 0x20, 0x10, 0x08, 0x88, 0x70,},
			{0x00, 0x00, 0x30, 0x50, 0x50, 0x90, 0xf8, 0x10, 0x10,},
			{0x00, 0x00, 0xf8, 0x80, 0xf0, 0x08, 0x08, 0x88, 0x70,},
			{0x00, 0x00, 0x30, 0x40, 0xf0, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0xf8, 0x08, 0x10, 0x20, 0x20, 0x40, 0x40,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x78, 0x10, 0x60,}};

static const DCPOS dclockpos5[6] = {
						{&dclock.dat[0], (WORD)(~0x00f8), 0, 0},
						{&dclock.dat[0], (WORD)(~0xe003), 6, 0},
						{&dclock.dat[2], (WORD)(~0x007c), 1, 0},
						{&dclock.dat[2], (WORD)(~0xf001), 7, 0},
						{&dclock.dat[4], (WORD)(~0x003e), 2, 0},
						{&dclock.dat[5], (WORD)(~0x00f8), 0, 0}};

static void resetfont5(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000006;
		}
		else {
			pat = 0x00030006;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*6+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*7+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*9+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------ font6

// 4x6
static const UINT8 clockchr6[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x10, 0x20, 0x40, 0xf0,},
			{0x00, 0x00, 0x00, 0xf0, 0x20, 0x60, 0x10, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x40, 0x80, 0xa0, 0xa0, 0xf0, 0x20,},
			{0x00, 0x00, 0x00, 0xf0, 0x80, 0x60, 0x10, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x40, 0x80, 0xe0, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0xe0, 0x10, 0x10, 0x20, 0x20, 0x40,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x60, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x70, 0x20, 0x40,}};

static const DCPOS dclockpos6[6] = {
						{&dclock.dat[0], (WORD)(~0x00f0), 0, 0},
						{&dclock.dat[0], (WORD)(~0x8007), 5, 0},
						{&dclock.dat[1], (WORD)(~0x000f), 4, 0},
						{&dclock.dat[2], (WORD)(~0x0078), 1, 0},
						{&dclock.dat[3], (WORD)(~0x00f0), 0, 0},
						{&dclock.dat[3], (WORD)(~0x8007), 5, 0}};

static void resetfont6(void) {

	DWORD pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000020;
		}
		else {
			pat = 0x00000220;
		}
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*8+1]) = pat;
		*(DWORD *)(&dclock.dat[(DCLOCK_X/8)*10+1]) = pat;
	}
}


// ------------------------------------------------------------------------

typedef struct {
const UINT8	*fnt;
const DCPOS	*pos;
	void	(*init)(void);
} DCLOCKFNT;

static const DCLOCKFNT fonttype[] =
					{{clockchr1[0], dclockpos1, resetfont1},
					 {clockchr2[0], dclockpos2, resetfont2},
					 {clockchr3[0], dclockpos3, resetfont3},
					 {clockchr4[0], dclockpos2, resetfont4},
					 {clockchr5[0], dclockpos5, resetfont5},
					 {clockchr6[0], dclockpos6, resetfont6}};


// ------------------------------------------------------------------------

void dclock_init(void) {

	pal_makegrad(dclockpal.pal32, 4,
									np2oscfg.clk_color1, np2oscfg.clk_color2);
}

void dclock_init8(void) {

	UINT8	i, j;
	DWORD	work = 0;							// vc++4.2

	for (i=0; i<16; i++) {
		for (j=1; j<0x10; j<<=1) {
			work <<= 8;
			if (i & j) {
				work++;
			}
		}
		for (j=0; j<4; j++) {
			dclockpal.pal8[j][i] = work * (START_PALORG + j);
		}
	}
}

void dclock_init16(void) {

	UINT	i;

	for (i=0; i<4; i++) {
		dclockpal.pal16[i] = scrnmng_makepal16(dclockpal.pal32[i]);
	}
}

void dclock_reset(void) {

const DCLOCKFNT *fnt;

	ZeroMemory(&dclock, sizeof(dclock));

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			np2oscfg.clk_x = 4;
		}
		else if (np2oscfg.clk_x <= 6) {
			np2oscfg.clk_x = 6;
		}
		else {
			np2oscfg.clk_x = 0;
		}
	}
	if (np2oscfg.clk_fnt >= NELEMENTS(fonttype)) {
		np2oscfg.clk_fnt = 0;
	}

	dclock.clk_x = np2oscfg.clk_x;
	fnt = fonttype + np2oscfg.clk_fnt;
	dclock.fnt = fnt->fnt;
	dclock.pos = fnt->pos;
	fnt->init();
	dclock_callback();
	dclock_redraw();
}

void dclock_callback(void) {

	int			i;
	UINT8		count = 13;
	UINT8		buf[8];
	_SYSTIME	st;

	if ((scrnmng_isfullscreen()) && (dclock.clk_x)) {
		timemng_gettime(&st);
		buf[0] = (st.hour / 10) + 1;
		buf[1] = (st.hour % 10) + 1;
		buf[2] = (st.minute / 10) + 1;
		buf[3] = (st.minute % 10) + 1;
		if (dclock.clk_x > 4) {
			buf[4] = (st.second / 10) + 1;
			buf[5] = (st.second % 10) + 1;
		}
		for (i=dclock.clk_x; i--;) {
			if (dclock.now[i] != buf[i]) {
				dclock.now[i] = buf[i];
				dclock.flm[i] = count;
				dclock.drawing |= (1 << i);
				count += 4;
			}
		}
	}
}

void dclock_redraw(void) {

	dclock.drawing = 0x3f;
}

BOOL dclock_disp(void) {

	return((dclock.drawing != 0) ||
			(*(DWORD *)(dclock.flm + 0) != 0) ||
			(*(DWORD *)(dclock.flm + 4) != 0));
}





#if 0
LABEL void __fastcall dclock_cntdown(UINT8 value) {

	__asm {
				cmp		np2oscfg.clk_x, 0
				je		dclock_cdn
				cmp		dword ptr (dclock.dclock_flm), 0
				jne		dclock_cdn
				cmp		dword ptr (dclock.dclock_flm + 4), 0
				jne		dclock_cdn
				ret

dclock_cdn:		push	ecx
				xor		edx, edx
				cmp		cl, 0
				jne		dclock_cdnlp
				inc		cl
dclock_cdnlp:	movzx	eax, dclock[edx].dclock_flm
				cmp		eax, 0
				je		dclock_cdned
				cmp		eax, DCLOCKY_MAX
				jae		dclock_upabove
				mov		ch, dclocky[eax]
				sub		al, cl
				jc		dclock_set0
				cmp		ch, dclocky[eax]
				je		dclock_outflm
				jmp		dclock_setdraw

dclock_upabove:	sub		al, cl
				jc		dclock_set0
				cmp		al, DCLOCKY_MAX
				jae		dclock_outflm
				jmp		dclock_setdraw
dclock_set0:	xor		eax, eax
dclock_setdraw:	bts		dclock.dclock_drawing, dx
dclock_outflm:	mov		dclock[edx].dclock_flm, al
dclock_cdned:	inc		edx
				cmp		dl, np2oscfg.clk_x
				jc		dclock_cdnlp
				pop		ecx
				ret
	}
}

LABEL void dclock_make(void) {

	__asm {
				cmp		np2oscfg.clk_x, 0
				je		dclockmakeend
				pushad
				xor		ebx, ebx
makedclock_lp:	btr		dclock.dclock_drawing, bx
				jc		makedclock_1
makedclock_ed:	inc		ebx
				cmp		bl, np2oscfg.clk_x
				jc		makedclock_lp
				popad
dclockmakeend:	ret

makedclock_1:	mov		eax, dclock_font.dclock_put
				lea		eax, [eax + ebx*8]
				mov		edi, [eax]DCLOCK_POS.dclock_pos
				mov		dx, [eax]DCLOCK_POS.dclock_mask
				mov		cl, [eax]DCLOCK_POS.dclock_rolbit
				movzx	eax, dclock[ebx].dclock_flm
				cmp		eax, 0
				je		makedclock_y0
				cmp		eax, DCLOCKY_MAX
				jb		makedclock_ani
				movzx	eax, dclock[ebx].dclock_bak
				jmp		makedclock0put

makedclock_y0:	movzx	eax, dclock[ebx].dclock_now
				mov		dclock[ebx].dclock_bak, al

makedclock0put:	mov		ch, 3
makedclock0_up:	and		[edi], dx
				add		edi, (DCLOCK_X / 8)
				dec		ch
				jne		makedclock0_up
				shl		eax, 4
				add		eax, dclock_font.dclock_fnt
				mov		esi, eax
				mov		ch, 9
makedclock0_dn:	movzx	eax, byte ptr [esi]
				ror		ax, cl
				and		[edi], dx
				or		[edi], ax
				inc		esi
				add		edi, (DCLOCK_X / 8)
				dec		ch
				jne		makedclock0_dn
				jmp		makedclock_ed

makedclock_ani:	movzx	eax, dclocky[eax]
				push	eax
				mov		ch, 3
				sub		ch, al
				je		makedclock_an2
makedclkani_up:	and		[edi], dx
				add		edi, (DCLOCK_X / 8)
				dec		ch
				jne		makedclkani_up
makedclock_an2:	movzx	esi, dclock[ebx].dclock_now
				shl		esi, 4
				add		esi, dclock_font.dclock_fnt
				mov		ch, 9
makedclkani_md:	movzx	eax, byte ptr [esi]
				ror		ax, cl
				and		[edi], dx
				or		[edi], ax
				inc		esi
				add		edi, (DCLOCK_X / 8)
				dec		ch
				jne		makedclkani_md
				pop		eax

				mov		ch, al
				movzx	esi, dclock[ebx].dclock_bak
				shl		esi, 4
				sub		esi, eax
				add		esi, 9
				add		esi, dclock_font.dclock_fnt
makedclkani_dn:	movzx	eax, byte ptr [esi]
				ror		ax, cl
				and		[edi], dx
				or		[edi], ax
				inc		esi
				add		edi, (DCLOCK_X / 8)
				dec		ch
				jne		makedclkani_dn
				jmp		makedclock_ed
	}
}

LABEL void __fastcall dclock_out8(void *ptr, DWORD width) {

	__asm {
				pushad
				mov		esi, offset dclock_dat
				mov		edi, offset outcolors
				mov		bh, 4
dclockout_lp1:	mov		bl, 3
dclockout_lp2:	mov		ebp, (DCLOCK_X/8)
dclockout_lp3:	movzx	eax, byte ptr [esi]
				shr		eax, 4
				mov		eax, [eax*4 + edi]
				mov		[ecx], eax
				movzx	eax, byte ptr [esi]
				and		eax, 15
				mov		eax, [eax*4 + edi]
				mov		[ecx+4], eax
				inc		esi
				add		ecx, 8
				dec		ebp
				jne		dclockout_lp3
				lea		ecx, [ecx + edx - DCLOCK_X]
				dec		bl
				jne		dclockout_lp2
				add		edi, 4*16
				dec		bh
				jne		dclockout_lp1
				popad
				ret
	}
}

LABEL void __fastcall dclock_out16(void *ptr, DWORD width) {

	__asm {
				pushad
				mov		esi, offset dclock_dat
				mov		edi, offset outcolors16
				mov		bh, 4
dclockout_lp1:	mov		bl, 3
dclockout_lp2:	push	ebx
				mov		ebp, (DCLOCK_X/8)
dclockout_lp3:	mov		bl, [esi]
				mov		bh, 8
dclockout_lp4:	rcl		bl, 1
				sbb		ax, ax
				and		ax, [edi]
				mov		[ecx], ax
				add		ecx, 2
				dec		bh
				jne		dclockout_lp4
				inc		esi
				dec		ebp
				jne		dclockout_lp3
				pop		ebx
				lea		ecx, [ecx + edx - DCLOCK_X*2]
				dec		bl
				jne		dclockout_lp2
				add		edi, 2
				dec		bh
				jne		dclockout_lp1
				popad
				ret
	}
}
#endif

