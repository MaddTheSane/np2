#include	"compiler.h"
#include	"cpucore.h"
#include	"memory.h"
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

#if 0
typedef struct {
	BYTE	raster;
	BYTE	cfi;
	BYTE	pl;
	BYTE	bl;
	BYTE	cl;
	BYTE	ssl;
	BYTE	padding[2];
} CRTDATA;

static const CRTDATA crtdata[] = {
						{0x07, 0x3b,	0x00, 0x07, 0x08, 0x00},
						{0x09, 0x4b,	0x1f, 0x08, 0x08, 0x00},
						{0x0f, 0x7b,	0x00, 0x0f, 0x10, 0x00},
						{0x13, 0x9b,	0x1e, 0x11, 0x10, 0x00}};
#endif


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
	UINT16		GBMDOTI;

	// GDC�o�b�t�@�����
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
		short dx, dy;
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
		STOREINTELWORD(vect.DC, dx);
		data = dy * 2;
		STOREINTELWORD(vect.D1, data);
		data -= dx;
		STOREINTELWORD(vect.D, data);
		data -= dx;
		STOREINTELWORD(vect.D2, data);
	}
	else if (ucw.GBDTYP == 0x02) {
		func = gdcsub_box;
		if (GBSX1 > GBSX2) {
			SWAPU16(GBSX1, GBSX2);
		}
		if (GBSY1 > GBSY2) {
			SWAPU16(GBSY1, GBSY2);
		}
		STOREINTELWORD(vect.DC, 3);
		STOREINTELWORD(vect.D1, 0xffff);
		data = GBSX2 - GBSX1;
		STOREINTELWORD(vect.DM, data);
		STOREINTELWORD(vect.D2, data);
		data = GBSY2 - GBSY1;
		STOREINTELWORD(vect.D, data);
	}
	else {
		return;
	}
	if ((CPU_CH & 0xc0) == 0x40) {
		GBSY1 += 200;
	}
	csrw = (GBSY1 * 40) + (GBSX1 >> 4);
	csrw += (GBSX1 & 0xf) << 20;
	GBMDOTI = LOADINTELWORD(ucw.GBMDOTI);
	if ((CPU_CH & 0x30) == 0x30) {
		if (ucw.GBON_PTN & 1) {
			func(0x04000 + csrw, &vect, GBMDOTI, GDCOPE_SET);
		}
		else {
			func(0x04000 + csrw, &vect, GBMDOTI, GDCOPE_CLEAR);
		}
		if (ucw.GBON_PTN & 2) {
			func(0x08000 + csrw, &vect, GBMDOTI, GDCOPE_SET);
		}
		else {
			func(0x08000 + csrw, &vect, GBMDOTI, GDCOPE_CLEAR);
		}
		if (ucw.GBON_PTN & 4) {
			func(0x0c000 + csrw, &vect, GBMDOTI, GDCOPE_SET);
		}
		else {
			func(0x0c000 + csrw, &vect, GBMDOTI, GDCOPE_CLEAR);
		}
	}
	else {
		func(csrw + 0x4000 + ((CPU_CH & 0x30) << 10), &vect,
														GBMDOTI, ucw.GBDOTU);
	}
}

