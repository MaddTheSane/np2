#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"font.h"


static	int		sti_waiting = 0;

typedef struct {
	BYTE	GBON_PTN;
	BYTE	GBBCC;
	BYTE	GBDOTU;
	BYTE	GBDSP;
	BYTE	GBCPC[4];
	BYTE	GBSX1[2];
	BYTE	GBSY1[2];
	BYTE	GBLNG1[2];
	BYTE	GBWDPA[2];
	BYTE	GBRBUF[2][3];
	BYTE	GBSX2[2];
	BYTE	GBSY2[2];
	BYTE	GBMDOT[2];
	BYTE	GBCIR[2];
	BYTE	GBLNG2[2];
	BYTE	GBMDOTI[8];
	BYTE	GBDTYP;
	BYTE	GBFILL;
} UCWTBL;

typedef struct {
	UINT8	raster;
	UINT8	pl;
	UINT8	bl;
	UINT8	cl;
} CRTDATA;

static const CRTDATA crtdata[4] = {
						{0x07,	0x00, 0x07, 0x08},
						{0x09,	0x1f, 0x08, 0x08},
						{0x0f,	0x00, 0x0f, 0x10},
						{0x13,	0x1e, 0x11, 0x10}};

typedef struct {
	UINT8	lr;
	UINT8	cfi;
} CSRFORM;

static const CSRFORM csrform[4] = {
						{0x07, 0x3b}, {0x09, 0x4b},
						{0x0f, 0x7b}, {0x13, 0x9b}};

static const UINT8 sync200l[8] = {0x02,0x26,0x03,0x11,0x86,0x0f,0xc8,0x84};
static const UINT8 sync200m[8] = {0x02,0x26,0x03,0x11,0x83,0x07,0x90,0x65};
static const UINT8 sync400m[8] = {0x02,0x4e,0x07,0x25,0x87,0x07,0x90,0x65};


static UINT16 keyget(void) {

	UINT	pos;
	UINT	kbbufhead;

	if (mem[MEMB_KB_COUNT]) {
		mem[MEMB_KB_COUNT]--;
		pos = GETBIOSMEM16(MEMW_KB_BUF_HEAD);
		kbbufhead = pos + 2;
		if (kbbufhead >= 0x522) {
			kbbufhead = 0x502;
		}
		SETBIOSMEM16(MEMW_KB_BUF_HEAD, kbbufhead);
		return(GETBIOSMEM16(pos));
	}
	return(0xffff);
}

static void bios0x18_10(REG8 curdel) {

	UINT8	sts;
	UINT	pos;

	sts = mem[MEMB_CRT_STS_FLAG];
	mem[MEMB_CRT_STS_FLAG] = sts & (~0x40);
	pos = sts & 0x01;
	if (sts & 0x80) {
		pos += 2;
	}
	mem[MEMB_CRT_CNT] = (curdel << 5);
	gdc.m.para[GDC_CSRFORM + 0] = csrform[pos].lr;
	gdc.m.para[GDC_CSRFORM + 1] = curdel << 5;
	gdc.m.para[GDC_CSRFORM + 2] = csrform[pos].cfi;
	gdcs.textdisp |= GDCSCRN_ALLDRAW2 | GDCSCRN_EXT;
}

void bios0x18_16(BYTE chr, BYTE atr) {

	UINT32	i;

	for (i=0xa0000; i<0xa2000; i+=2) {
		mem[i+0] = chr;
		mem[i+1] = 0;
	}
	for (; i<0xa3fe0; i+=2) {
		mem[i] = atr;
	}
	gdcs.textdisp |= GDCSCRN_ALLDRAW;
}

#define	SWAPU16(a, b) { UINT16 tmp; tmp = (a); (a) = (b); (b) = tmp; }

static REG8 swapbit(REG8 bit) {

	REG8	ret;

	ret = 0;
	while(bit) {
		ret = (ret << 1) + (bit & 1);
		bit >>= 1;
	}
	return(ret);
}

