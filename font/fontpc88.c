#include	"compiler.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"font.h"
#include	"fontdata.h"


static void pc88knjcpy1(BYTE *dst, const BYTE *src, int from, int to) {

	int		i;
	int		j;
	int		k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		q = dst + 0x21000 + (i << 4);
		for (j=0x21; j<0x7f; j++) {
			p = NULL;
			// BITMAP��̊����̃|�C���^�����߂�
								// 2121�`277e
			if ((i >= 0x01) && (i <= 0x07)) {
				p = src + ((j & 0x1f) * 0x20) + (i * 0x400)
													+ ((j & 0x60) * 0x100);
			}
								// 3021�`4f7e
			else if ((i >= 0x10) && (i <= 0x2f)) {
				p = src + ((j & 0x1f) * 0x20) + ((i & 0x7) * 0x400)
						+ ((((i - 0x10) / 0x8) ^ 2) * 0x2000)
						+ (((j & 0x60) / 0x20) * 0x8000);
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

static void pc88knjcpy2(BYTE *dst, const BYTE *src, int from, int to) {

	int		i, j, k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		q = dst + 0x21000 + (i << 4);
		for (j=0x21; j<0x7f; j++) {
			p = NULL;
			// �����̃|�C���^�����߂�
								// 7021�`737e
			if ((i >= 0x50) && (i <= 0x53)) {
				p = src + ((j & 0x1f) * 0x20)
							+ ((i - 0x50) * 0x400)
							+ ((j & 0x60) * 0x100);
			}
								// 5021�`6f7e
			else if ((i >= 0x30) && (i <= 0x4f)) {
				p = src + ((j & 0x1f) * 0x20)
					+ ((i & 0x7) * 0x400)
					+ (((i - 0x30) / 0x8) * 0x2000)
					+ (((j & 0x60) / 0x20) * 0x8000);
			}
			if (p) {							// �K�i���R�[�h�Ȃ��
				// �R�s�[����
				for (k=0; k<16; k++) {
					*(q+k) = *p++;
					*(q+k+0x800) = *p++;
				}
			}
			q += 0x1000;
		}
	}
}

BYTE fontpc88_read(const char *filename, BYTE loading) {

	FILEH	fh;
	BYTE	*work;
	char	fname[MAX_PATH];

	work = (BYTE *)_MALLOC(0x20000, "pc88font");
	if (work == NULL) {
		goto fr88_err1;
	}
	file_cpyname(fname, filename, sizeof(fname));

	// ��Q�����ȊO��ǂޕK�v�͂���H
	if (loading & (FONT_ANK8 | FONTLOAD_ANK | FONT_KNJ1)) {

		// ��������ǂݍ���ł݂�
		file_cutname(fname);
		file_catname(fname, pc88knj1name, sizeof(fname));
		fh = file_open_rb(fname);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 0x20000) == 0x20000) {

				// 8dot ANK��ǂޕK�v�����邩
				if (loading & FONT_ANK8) {
					loading &= ~FONT_ANK8;
					CopyMemory(fontrom + 0x82000, work + 0x1000, 8*256);
				}

				// 16dot ASCII��ǂޕK�v�����邩
				if (loading & FONT_ANK16a) {
					loading &= ~FONT_ANK16a;
					CopyMemory(fontrom + 0x80000, work + 0x0000, 16*128);
				}

				// 16dot ANK(0x80�`)��ǂޕK�v�����邩
				if (loading & FONT_ANK16b) {
					loading &= ~FONT_ANK16b;
					CopyMemory(fontrom + 0x80800, work + 0x0800, 16*128);
				}

				// ��ꐅ��������ǂݍ��ށH
				if (loading & FONT_KNJ1) {
					loading &= ~FONT_KNJ1;
					pc88knjcpy1(fontrom, work, 0x01, 0x30);
					fontdata_patchjis();
				}
			}

			// �N���[�Y���� �Z�N�V�����I���
			file_close(fh);
		}
	}

	// ��Q������ǂޕK�v�͂���H
	if (loading & FONT_KNJ2) {

		// ��������ǂݍ���ł݂�
		file_cutname(fname);
		file_catname(fname, pc88knj2name, sizeof(fname));
		fh = file_open_rb(fname);
		if (fh != FILEH_INVALID) {
			if (file_read(fh, work, 0x20000) == 0x20000) {

				loading &= ~FONT_KNJ2;
				pc88knjcpy2(fontrom, work, 0x31, 0x56);
			}

			// �N���[�Y���� �Z�N�V�����I���
			file_close(fh);
		}
	}

	// ANK��ǂݍ��ޕK�v�͂���H
	if (loading & (FONT_ANK8 | FONTLOAD_ANK)) {

		// ��������ǂݍ���ł݂�
		file_cutname(fname);
		file_catname(fname, pc88ankname, sizeof(fname));
		fh = file_open_rb(fname);
		if (fh != FILEH_INVALID) {

			// �ǂݍ���ł݂�
			if (file_read(fh, work, 0x1800) == 0x1800) {

				// 8dot ANK��ǂޕK�v�����邩
				if (loading & FONT_ANK8) {
					loading &= ~FONT_ANK8;
					CopyMemory(fontrom + 0x82000, work + 0x0000, 8*256);
				}

				// 16dot ASCII��ǂޕK�v�����邩
				if (loading & FONT_ANK16a) {
					loading &= ~FONT_ANK16a;
					CopyMemory(fontrom + 0x80000, work + 0x0800, 16*128);
				}

				// 16dot ANK(0x80�`)��ǂޕK�v�����邩
				if (loading & FONT_ANK16b) {
					loading &= ~FONT_ANK16b;
					CopyMemory(fontrom + 0x80800, work + 0x1000, 16*128);
				}
			}

			// �N���[�Y���� ANK�͏I���
			file_close(fh);
		}
	}

	_MFREE(work);

fr88_err1:
	return(loading);
}