void bios0x18(void) {

	UINT	pos;
	BYTE	buf[34];
	BYTE	*p;
	int		i;
	UINT16	tmp;
	UINT32	pal;

//	TRACE_("int18", CPU_AH);

	sti_waiting ^= 1;
	if (sti_waiting) {					// �����݋��̗V��
		CPU_STI;
		if (PICEXISTINTR) {
			CPU_IP--;
			nevent_forceexit();
			return;
		}
	}
	sti_waiting = 0;

	switch(CPU_AH) {
		case 0x00:						// �L�[�E�f�[�^�̓ǂ݂���
			if (mem[MEMB_KB_COUNT]) {
				CPU_AX = keyget();
			}
			else {
				CPU_IP--;
				CPU_REMCLOCK = -1;
				break;
			}
			break;

   		case 0x01:						// �L�[�E�o�b�t�@��Ԃ̃Z���X
			if (mem[MEMB_KB_COUNT]) {
				pos = GETBIOSMEM16(MEMW_KB_BUF_HEAD);
				CPU_AX = GETBIOSMEM16(pos);
				CPU_BH = 1;
			}
			else {
				CPU_BH = 0;
			}
			break;

   		case 0x02:						// �V�t�g�E�L�[��Ԃ̃Z���X
			CPU_AL = mem[MEMB_SHIFT_STS];
			break;

   		case 0x03:						// �L�[�{�[�h�E�C���^�t�F�C�X�̏�����
			bios0x09_init();
			break;

   		case 0x04:						// �L�[���͏�Ԃ̃Z���X
			CPU_AH = mem[0x00052a + (CPU_AL & 0x0f)];
 			break;

   		case 0x05:						// �L�[���̓Z���X
			if (mem[MEMB_KB_COUNT]) {
				CPU_AX = keyget();
				CPU_BH = 1;
			}
			else {
				CPU_BH = 0;
			}
 			break;

   		case 0x0a:						// CRT���[�h�̐ݒ�
#if 1
			mem[MEMB_CRT_STS_FLAG] = 0x80 | (CPU_AL & 0x0f);
			// GDC�o�b�t�@�����
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			gdc.mode1 &= ~(0x25);
			gdc.mode1 |= 0x08;
			if (CPU_AL & 0x02) {
				gdc.mode1 |= 0x04;				// 40��
			}
			if (CPU_AL & 0x04) {
				gdc.mode1 |= 0x01;				// �A�g���r���[�g
			}
			if (CPU_AL & 0x08) {
				gdc.mode1 |= 0x20;				// �R�[�h�A�N�Z�X
			}
			if (CPU_AL & 0x01) {					// 20�s
				mem[MEMB_CRT_RASTER] = 0x13;
				gdc.m.para[GDC_CSRFORM + 0] = 0x13;
				gdc.m.para[GDC_CSRFORM + 1] = 0x00;
				gdc.m.para[GDC_CSRFORM + 2] = 0x9b;
				crtc.reg.pl = 0x1e;
				crtc.reg.bl = 0x11;
			}
			else {									// 25�s
				mem[MEMB_CRT_RASTER] = 0x0f;
				gdc.m.para[GDC_CSRFORM + 0] = 0x0f;
				gdc.m.para[GDC_CSRFORM + 1] = 0x00;
				gdc.m.para[GDC_CSRFORM + 2] = 0x7b;
				crtc.reg.pl = 0x00;
				crtc.reg.bl = 0x0f;
			}
			crtc.reg.cl = 0x10;
			crtc.reg.ssl = 0x00;
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			gdc_restorekacmode();
			break;
#else
			// GDC�o�b�t�@�����
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			mem[MEMB_CRT_STS_FLAG] = CPU_AL;
			if (systemport_r(0x33) & 0x08) {
				mem[MEMB_CRT_STS_FLAG] |= 0x80;
			}
#endif

   		case 0x0b:						// CRT���[�h�̃Z���X
			CPU_AL = mem[MEMB_CRT_STS_FLAG];
 			break;

   		case 0x0c:						// �e�L�X�g��ʂ̕\���J�n
			if (!(gdcs.textdisp & GDCSCRN_ENABLE)) {
				gdcs.textdisp |= GDCSCRN_ENABLE;
				screenupdate |= 2;
 			}
 			break;

   		case 0x0d:						// �e�L�X�g��ʂ̕\���I��
			if (gdcs.textdisp & GDCSCRN_ENABLE) {
				gdcs.textdisp &= ~(GDCSCRN_ENABLE);
				screenupdate |= 2;
			}
 			break;

		case 0x0e:						// ��̕\���̈�̐ݒ�
			// GDC�o�b�t�@�����
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			ZeroMemory(&gdc.m.para[GDC_SCROLL], 16);
			tmp = CPU_DX >> 1;
			STOREINTELWORD(gdc.m.para + GDC_SCROLL, tmp);
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			screenupdate |= 2;
 			break;

		case 0x0f:						// �����̕\���̈�̐ݒ�
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
					if (!(mem[MEMB_CRT_STS_FLAG] & 1)) {	// 25
						t *= (16 * 16);
					}
					else {									// 20
						t *= (20 * 16);
					}
					if (!(mem[MEMB_CRT_STS_FLAG] & 0x80)) {			// ver0.29
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

   		case 0x10:						// �J�[�\���^�C�v�̐ݒ�
			// GDC�o�b�t�@�����
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			gdc.m.para[GDC_CSRFORM + 0] &= 0x7f;
			gdc.m.para[GDC_CSRFORM + 1] &= 0xdf;
			gdc.m.para[GDC_CSRFORM + 1] |= (CPU_AL & 1) << 5;
			gdcs.textdisp |= GDCSCRN_EXT;
 			break;

   		case 0x11:						// �J�[�\���̕\���J�n
			// GDC�o�b�t�@�����
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

   		case 0x12:						// �J�[�\���̕\����~
			// GDC�o�b�t�@�����
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

   		case 0x13:						// �J�[�\���ʒu�̐ݒ�
			// GDC�o�b�t�@�����
			if (gdc.m.cnt) {
				gdc_work(GDCWORK_MASTER);
			}
			gdc_forceready(&gdc.m);

			tmp = CPU_DX >> 1;
			if (LOADINTELWORD(gdc.m.para + GDC_CSRW) != tmp) {
				STOREINTELWORD(gdc.m.para + GDC_CSRW, tmp);
				gdcs.textdisp |= GDCSCRN_EXT;
			}
 			break;

   		case 0x14:						// �t�H���g�p�^�[���̓ǂݏo��
			switch(CPU_DH) {
				case 0x00:			// 8x8
					i286_memword_write(CPU_BX, CPU_CX, 0x0101);
					i286_memstr_write(CPU_BX, CPU_CX + 2,
								fontrom + 0x82000 + (CPU_DL << 3), 8);
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

 		case 0x15:						// ���C�g�y���ʒu�ǂ݂���
 			break;

   		case 0x16:						// �e�L�X�gVRAM�̏�����
			bios0x18_16(CPU_DL, CPU_DH);
 			break;

		case 0x17:						// �u�U�[�̋N��
			iocore_out8(0x37, 0x06);
			break;

		case 0x18:						// �u�U�[�̒�~
			iocore_out8(0x37, 0x07);
			break;

		case 0x19:						// ���C�g�y��������Ԃ̏�����
			break;

   		case 0x1a:						// ���[�U�[�����̒�`
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

		case 0x1b:						// KCG�A�N�Z�X���[�h�̐ݒ�
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

   		case 0x40:						// �O���t�B�b�N��ʂ̕\���J�n
			if (!(gdcs.grphdisp & GDCSCRN_ENABLE)) {
				gdcs.grphdisp |= GDCSCRN_ENABLE;
				screenupdate |= 2;
			}
 			break;

   		case 0x41:						// �O���t�B�b�N��ʂ̕\���I��
			if (gdcs.grphdisp & GDCSCRN_ENABLE) {
				gdcs.grphdisp &= ~(GDCSCRN_ENABLE);
				screenupdate |= 2;
			}
 			break;

   		case 0x42:						// �\���̈�̐ݒ�
			// GDC�o�b�t�@�����
			if (gdc.s.cnt) {
				gdc_work(GDCWORK_SLAVE);
			}
			gdc_forceready(&gdc.s);

			ZeroMemory(&gdc.s.para[GDC_SCROLL], 8);
			switch(CPU_CH & 0xc0) {
				case 0x40:					// UPPER
					if ((mem[MEMB_PRXDUPD] & 0x24) == 0x24) {
						mem[MEMB_PRXDUPD] ^= 4;
						gdc.clock &= ~3;
						gdc.s.para[GDC_PITCH] = 40;
						gdcs.grphdisp |= GDCSCRN_EXT;
					}
					gdc.mode1 |= 0x10;
					gdc.s.para[GDC_CSRFORM] = 1;
					gdc.s.para[GDC_SCROLL+0] = (200*40) & 0xff;
					gdc.s.para[GDC_SCROLL+1] = (200*40) >> 8;
					break;

				case 0x80:					// LOWER
					if ((mem[MEMB_PRXDUPD] & 0x24) == 0x24) {
						mem[MEMB_PRXDUPD] ^= 4;
						gdc.clock &= ~3;
						gdc.s.para[GDC_PITCH] = 40;
						gdcs.grphdisp |= GDCSCRN_EXT;
					}
					gdc.mode1 |= 0x10;
					gdc.s.para[GDC_CSRFORM] = 1;
					break;

				default:					// ALL
					if ((mem[MEMB_PRXDUPD] & 0x24) == 0x20) {
						mem[MEMB_PRXDUPD] ^= 4;
						gdc.clock |= 3;
						gdc.s.para[GDC_PITCH] = 80;
						gdcs.grphdisp |= GDCSCRN_EXT;
					}
					gdc.mode1 &= ~(0x10);
					gdc.s.para[GDC_CSRFORM] = 0;
					break;
			}
			gdcs.disp = (CPU_CH >> 4) & 1;
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			screenupdate |= 2;
 			break;

		case 0x43:						// �p���b�g�̐ݒ�
			i286_memstr_read(CPU_DS, CPU_BX + offsetof(UCWTBL, GBCPC),
																	buf, 4);
			pal = LOADINTELDWORD(buf);
			for (i=8; i--;) {
				gdc_setdegitalpal(i, (BYTE)(pal & 15));
				pal >>= 4;
			}
			break;

		case 0x47:						// �����A��`�̕`��
			bios18_47();
			break;
	}
}

