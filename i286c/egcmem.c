#include	"compiler.h"
#include	"memory.h"
#include	"egcmem.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"


// エンディアン修正しる！

// C版EGCのみ ROPの回数を記録する
// #define		LOG_EGCROP


static	QWORD_P		egc_src;
static	QWORD_P		data;

static const UINT planead[4] = {VRAM_B, VRAM_R, VRAM_G, VRAM_E};


static const BYTE bytemask_u0[64] =		// dir:right by startbit + (len-1)*8
					{0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
					 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01,
					 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x03, 0x01,
					 0xf0, 0x78, 0x3c, 0x1e, 0x0f, 0x07, 0x03, 0x01,
					 0xf8, 0x7c, 0x3e, 0x1f, 0x0f, 0x07, 0x03, 0x01,
					 0xfc, 0x7e, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
					 0xfe, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
					 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

static const BYTE bytemask_u1[8] =		// dir:right by length
					{0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

static const BYTE bytemask_d0[64] =		// dir:left by startbit + (len-1)*8
					{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
					 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80,
					 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xc0, 0x80,
					 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xe0, 0xc0, 0x80,
					 0x1f, 0x3e, 0x7c, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
					 0x3f, 0x7e, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
					 0x7f, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
					 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80};

static const BYTE bytemask_d1[8] =		// dir:left by length
					{0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

#ifdef LOG_EGCROP
static	int		egcropcnt[256];
#endif


void egcshift(void) {

	BYTE	src8, dst8;

	egc.remain = (egc.leng & 0xfff) + 1;
	egc.func = (egc.sft >> 12) & 1;
	if (!egc.func) {
		egc.inptr = egc.buf;
		egc.outptr = egc.buf;
	}
	else {
		egc.inptr = egc.buf + 4096/8 + 3;
		egc.outptr = egc.buf + 4096/8 + 3;
	}
	egc.srcbit = egc.sft & 0x0f;
	egc.dstbit = (egc.sft >> 4) & 0x0f;

	src8 = egc.srcbit & 0x07;
	dst8 = egc.dstbit & 0x07;
	if (src8 < dst8) {

// dir:inc
// ****---4 -------8 --------
// ******-- -4------ --8----- --
// 1st -> data[0] >> (dst - src)
// 2nd -> (data[0] << (8 - (dst - src))) | (data[1] >> (dst - src))

// dir:dec
//          -------- 8------- 6-----**
//      --- -----8-- -----6-- ---*****
// 1st -> data[0] << (dst - src)
// 2nd -> (data[0] >> (8 - (dst - src))) | (data[1] << (dst - src))

		egc.func += 2;
		egc.sft8bitr = dst8 - src8;
		egc.sft8bitl = 8 - egc.sft8bitr;
	}
	else if (src8 > dst8) {

// dir:inc
// ****---4 -------8 --------
// **---4-- -----8-- ------
// 1st -> (data[0] << (src - dst)) | (data[1] >> (8 - (src - dst))
// 2nd -> (data[0] << (src - dst)) | (data[1] >> (8 - (src - dst))

// dir:dec
//          -------- 8------- 3--*****
//             ----- ---8---- ---3--**
// 1st -> (data[0] >> (dst - src)) | (data[-1] << (8 - (src - dst))
// 2nd -> (data[0] >> (dst - src)) | (data[-1] << (8 - (src - dst))

		egc.func += 4;
		egc.sft8bitl = src8 - dst8;
		egc.sft8bitr = 8 - egc.sft8bitl;
	}
	egc.stack = 0;
}


static void egcsftb_upn_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
			egc.dstbit = 0;
		}
		else {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
			egc.dstbit = 0;
		}
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_u1[egc.remain - 1];
			egc.remain = 0;
		}
	}
	egc_src.b[0][ext] = egc.outptr[0];
	egc_src.b[1][ext] = egc.outptr[4];
	egc_src.b[2][ext] = egc.outptr[8];
	egc_src.b[3][ext] = egc.outptr[12];
	egc.outptr++;
}


