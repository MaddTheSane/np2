#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"rsbios.h"


void bios0x0c(void) {

	UINT16	doff;
	UINT16	dseg;
	BYTE	flag;
	BYTE	data;
	BYTE	stat;
	UINT16	pos;
	UINT16	cnt;

	doff = GETBIOSMEM16(MEMW_RS_CH0_OFST);
	dseg = GETBIOSMEM16(MEMW_RS_CH0_SEG);

	flag = i286_membyte_read(dseg, doff + R_FLAG);
	data = iocore_inp8(0x30);							// �f�[�^�������
	stat = iocore_inp8(0x32) & 0xfc;					// �X�e�[�^�X
	stat |= (iocore_inp8(0x33) & 3);

#if 0
	if (stat & 0x38) {
		iocore_out8(0x32, flag | 0x10);
	}
#endif

	if (!(flag & RFLAG_BFULL)) {
		// SI/SO�ϊ�
		if (mem[MEMB_RS_S_FLAG] & 0x80) {
			if (data >= 0x20) {
				if (mem[MEMB_RS_S_FLAG] & 0x10) {
					data |= 0x80;
				}
				else {
					data &= 0x7f;
				}
			}
			else if (data == RSCODE_SO) {
				mem[MEMB_RS_S_FLAG] |= 0x10;
				iocore_out8(0x00, 0x20);
				return;
			}
			else if (data == RSCODE_SI) {
				mem[MEMB_RS_S_FLAG] &= ~0x10;
				iocore_out8(0x00, 0x20);
				return;
			}
		}

		// DEL�R�[�h�̈���
		if (mem[MEMB_RS_D_FLAG] & 0x01) {					// CH0 -> bit0
			if (((data & 0x7f) == 0x7f) && (mem[MEMB_MSW3] & 0x80)) {
				data = 0;
			}
		}
		// �f�[�^����
		pos = i286_memword_read(dseg, doff + R_PUTP);
		i286_memword_write(dseg, pos, (UINT16)((data << 8) | stat));

		// ���̃|�C���^���X�g�A
		pos += 2;
		if (pos >= i286_memword_read(dseg, doff + R_TAILP)) {
			pos = i286_memword_read(dseg, doff + R_HEADP);
		}
		i286_memword_write(dseg, doff + R_PUTP, pos);

		// �J�E���^�̃C���N�������g
		cnt = i286_memword_read(dseg, doff + R_CNT) + 1;
		i286_memword_write(dseg, doff + R_CNT, cnt);

		// �I�[�o�[�t���[��������
		if (pos == i286_memword_read(dseg, doff + R_GETP)) {
			flag |= RFLAG_BFULL;
		}

		// XOFF�𑗐M�H
		if (((flag & (RFLAG_XON | RFLAG_XOFF)) == RFLAG_XON) &&
			(cnt >= i286_memword_read(dseg, doff + R_XON))) {
			iocore_out8(0x30, RSCODE_XOFF);
			flag |= RFLAG_XOFF;
		}
	}
	else {
		i286_membyte_write(dseg, doff + R_CMD,
				(BYTE)(i286_membyte_read(dseg, doff + R_CMD) | RFLAG_BOVF));
	}
	i286_membyte_write(dseg, doff + R_INT,
				(BYTE)(i286_membyte_read(dseg, doff + R_INT) | RINT_INT));
	i286_membyte_write(dseg, doff + R_FLAG, flag);
	iocore_out8(0x00, 0x20);
}

