#include	"compiler.h"
#include	"dosio.h"
#include	"memory.h"
#include	"font.h"
#include	"fontdata.h"


static void fm7ankcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		y;
const BYTE	*p;
	int		ank;

	for (ank=from; ank<to; ank++) {
		// ANK�t�H���g�̃X�^�[�g�ʒu
		p = src + 0x6000 + ((ank & 0x1f) * 0x20)
								+ (((ank / 0x20) - 1) * 0x400);
		for (y=0; y<16; y++) {
			*dst++ = *p;
			p += 2;
		}
	}
}

static void fm7knjcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		i, j, k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		q = dst + 0x21000 + (i << 4);
		for (j=0x21; j<0x7f; j++) {
			p = NULL;
			// �����̃|�C���^�����߂�
			if ((i >= 0x01) && (i < 0x08)) {			// 2121�`277e
				p = src + ((j & 0x1f) * 0x20) + (i * 0x400);

				if (j >= 0x60) {
					p += 0x2000;
				}
				else if (j >= 0x40) {
					p += 0x4000;
				}
			}
			else if ((i >= 0x10) && (i < 0x20)) {		// 3021�`3f7e
				p = src + 0x08000 + ((j & 0x1f) * 0x20)
							+ ((i - 0x10) * 0x400)
							+ (((j / 0x20) - 1) * 0x4000);
			}
			else if ((i >= 0x20) && (i < 0x50)) {		// 4021�`4f7e
				p = src + 0x14000 + ((j & 0x1f) * 0x20)
							+ ((i - 0x20) * 0x400)
							+ (((j / 0x20) - 1) * 0x4000);
			}
			if (p) {							// �K�i���R�[�h�Ȃ��
				// �R�s�[����
				for (k=0; k<16; k++) {
					*(q+k+0x000) = p[0];
					*(q+k+0x800) = p[1];
					p += 2;
				}
			}
			q += 0x1000;
		}
	}
}

BYTE fontfm7_read(const char *filename, BYTE loading) {

	FILEH	fh;
	BYTE	*work;
	char	fname[MAX_PATH];

	work = (BYTE *)_MALLOC(0x20000, "fm7font");
	if (work == NULL) {
		goto frf7_err1;
	}
	file_cpyname(fname, filename, sizeof(fname));

	// 8dot ANK��ǂݍ��ޕK�v�͂���H
	if (loading & FONT_ANK8) {
		file_cutname(fname);
		file_catname(fname, fm7ankname, sizeof(fname));
		fh = file_open_rb(fname);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 2048) == 2048) {
				loading &= ~FONT_ANK8;
				CopyMemory(fontrom + 0x82100, work + 0x100, 0x60*8);
				CopyMemory(fontrom + 0x82500, work + 0x500, 0x40*8);
			}
			file_close(fh);
		}
	}

	// 16dot ASCII �y�� ������ǂݍ��ޕK�v�͂��邩�H
	if (loading & (FONT_ANK16a | FONT_KNJ1)) {
		file_cutname(fname);
		file_catname(fname, fm7knjname, sizeof(fname));
		fh = file_open_rb(fname);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 0x20000) == 0x20000) {

				// 16dot ASCII��ǂݍ��ށH
				if (loading & FONT_ANK16a) {
					loading &= ~FONT_ANK16a;
					fm7ankcpy(fontrom + 0x80200, work, 0x20, 0x7f);
					fontdata_patch16a();
				}

				// ��ꐅ��������ǂݍ��ށH
				if (loading & FONT_KNJ1) {
					loading &= ~FONT_KNJ1;
					fm7knjcpy(fontrom, work, 0x01, 0x30);
					fontdata_patchjis();
				}
			}
			file_close(fh);
		}
	}
	_MFREE(work);

frf7_err1:
	return(loading);
}