static void egcsftb_dnn_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
			egc.dstbit = 0;
		}
		else {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
			egc.dstbit = 0;
		}
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_d1[egc.remain - 1];
			egc.remain = 0;
		}
	}
	egc_src.b[0][ext] = egc.outptr[0];
	egc_src.b[1][ext] = egc.outptr[4];
	egc_src.b[2][ext] = egc.outptr[8];
	egc_src.b[3][ext] = egc.outptr[12];
	egc.outptr--;
}


// ****---4 -------8 --------
// ******-- -4------ --8----- --
// 1st -> data[0] >> (dst - src)
// 2nd -> (data[0] << (8 - (dst - src))) | (data[1] >> (dst - src))

static void egcsftb_upr_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
		}
		else {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
		}
		egc.dstbit = 0;
		egc_src.b[0][ext] = (egc.outptr[0] >> egc.sft8bitr);
		egc_src.b[1][ext] = (egc.outptr[4] >> egc.sft8bitr);
		egc_src.b[2][ext] = (egc.outptr[8] >> egc.sft8bitr);
		egc_src.b[3][ext] = (egc.outptr[12] >> egc.sft8bitr);
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_u1[egc.remain - 1];
			egc.remain = 0;
		}
		egc_src.b[0][ext] = (egc.outptr[0] << egc.sft8bitl) |
							(egc.outptr[1] >> egc.sft8bitr);
		egc_src.b[1][ext] = (egc.outptr[4] << egc.sft8bitl) |
							(egc.outptr[5] >> egc.sft8bitr);
		egc_src.b[2][ext] = (egc.outptr[8] << egc.sft8bitl) |
							(egc.outptr[9] >> egc.sft8bitr);
		egc_src.b[3][ext] = (egc.outptr[12] << egc.sft8bitl) |
							(egc.outptr[13] >> egc.sft8bitr);
		egc.outptr++;
	}
}


//          -------- 8------- 6-----**
//      --- -----8-- -----6-- ---*****
// 1st -> data[0] << (dst - src)
// 2nd -> (data[0] >> (8 - (dst - src))) | (data[-1] << (dst - src))

static void egcsftb_dnr_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
		}
		else {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
		}
		egc.dstbit = 0;
		egc_src.b[0][ext] = (egc.outptr[0] << egc.sft8bitr);
		egc_src.b[1][ext] = (egc.outptr[4] << egc.sft8bitr);
		egc_src.b[2][ext] = (egc.outptr[8] << egc.sft8bitr);
		egc_src.b[3][ext] = (egc.outptr[12] << egc.sft8bitr);
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_d1[egc.remain - 1];
			egc.remain = 0;
		}
		egc.outptr--;
		egc_src.b[0][ext] = (egc.outptr[1] >> egc.sft8bitl) |
							(egc.outptr[0] << egc.sft8bitr);
		egc_src.b[1][ext] = (egc.outptr[5] >> egc.sft8bitl) |
							(egc.outptr[4] << egc.sft8bitr);
		egc_src.b[2][ext] = (egc.outptr[9] >> egc.sft8bitl) |
							(egc.outptr[8] << egc.sft8bitr);
		egc_src.b[3][ext] = (egc.outptr[13] >> egc.sft8bitl) |
							(egc.outptr[12] << egc.sft8bitr);
	}
}


// ****---4 -------8 --------
// **---4-- -----8-- ------
// 1st -> (data[0] << (src - dst)) | (data[1] >> (8 - (src - dst))
// 2nd -> (data[0] << (src - dst)) | (data[1] >> (8 - (src - dst))

static void egcsftb_upl_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
			egc.dstbit = 0;
		}
		else {
			egc.srcmask.b[ext] = bytemask_u0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
			egc.dstbit = 0;
		}
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_u1[egc.remain - 1];
			egc.remain = 0;
		}
	}
	egc_src.b[0][ext] = (egc.outptr[0] << egc.sft8bitl) |
						(egc.outptr[1] >> egc.sft8bitr);
	egc_src.b[1][ext] = (egc.outptr[4] << egc.sft8bitl) |
						(egc.outptr[5] >> egc.sft8bitr);
	egc_src.b[2][ext] = (egc.outptr[8] << egc.sft8bitl) |
						(egc.outptr[9] >> egc.sft8bitr);
	egc_src.b[3][ext] = (egc.outptr[12] << egc.sft8bitl) |
						(egc.outptr[13] >> egc.sft8bitr);
	egc.outptr++;
}