static void bios18_47(void) {

	UCWTBL		ucw;
	GDCVECT		vect;
	UINT16		GBSX1;
	UINT16		GBSY1;
	UINT16		GBSX2;
	UINT16		GBSY2;
	GDCSUBFN	func;
	UINT32		csrw;
	UINT16		data;
	UINT16		data2;
	UINT16		GBMDOTI;
	UINT8		ope;
	SINT16		dx;
	SINT16		dy;

	// GDCバッファを空に
	if (gdc.s.cnt) {
		gdc_work(GDCWORK_SLAVE);
	}
	gdc_forceready(&gdc.s);

	i286_memstr_read(CPU_DS, CPU_BX, &ucw, sizeof(ucw));

	GBSX1 = LOADINTELWORD(ucw.GBSX1);
	GBSY1 = LOADINTELWORD(ucw.GBSY1);
	GBSX2 = LOADINTELWORD(ucw.GBSX2);
	GBSY2 = LOADINTELWORD(ucw.GBSY2);
	ZeroMemory(&vect, sizeof(vect));
	if (ucw.GBDTYP == 0x01) {
		func = gdcsub_line;
		if ((GBSX1 > GBSX2) ||
			((GBSX1 == GBSX2) && (GBSY1 > GBSY2))) {
			SWAPU16(GBSX1, GBSX2);
			SWAPU16(GBSY1, GBSY2);
		}
		dx = GBSX2 - GBSX1;
		dy = GBSY2 - GBSY1;
		if (dy > 0) {
			if (dx < dy) {
				vect.ope = 0;
				SWAPU16(dx, dy);
			}
			else {
				vect.ope = 1;
			}
		}
		else {
			dy = -dy;
			if (dx > dy) {
				vect.ope = 2;
			}
			else {
				vect.ope = 3;
				SWAPU16(dx, dy);
			}
		}
		vect.ope += 0x08;
		STOREINTELWORD(vect.DC, dx);
		data = dy * 2;
		STOREINTELWORD(vect.D1, data);
		data -= dx;
		STOREINTELWORD(vect.D, data);
		data -= dx;
		STOREINTELWORD(vect.D2, data);
	}
	else if (ucw.GBDTYP <= 0x02) {
		func = gdcsub_box;
		vect.ope = 0x40 + (ucw.GBDSP & 7);
		dx = GBSX2 - GBSX1;
		if (dx < 0) {
			dx = 0 - dx;
		}
		dy = GBSY2 - GBSY1;
		if (dy < 0) {
			dy = 0 - dy;
		}
		switch(ucw.GBDSP & 3) {
			case 0:
				data = dy;
				data2 = dx;
				break;

			case 1:
				data2 = (UINT16)dx + (UINT16)dy;
				data2 >>= 1;
				data = (UINT16)dx - (UINT16)dy;
				data = (data >> 1) & 0x3fff;
				break;

			case 2:
				data = dx;
				data2 = dy;
				break;

			case 3:
				data2 = (UINT16)dx + (UINT16)dy;
				data2 >>= 1;
				data = (UINT16)dy - (UINT16)dx;
				data = (data >> 1) & 0x3fff;
				break;
		}
		STOREINTELWORD(vect.DC, 3);
		STOREINTELWORD(vect.D, data);
		STOREINTELWORD(vect.D2, data2);
		STOREINTELWORD(vect.D1, 0xffff);
		STOREINTELWORD(vect.DM, data);
	}
	else {
		func = gdcsub_circle;
		vect.ope = 0x20 + (ucw.GBDSP & 7);
		vect.DC[0] = ucw.GBLNG1[0];
		vect.DC[1] = ucw.GBLNG1[1];
		data = LOADINTELWORD(ucw.GBLNG2) - 1;
		STOREINTELWORD(vect.D, data);
		data >>= 1;
		STOREINTELWORD(vect.D2, data);
		STOREINTELWORD(vect.D1, 0x3fff);
		if (ucw.GBDTYP == 0x04) {
			vect.DM[0] = ucw.GBMDOT[0];
			vect.DM[1] = ucw.GBMDOT[1];
		}
	}
	if ((CPU_CH & 0xc0) == 0x40) {
		GBSY1 += 200;
	}
	csrw = (GBSY1 * 40) + (GBSX1 >> 4);
	csrw += (GBSX1 & 0xf) << 20;
	GBMDOTI = (swapbit(ucw.GBMDOTI[0]) << 8) + swapbit(ucw.GBMDOTI[1]);
	if ((CPU_CH & 0x30) == 0x30) {
		ope = (ucw.GBON_PTN & 1)?GDCOPE_SET:GDCOPE_CLEAR;
		func(csrw + 0x4000, &vect, GBMDOTI, ope);
		ope = (ucw.GBON_PTN & 2)?GDCOPE_SET:GDCOPE_CLEAR;
		func(csrw + 0x8000, &vect, GBMDOTI, ope);
		ope = (ucw.GBON_PTN & 4)?GDCOPE_SET:GDCOPE_CLEAR;
		csrw += 0xc000;
		func(csrw, &vect, GBMDOTI, ope);
	}
	else {
		ope = ucw.GBDOTU & 3;
		csrw += 0x4000 + ((CPU_CH & 0x30) << 10);
		func(csrw, &vect, GBMDOTI, ope);
	}

	// 最後に使った奴を記憶
	*(UINT16 *)(mem + MEMW_PRXGLS) = *(UINT16 *)(ucw.GBMDOTI);
	*(UINT16 *)(gdc.s.para + GDC_TEXTW) = *(UINT16 *)(ucw.GBMDOTI);

	gdc.s.para[GDC_WRITE] = ope;
	mem[MEMB_PRXDUPD] &= ~3;
	mem[MEMB_PRXDUPD] |= ope;
}