//          -------- 8------- 3--*****
//             ----- ---8---- ---3--**
// 1st -> (data[0] >> (dst - src)) | (data[-1] << (8 - (src - dst))
// 2nd -> (data[0] >> (dst - src)) | (data[-1] << (8 - (src - dst))

static void egcsftb_dnl_sub(DWORD ext) {

	if (egc.dstbit >= 8) {
		egc.dstbit -= 8;
		egc.srcmask.b[ext] = 0;
		return;
	}
	if (egc.dstbit) {
		if ((egc.dstbit + egc.remain) >= 8) {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (7*8)];
			egc.remain -= (8 - egc.dstbit);
			egc.dstbit = 0;
		}
		else {
			egc.srcmask.b[ext] = bytemask_d0[egc.dstbit + (egc.remain - 1)*8];
			egc.remain = 0;
			egc.dstbit = 0;
		}
	}
	else {
		if (egc.remain >= 8) {
			egc.remain -= 8;
		}
		else {
			egc.srcmask.b[ext] = bytemask_d1[egc.remain - 1];
			egc.remain = 0;
		}
	}
	egc.outptr--;
	egc_src.b[0][ext] = (egc.outptr[1] >> egc.sft8bitl) |
						(egc.outptr[0] << egc.sft8bitr);
	egc_src.b[1][ext] = (egc.outptr[5] >> egc.sft8bitl) |
						(egc.outptr[4] << egc.sft8bitr);
	egc_src.b[2][ext] = (egc.outptr[9] >> egc.sft8bitl) |
						(egc.outptr[8] << egc.sft8bitr);
	egc_src.b[3][ext] = (egc.outptr[13] >> egc.sft8bitl) |
						(egc.outptr[12] << egc.sft8bitr);
}