static void bios18_49(void) {

	UCWTBL		ucw;
	UINT		i;
	BYTE		pat[8];
	UINT16		tmp;
	GDCVECT		vect;
	UINT16		GBSX1;
	UINT16		GBSY1;
	UINT32		csrw;
	UINT8		ope;

	// GDCバッファを空に
	if (gdc.s.cnt) {
		gdc_work(GDCWORK_SLAVE);
	}
	gdc_forceready(&gdc.s);

	i286_memstr_read(CPU_DS, CPU_BX, &ucw, sizeof(ucw));
	for (i=0; i<8; i++) {
		mem[MEMW_PRXGLS + i] = ucw.GBMDOTI[i];
		pat[i] = swapbit(ucw.GBMDOTI[i]);
		gdc.s.para[GDC_TEXTW + i] = pat[i];
	}
	vect.ope = 0x10 + (ucw.GBDSP & 7);
	if (*(UINT16 *)ucw.GBLNG1) {
		tmp = (LOADINTELWORD(ucw.GBLNG2) - 1) & 0x3fff;
		STOREINTELWORD(vect.DC, tmp);
		vect.D[0] = ucw.GBLNG1[0];
		vect.D[1] = ucw.GBLNG1[1];
	}
	else {
		STOREINTELWORD(vect.DC, 7);
		vect.D[0] = gdc.s.para[GDC_VECTW + 3];
		vect.D[1] = gdc.s.para[GDC_VECTW + 4];
	}

	GBSX1 = LOADINTELWORD(ucw.GBSX1);
	GBSY1 = LOADINTELWORD(ucw.GBSY1);
	if ((CPU_CH & 0xc0) == 0x40) {
		GBSY1 += 200;
	}
	csrw = (GBSY1 * 40) + (GBSX1 >> 4);
	csrw += (GBSX1 & 0xf) << 20;
	if ((CPU_CH & 0x30) == 0x30) {
		ope = (ucw.GBON_PTN & 1)?GDCOPE_SET:GDCOPE_CLEAR;
		gdcsub_text(csrw + 0x4000, &vect, pat, ope);
		ope = (ucw.GBON_PTN & 2)?GDCOPE_SET:GDCOPE_CLEAR;
		gdcsub_text(csrw + 0x8000, &vect, pat, ope);
		ope = (ucw.GBON_PTN & 4)?GDCOPE_SET:GDCOPE_CLEAR;
		csrw += 0xc000;
		gdcsub_text(csrw, &vect, pat, ope);
	}
	else {
		ope = ucw.GBDOTU & 3;
		csrw += 0x4000 + ((CPU_CH & 0x30) << 10);
		gdcsub_text(csrw, &vect, pat, ope);
	}

	// 最後に使った奴を記憶
	gdc.s.para[GDC_WRITE] = ope;
	mem[MEMB_PRXDUPD] &= ~3;
	mem[MEMB_PRXDUPD] |= ope;
}


// ----

void bios0x18(void) {

	union {
		BOOL	b;
		UINT16	w;
		UINT32	d;
const CRTDATA	*crt;
	}		tmp;

	UINT	pos;
	BYTE	buf[34];
	BYTE	*p;
	int		i;

#if 0
	TRACEOUT(("int18 AX=%.4x %.4x:%.4x", CPU_AX,
							i286_memword_read(CPU_SS, CPU_SP+2),
							i286_memword_read(CPU_SS, CPU_SP)));
#endif

	sti_waiting ^= 1;
	if (sti_waiting) {					// 割込み許可の遊び
		CPU_STI;
		if (PICEXISTINTR) {
			CPU_IP--;
			nevent_forceexit();
			return;
		}
	}
	sti_waiting = 0;

	switch(CPU_AH) {
		case 0x00:						// キー・データの読みだし
			if (mem[MEMB_KB_COUNT]) {
				CPU_AX = keyget();
			}
			else {
				CPU_IP--;
				CPU_REMCLOCK = -1;
				break;
			}
			break;

   		case 0x01:						// キー・バッファ状態のセンス
			if (mem[MEMB_KB_COUNT]) {
				tmp.d = GETBIOSMEM16(MEMW_KB_BUF_HEAD);
				CPU_AX = GETBIOSMEM16(tmp.d);
				CPU_BH = 1;
			}
			else {
				CPU_BH = 0;
			}
			break;

   		case 0x02:						// シフト・キー状態のセンス
			CPU_AL = mem[MEMB_SHIFT_STS];
			break;

   		case 0x03:						// キーボード・インタフェイスの初期化
			bios0x09_init();
			break;

   		case 0x04:						// キー入力状態のセンス
			CPU_AH = mem[0x00052a + (CPU_AL & 0x0f)];
 			break;

   		case 0x05:						// キー入力センス
			if (mem[MEMB_KB_COUNT]) {
				CPU_AX = keyget();
				CPU_BH = 1;
			}
			else {
				CPU_BH = 0;
			}
 			break;

   		case 0x0a:						// CRTモードの設定
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			gdc.mode1 &= ~(0x2d);
			mem[MEMB_CRT_STS_FLAG] = CPU_AL;
			tmp.crt = crtdata;
			if (!(np2cfg.dipsw[0] & 1)) {
				mem[MEMB_CRT_STS_FLAG] |= 0x80;
				gdc.mode1 |= 0x08;
				tmp.crt += 2;
			}
			if (CPU_AL & 0x01) {
				tmp.crt += 1;					// 20行
			}
			if (CPU_AL & 0x02) {
				gdc.mode1 |= 0x04;				// 40桁
			}
			if (CPU_AL & 0x04) {
				gdc.mode1 |= 0x01;				// アトリビュート
			}
			if (CPU_AL & 0x08) {
				gdc.mode1 |= 0x20;				// コードアクセス
			}
			mem[MEMB_CRT_RASTER] = tmp.crt->raster;
			crtc.reg.pl = tmp.crt->pl;
			crtc.reg.bl = tmp.crt->bl;
			crtc.reg.cl = tmp.crt->cl;
			crtc.reg.ssl = 0;
			gdc_restorekacmode();
			bios0x18_10(0);
			break;

   		case 0x0b:						// CRTモードのセンス
			CPU_AL = mem[MEMB_CRT_STS_FLAG];
 			break;

   		case 0x0c:						// テキスト画面の表示開始
			if (!(gdcs.textdisp & GDCSCRN_ENABLE)) {
				gdcs.textdisp |= GDCSCRN_ENABLE;
				screenupdate |= 2;
 			}
 			break;

   		case 0x0d:						// テキスト画面の表示終了
			if (gdcs.textdisp & GDCSCRN_ENABLE) {
				gdcs.textdisp &= ~(GDCSCRN_ENABLE);
				screenupdate |= 2;
			}
 			break;

		case 0x0e:						// 一つの表示領域の設定
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			ZeroMemory(&gdc.m.para[GDC_SCROLL], 16);
			tmp.w = CPU_DX >> 1;
			SETBIOSMEM16(MEMW_CRT_W_VRAMADR, tmp.w);
			STOREINTELWORD(gdc.m.para + GDC_SCROLL + 0, tmp.w);
			tmp.w = 200 << 4;
			if (mem[MEMB_CRT_STS_FLAG] & 0x80) {
				tmp.w <<= 1;
			}
			SETBIOSMEM16(MEMW_CRT_W_RASTER, tmp.w);
			STOREINTELWORD(gdc.m.para + GDC_SCROLL + 2, tmp.w);
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			screenupdate |= 2;
 			break;

		case 0x0f:						// 複数の表示領域の設定
			SETBIOSMEM16(0x0053e, CPU_CX);
			SETBIOSMEM16(0x00540, CPU_BX);
			mem[0x00547] = CPU_DH;
			mem[0x0053D] = CPU_DL;
			// wait sync int
			if ((i = CPU_DL) > 0) {
				pos = CPU_CX;
				p = gdc.m.para + GDC_SCROLL + (CPU_DH << 2);
				while((i--) && (p < (gdc.m.para + GDC_SCROLL + 0x10))) {
					REG16 t;
					t = i286_memword_read(CPU_BX, pos);
					t >>= 1;
					STOREINTELWORD(p, t);
					t = i286_memword_read(CPU_BX, pos + 2);
					if (!(mem[MEMB_CRT_STS_FLAG] & 0x01)) {		// 25
						t *= (16 * 16);
					}
					else {										// 20
						t *= (20 * 16);
					}
					if (!(mem[MEMB_CRT_STS_FLAG] & 0x80)) {
						t >>= 1;
					}
					STOREINTELWORD(p + 2, t);
					pos += 4;
					p += 4;
				}
			}
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			screenupdate |= 2;
			break;

   		case 0x10:						// カーソルタイプの設定
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);
			bios0x18_10((REG8)(CPU_AL & 1));
 			break;

   		case 0x11:						// カーソルの表示開始
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);
													// 00/08/02
			if (gdc.m.para[GDC_CSRFORM] != (mem[MEMB_CRT_RASTER] | 0x80)) {
				gdc.m.para[GDC_CSRFORM] = mem[MEMB_CRT_RASTER] | 0x80;
			}
			gdcs.textdisp |= GDCSCRN_ALLDRAW | GDCSCRN_EXT;
			break;

   		case 0x12:						// カーソルの表示停止
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);
													// 00/08/02
			if (gdc.m.para[GDC_CSRFORM] != mem[MEMB_CRT_RASTER]) {
				gdc.m.para[GDC_CSRFORM] = mem[MEMB_CRT_RASTER];
				gdcs.textdisp |= GDCSCRN_ALLDRAW | GDCSCRN_EXT;
			}
			break;

   		case 0x13:						// カーソル位置の設定
			// GDCバッファを空に
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			tmp.w = CPU_DX >> 1;
			if (LOADINTELWORD(gdc.m.para + GDC_CSRW) != tmp.w) {
				STOREINTELWORD(gdc.m.para + GDC_CSRW, tmp.w);
				gdcs.textdisp |= GDCSCRN_EXT;
			}
 			break;

   		case 0x14:						// フォントパターンの読み出し
			switch(CPU_DH) {
				case 0x00:			// 8x8
					i286_memword_write(CPU_BX, CPU_CX, 0x0101);
					i286_memstr_write(CPU_BX, CPU_CX + 2,
								fontrom + 0x82000 + (CPU_DL << 4), 8);
					break;

				case 0x28:			// 8x16 KANJI
				case 0x29:
				case 0x2a:
				case 0x2b:
					i286_memword_write(CPU_BX, CPU_CX, 0x0102);
					i286_memstr_write(CPU_BX, CPU_CX + 2,
								fontrom + ((CPU_DL & 0x7f) << 12)
										+ ((CPU_DH - 0x20) << 4), 16);
					break;

				case 0x80:			// 8x16 ANK
					i286_memword_write(CPU_BX, CPU_CX, 0x0102);
					i286_memstr_write(CPU_BX, CPU_CX + 2,
								fontrom + 0x80000 + (CPU_DL << 4), 16);
					break;

				default:
					buf[0] = 0x02;
					buf[1] = 0x02;
					p = fontrom + ((CPU_DL & 0x7f) << 12)
								+ (((CPU_DH - 0x20) & 0x7f) << 4);
					for (i=1; i<17; i++, p++) {
						buf[i*2+0] = *p;
						buf[i*2+1] = *(p+0x800);
					}
					i286_memstr_write(CPU_BX, CPU_CX, buf, 34);
					break;
			}
 			break;

 		case 0x15:						// ライトペン位置読みだし
 			break;

   		case 0x16:						// テキストVRAMの初期化
			bios0x18_16(CPU_DL, CPU_DH);
 			break;

		case 0x17:						// ブザーの起呼
			iocore_out8(0x37, 0x06);
			break;

		case 0x18:						// ブザーの停止
			iocore_out8(0x37, 0x07);
			break;

		case 0x19:						// ライトペン押下状態の初期化
			break;

   		case 0x1a:						// ユーザー文字の定義
			if ((CPU_DH & 0x7e) == 0x76) {
				i286_memstr_read(CPU_BX, CPU_CX + 2, buf, 32);
				p = fontrom + ((CPU_DL & 0x7f) << 12)
							+ (((CPU_DH - 0x20) & 0x7f) << 4);
				for (i=0; i<16; i++, p++) {
					*p = buf[i*2+0];
					*(p+0x800) = buf[i*2+1];
				}
				cgwindow.writable |= 0x80;
			}
			break;

		case 0x1b:						// KCGアクセスモードの設定
			switch(CPU_AL) {
				case 0:
					mem[MEMB_CRT_STS_FLAG] &= ~0x08;
					gdc.mode1 &= ~0x20;
					gdc_restorekacmode();
					break;

				case 1:
					mem[MEMB_CRT_STS_FLAG] |= 0x08;
					gdc.mode1 |= 0x20;
					gdc_restorekacmode();
					break;
			}
			break;

   		case 0x40:						// グラフィック画面の表示開始
			// GDCバッファを空に
			if (gdc.s.cnt) {
				gdc_work(GDCWORK_SLAVE);
			}
			if (!(gdcs.grphdisp & GDCSCRN_ENABLE)) {
				gdcs.grphdisp |= GDCSCRN_ENABLE;
				screenupdate |= 2;
			}
			mem[MEMB_PRXCRT] |= 0x80;
 			break;

   		case 0x41:						// グラフィック画面の表示終了
			// GDCバッファを空に
			if (gdc.s.cnt) {
				gdc_work(GDCWORK_SLAVE);
			}
			gdc_forceready(&gdc.s);
			if (gdcs.grphdisp & GDCSCRN_ENABLE) {
				gdcs.grphdisp &= ~(GDCSCRN_ENABLE);
				screenupdate |= 2;
			}
			mem[MEMB_PRXCRT] &= 0x7f;
 			break;

   		case 0x42:						// 表示領域の設定
			// GDCバッファを空に
			if (gdc.s.cnt) {
				gdc_work(GDCWORK_SLAVE);
			}
			gdc_forceready(&gdc.s);

			ZeroMemory(&gdc.s.para[GDC_SCROLL], 8);
			if ((CPU_CH & 0xc0) == 0xc0) {		// ALL
				tmp.b = FALSE;
				if ((mem[MEMB_PRXDUPD] & 0x24) == 0x20) {
					mem[MEMB_PRXDUPD] ^= 4;
					gdc.clock |= 3;
					CopyMemory(gdc.s.para + GDC_SYNC, sync400m, 8);
					gdc.s.para[GDC_PITCH] = 80;
					gdcs.grphdisp |= GDCSCRN_EXT;
					mem[MEMB_PRXDUPD] |= 0x08;
				}
			}
			else {
				tmp.b = TRUE;
				if ((mem[MEMB_PRXDUPD] & 0x24) == 0x24) {
					mem[MEMB_PRXDUPD] ^= 4;
					gdc.clock &= ~3;
					CopyMemory(gdc.s.para + GDC_SYNC,
						(mem[MEMB_PRXCRT] & 0x40)?sync200m:sync200l, 8);
					gdc.s.para[GDC_PITCH] = 40;
					gdcs.grphdisp |= GDCSCRN_EXT;
					mem[MEMB_PRXDUPD] |= 0x08;
				}
				if (CPU_CH & 0x40) {			// UPPER
					gdc.s.para[GDC_SCROLL+0] = (200*40) & 0xff;
					gdc.s.para[GDC_SCROLL+1] = (200*40) >> 8;
				}
			}
			if ((!tmp.b) || (!(mem[MEMB_PRXCRT] & 0x40))) {
				gdc.mode1 &= ~(0x10);
				gdc.s.para[GDC_CSRFORM] = 0;
			}
			else {
				gdc.mode1 |= 0x10;
				gdc.s.para[GDC_CSRFORM] = 1;
			}
			gdcs.disp = (CPU_CH >> 4) & 1;
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			screenupdate |= 2;
 			break;

		case 0x43:						// パレットの設定
			i286_memstr_read(CPU_DS, CPU_BX + offsetof(UCWTBL, GBCPC),
																	buf, 4);
			tmp.d = LOADINTELDWORD(buf);
			for (i=8; i--;) {
				gdc_setdegitalpal(i, (REG8)(tmp.d & 15));
				tmp.d >>= 4;
			}
			break;

		case 0x44:						// ボーダカラーの設定
//			if (!(mem[MEMB_PRXCRT] & 0x40)) {
//				color = i286_membyte_read(CPU_DS, CPU_BX + 1);
//			}
			break;

		case 0x47:						// 直線、矩形の描画
		case 0x48:						// 円の描画
			bios18_47();
			break;

		case 0x49:						// グラフィック文字の描画
			bios18_49();
			break;

		case 0x4a:						// 描画モードの設定
			if (!(mem[MEMB_PRXCRT] & 0x01)) {
				gdc.s.para[GDC_SYNC] = CPU_CH;
				gdcs.grphdisp |= GDCSCRN_EXT;
				if (CPU_CH & 0x10) {
					mem[MEMB_PRXDUPD] &= ~0x08;
				}
				else {
					mem[MEMB_PRXDUPD] |= 0x08;
				}
			}
			break;

	}
}