static void egcsftb_upn0(DWORD adrs) {

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_upn_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_upn0(DWORD adrs) {

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_upn_sub(0);
	if (egc.remain) {
		egcsftb_upn_sub(1);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[1] = 0;
	}
	egcshift();
	(void)adrs;
}


static void egcsftb_dnn0(DWORD adrs) {

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_dnn_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_dnn0(DWORD adrs) {

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_dnn_sub(1);
	if (egc.remain) {
		egcsftb_dnn_sub(0);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[0] = 0;
	}
	egcshift();
	(void)adrs;
}


static void egcsftb_upr0(DWORD adrs) {			// dir:up srcbit < dstbit

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_upr_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_upr0(DWORD adrs) {			// dir:up srcbit < dstbit

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_upr_sub(0);
	if (egc.remain) {
		egcsftb_upr_sub(1);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[1] = 0;
	}
	egcshift();
	(void)adrs;
}


static void egcsftb_dnr0(DWORD adrs) {			// dir:up srcbit < dstbit

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_dnr_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_dnr0(DWORD adrs) {			// dir:up srcbit < dstbit

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_dnr_sub(1);
	if (egc.remain) {
		egcsftb_dnr_sub(0);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[0] = 0;
	}
	egcshift();
	(void)adrs;
}


static void egcsftb_upl0(DWORD adrs) {			// dir:up srcbit > dstbit

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_upl_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_upl0(DWORD adrs) {			// dir:up srcbit > dstbit

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_upl_sub(0);
	if (egc.remain) {
		egcsftb_upl_sub(1);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[1] = 0;
	}
	egcshift();
	(void)adrs;
}


static void egcsftb_dnl0(DWORD adrs) {			// dir:up srcbit > dstbit

	int		ext;

	ext = adrs & 1;
	if (egc.stack < (DWORD)(8 - egc.dstbit)) {
		egc.srcmask.b[ext] = 0;
		return;
	}
	egc.stack -= (8 - egc.dstbit);
	egcsftb_dnl_sub(ext);
	if (!egc.remain) {
		egcshift();
	}
}

static void egcsftw_dnl0(DWORD adrs) {			// dir:up srcbit > dstbit

	if (egc.stack < (DWORD)(16 - egc.dstbit)) {
		egc.srcmask.w = 0;
		return;
	}
	egc.stack -= (16 - egc.dstbit);
	egcsftb_dnl_sub(1);
	if (egc.remain) {
		egcsftb_dnl_sub(0);
		if (egc.remain) {
			return;
		}
	}
	else {
		egc.srcmask.b[0] = 0;
	}
	egcshift();
	(void)adrs;
}


static void (*egcsft_proc[])(DWORD adrs) = {
		egcsftw_upn0,	egcsftw_dnn0,
		egcsftw_upr0,	egcsftw_dnr0,
		egcsftw_upl0,	egcsftw_dnl0,

		egcsftb_upn0,	egcsftb_dnn0,
		egcsftb_upr0,	egcsftb_dnr0,
		egcsftb_upl0,	egcsftb_dnl0};

// ---------------------------------------------------------------------------

static void shiftinput_byte(DWORD ext) {

	if (egc.stack <= 16) {
		if (egc.srcbit >= 8) {
			egc.srcbit -= 8;
		}
		else {
			egc.stack += (8 - egc.srcbit);
			egc.srcbit = 0;
		}
		if (!(egc.sft & 0x1000)) {
			egc.inptr++;
		}
		else {
			egc.inptr--;
		}
	}
	egc.srcmask.b[ext] = 0xff;
	egcsft_proc[egc.func + 6](ext);
}

static void shiftinput_incw(void) {

	if (egc.stack <= 16) {
		egc.inptr += 2;
		if (egc.srcbit >= 8) {
			egc.outptr++;
		}
		egc.stack += (16 - egc.srcbit);
		egc.srcbit = 0;
	}
	egc.srcmask.w = 0xffff;
	egcsft_proc[egc.func](0);
}

static void shiftinput_decw(void) {

	if (egc.stack <= 16) {
		egc.inptr -= 2;
		if (egc.srcbit >= 8) {
			egc.outptr--;
		}
		egc.stack += (16 - egc.srcbit);
		egc.srcbit = 0;
	}
	egc.srcmask.w = 0xffff;
	egcsft_proc[egc.func](0);
}


#define	EGCOPE_SHIFT {												\
	if (egc.ope & 0x400) {											\
		if (func < 6) {												\
			if (!(egc.sft & 0x1000)) {								\
				*(WORD *)(egc.inptr + 0) = value;					\
				*(WORD *)(egc.inptr + 4) = value;					\
				*(WORD *)(egc.inptr + 8) = value;					\
				*(WORD *)(egc.inptr +12) = value;					\
				shiftinput_incw();									\
			}														\
			else {													\
				*(WORD *)(egc.inptr - 1) = value;					\
				*(WORD *)(egc.inptr + 3) = value;					\
				*(WORD *)(egc.inptr + 7) = value;					\
				*(WORD *)(egc.inptr +11) = value;					\
				shiftinput_decw();									\
			}														\
		}															\
		else {														\
			*(egc.inptr + 0) = (BYTE)value;							\
			*(egc.inptr + 4) = (BYTE)value;							\
			*(egc.inptr + 8) = (BYTE)value;							\
			*(egc.inptr +12) = (BYTE)value;							\
			shiftinput_byte(ad & 1);								\
		}															\
	}																\
}


static void gdc_ope(DWORD ad, WORD value, int func) {

	QWORD_P		pat;
	QWORD_P		dst;

	egc.mask2.w = egc.mask.w;

	switch(egc.ope & 0x1800) {
		case 0x0800:
			EGCOPE_SHIFT;
			switch(egc.fgbg & 0x6000) {
				case 0x2000:
					pat.d[0] = egc.bgc.d[0];
					pat.d[1] = egc.bgc.d[1];
					break;
				case 0x4000:
					pat.d[0] = egc.fgc.d[0];
					pat.d[1] = egc.fgc.d[1];
					break;
				default:
					if ((egc.ope & 0x0300) == 0x0100) {	// ver0.29
						pat.d[0] = egc_src.d[0];
						pat.d[1] = egc_src.d[1];
					}
					else {
						pat.d[0] = egc.patreg.d[0];
						pat.d[1] = egc.patreg.d[1];
					}
					break;
			}
			ad &= ~1;
			dst.w[0] = *(WORD *)(&mem[ad + VRAM_B]);
			dst.w[1] = *(WORD *)(&mem[ad + VRAM_R]);
			dst.w[2] = *(WORD *)(&mem[ad + VRAM_G]);
			dst.w[3] = *(WORD *)(&mem[ad + VRAM_E]);

#ifdef LOG_EGCROP
			egcropcnt[egc.ope & 0xff]++;
#endif
			data.d[0] = 0;
			data.d[1] = 0;
			if (egc.ope & 0x80) {
				data.d[0] |= (pat.d[0] & egc_src.d[0] & dst.d[0]);
				data.d[1] |= (pat.d[1] & egc_src.d[1] & dst.d[1]);
			}
			if (egc.ope & 0x40) {
				data.d[0] |= ((~pat.d[0]) & egc_src.d[0] & dst.d[0]);
				data.d[1] |= ((~pat.d[1]) & egc_src.d[1] & dst.d[1]);
			}
			if (egc.ope & 0x20) {
				data.d[0] |= (pat.d[0] & egc_src.d[0] & (~dst.d[0]));
				data.d[1] |= (pat.d[1] & egc_src.d[1] & (~dst.d[1]));
			}
			if (egc.ope & 0x10) {
				data.d[0] |= ((~pat.d[0]) & egc_src.d[0] & (~dst.d[0]));
				data.d[1] |= ((~pat.d[1]) & egc_src.d[1] & (~dst.d[1]));
			}
			if (egc.ope & 0x08) {
				data.d[0] |= (pat.d[0] & (~egc_src.d[0]) & dst.d[0]);
				data.d[1] |= (pat.d[1] & (~egc_src.d[1]) & dst.d[1]);
			}
			if (egc.ope & 0x04) {
				data.d[0] |= ((~pat.d[0]) & (~egc_src.d[0]) & dst.d[0]);
				data.d[1] |= ((~pat.d[1]) & (~egc_src.d[1]) & dst.d[1]);
			}
			if (egc.ope & 0x02) {
				data.d[0] |= (pat.d[0] & (~egc_src.d[0]) & (~dst.d[0]));
				data.d[1] |= (pat.d[1] & (~egc_src.d[1]) & (~dst.d[1]));
			}
			if (egc.ope & 0x01) {
				data.d[0] |= ((~pat.d[0]) & (~egc_src.d[0]) & (~dst.d[0]));
				data.d[1] |= ((~pat.d[1]) & (~egc_src.d[1]) & (~dst.d[1]));
			}
			egc.mask2.w &= egc.srcmask.w;
			break;
		case 0x1000:
			switch(egc.fgbg & 0x6000) {
				case 0x2000:							// ver0.29
					data.d[0] = egc.bgc.d[0];
					data.d[1] = egc.bgc.d[1];
					break;
				case 0x4000:							// ver0.29
					data.d[0] = egc.fgc.d[0];
					data.d[1] = egc.fgc.d[1];
					break;
				default:
#if 0
					data.d[0] = egc.patreg.d[0];
					data.d[1] = egc.patreg.d[1];
#else
					EGCOPE_SHIFT;
					data.d[0] = egc_src.d[0];
					data.d[1] = egc_src.d[1];
					egc.mask2.w &= egc.srcmask.w;
#endif
					break;
			}
			break;
		default:
			data.w[0] = value;
			data.w[1] = value;
			data.w[2] = value;
			data.w[3] = value;
			break;
	}
}

BYTE MEMCALL egc_read(UINT32 addr) {

	DWORD	ad;
	DWORD	ext;

//	TRACE_("EGC read_byte", (WORD)(addr & 0x7fff));

	if (gdcs.access) {
		addr += VRAM_STEP;
	}
	ad = VRAM_POS(addr);
	ext = addr & 1;
	egc.lastvram.b[0][ext] = mem[ad + VRAM_B];
	egc.lastvram.b[1][ext] = mem[ad + VRAM_R];
	egc.lastvram.b[2][ext] = mem[ad + VRAM_G];
	egc.lastvram.b[3][ext] = mem[ad + VRAM_E];

	// shift input
	if (!(egc.ope & 0x400)) {
		egc.inptr[0] = egc.lastvram.b[0][ext];
		egc.inptr[4] = egc.lastvram.b[1][ext];
		egc.inptr[8] = egc.lastvram.b[2][ext];
		egc.inptr[12] = egc.lastvram.b[3][ext];
		shiftinput_byte(ext);
	}

	if ((egc.ope & 0x0300) == 0x0100) {
		egc.patreg.b[0][ext] = mem[ad + VRAM_B];
		egc.patreg.b[1][ext] = mem[ad + VRAM_R];
		egc.patreg.b[2][ext] = mem[ad + VRAM_G];
		egc.patreg.b[3][ext] = mem[ad + VRAM_E];
	}
	if (!(egc.ope & 0x2000)) {
		int pl = (egc.fgbg >> 8) & 3;
		if (!(egc.ope & 0x400)) {
			return(egc_src.b[pl][ext]);
		}
		else {
			return(mem[ad + planead[pl]]);
		}
	}
	return(mem[addr]);
}


void MEMCALL egc_write(UINT32 addr, BYTE value) {

	DWORD	ext;
	WORD	wvalue;

//	TRACE_("EGC write_byte", (WORD)(addr & 0x7fff));

	addr &= 0x7fff;
	ext = addr & 1;
	if (!gdcs.access) {
		gdcs.grphdisp |= 1;
		vramupdate[addr] |= 0x01;
	}
	else {
		gdcs.grphdisp |= 2;
		vramupdate[addr] |= 0x02;
		addr += VRAM_STEP;
	}
	if ((egc.ope & 0x0300) == 0x0200) {
		egc.patreg.b[0][ext] = mem[addr + VRAM_B];
		egc.patreg.b[1][ext] = mem[addr + VRAM_R];
		egc.patreg.b[2][ext] = mem[addr + VRAM_G];
		egc.patreg.b[3][ext] = mem[addr + VRAM_E];
	}

	wvalue = (value << 8) | (value);							// ver0.28/pr4
	if (!ext) {
		gdc_ope(addr, wvalue, egc.func + 6);
	}
	else {
		gdc_ope(addr, wvalue, egc.func + 6);
	}
	if (egc.mask2.b[ext]) {
		if (!(egc.access & 1)) {
			mem[addr + VRAM_B] &= ~egc.mask2.b[ext];
			mem[addr + VRAM_B] |= data.b[0][ext] & egc.mask2.b[ext];
		}
		if (!(egc.access & 2)) {
			mem[addr + VRAM_R] &= ~egc.mask2.b[ext];
			mem[addr + VRAM_R] |= data.b[1][ext] & egc.mask2.b[ext];
		}
		if (!(egc.access & 4)) {
			mem[addr + VRAM_G] &= ~egc.mask2.b[ext];
			mem[addr + VRAM_G] |= data.b[2][ext] & egc.mask2.b[ext];
		}
		if (!(egc.access & 8)) {
			mem[addr + VRAM_E] &= ~egc.mask2.b[ext];
			mem[addr + VRAM_E] |= data.b[3][ext] & egc.mask2.b[ext];
		}
	}
}

UINT16 MEMCALL egc_read_w(UINT32 addr) {

	DWORD	ad;

//	TRACE_("EGC read_word", (WORD)(addr & 0x7fff));

	if (!(addr & 1)) {
		if (gdcs.access) {
			addr += VRAM_STEP;
		}
		ad = VRAM_POS(addr);
		egc.lastvram.w[0] = *(WORD *)(&mem[ad + VRAM_B]);
		egc.lastvram.w[1] = *(WORD *)(&mem[ad + VRAM_R]);
		egc.lastvram.w[2] = *(WORD *)(&mem[ad + VRAM_G]);
		egc.lastvram.w[3] = *(WORD *)(&mem[ad + VRAM_E]);

		// shift input
		if (!(egc.ope & 0x400)) {
			if (!(egc.sft & 0x1000)) {
				*(WORD *)(egc.inptr + 0) = egc.lastvram.w[0];
				*(WORD *)(egc.inptr + 4) = egc.lastvram.w[1];
				*(WORD *)(egc.inptr + 8) = egc.lastvram.w[2];
				*(WORD *)(egc.inptr +12) = egc.lastvram.w[3];
				shiftinput_incw();
			}
			else {
				*(WORD *)(egc.inptr - 1) = egc.lastvram.w[0];
				*(WORD *)(egc.inptr + 3) = egc.lastvram.w[1];
				*(WORD *)(egc.inptr + 7) = egc.lastvram.w[2];
				*(WORD *)(egc.inptr +11) = egc.lastvram.w[3];
				shiftinput_decw();
			}
		}

		if ((egc.ope & 0x0300) == 0x0100) {
			egc.patreg.d[0] = egc.lastvram.d[0];
			egc.patreg.d[1] = egc.lastvram.d[1];
		}
		if (!(egc.ope & 0x2000)) {
			int pl = (egc.fgbg >> 8) & 3;
			if (!(egc.ope & 0x400)) {
				return(egc_src.w[pl]);
			}
			else {
				return(*(WORD *)(&mem[ad + planead[pl]]));
			}
		}
		return(*(WORD *)(&mem[addr]));
	}
	else if (!(egc.sft & 0x1000)) {
		WORD	ret;
		ret = egc_read(addr);
		ret |= egc_read(addr+1) << 8;
		return(ret);
	}
	else {
		WORD	ret;
		ret = egc_read(addr+1) << 8;
		ret |= egc_read(addr);
		return(ret);
	}
}

void MEMCALL egc_write_w(UINT32 addr, UINT16 value) {

//	TRACE_("EGC write_word", (WORD)(addr & 0x7fff));

	if (!(addr & 1)) {											// word access
		addr &= 0x7ffe;
		if (!gdcs.access) {
			gdcs.grphdisp |= 1;
			*(WORD *)(&vramupdate[addr]) |= 0x0101;
		}
		else {
			gdcs.grphdisp |= 2;
			*(WORD *)(&vramupdate[addr]) |= 0x0202;
			addr += VRAM_STEP;
		}
		if ((egc.ope & 0x0300) == 0x0200) {
			egc.patreg.w[0] = *(WORD *)(&mem[addr + VRAM_B]);
			egc.patreg.w[1] = *(WORD *)(&mem[addr + VRAM_R]);
			egc.patreg.w[2] = *(WORD *)(&mem[addr + VRAM_G]);
			egc.patreg.w[3] = *(WORD *)(&mem[addr + VRAM_E]);
		}
		gdc_ope(addr, value, egc.func);
		if (egc.mask2.w) {
			if (!(egc.access & 1)) {
				*(WORD *)(&mem[addr + VRAM_B]) &= ~egc.mask2.w;
				*(WORD *)(&mem[addr + VRAM_B]) |= data.w[0] & egc.mask2.w;
			}
			if (!(egc.access & 2)) {
				*(WORD *)(&mem[addr + VRAM_R]) &= ~egc.mask2.w;
				*(WORD *)(&mem[addr + VRAM_R]) |= data.w[1] & egc.mask2.w;
			}
			if (!(egc.access & 4)) {
				*(WORD *)(&mem[addr + VRAM_G]) &= ~egc.mask2.w;
				*(WORD *)(&mem[addr + VRAM_G]) |= data.w[2] & egc.mask2.w;
			}
			if (!(egc.access & 8)) {
				*(WORD *)(&mem[addr + VRAM_E]) &= ~egc.mask2.w;
				*(WORD *)(&mem[addr + VRAM_E]) |= data.w[3] & egc.mask2.w;
			}
		}
	}
	else if (!(egc.sft & 0x1000)) {
		egc_write(addr, (BYTE)value);
		egc_write(addr+1, (BYTE)(value >> 8));
	}
	else {
		egc_write(addr+1, (BYTE)(value >> 8));
		egc_write(addr, (BYTE)value);
	}
}

