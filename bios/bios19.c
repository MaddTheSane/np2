#include	"compiler.h"
#include	"cpucore.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"rsbios.h"


static const UINT rs_speed[] = {
						// 5MHz
						0x0800, 0x0400, 0x0200, 0x0100,
						0x0080, 0x0040, 0x0020, 0x0010,
						0x0008, 0x0004, 0x0002, 0x0001,
						// 4MHz
						0x0680, 0x0340, 0x01a0, 0x00d0,
						0x0068, 0x0034, 0x001a, 0x000d};


void bios0x19(void) {

	BYTE	speed;
	BYTE	mode;
	RSBIOS	rsb;
	UINT16	doff;
	UINT16	cnt;
	UINT16	dseg;
	BYTE	flag;

	if (CPU_AH < 2) {
		// �ʐM���x�c
		mode = CPU_CH | 0x02;
		speed = CPU_AL;
		if (speed >= 8) {
			speed = 4;						// 1200bps
		}
		if (mem[MEMB_BIOS_FLAG] & 0x80) {	// 4MHz?
			speed += 12;
		}

#if 1	// NP2�ł͖��T�|�[�g�ׁ̈@���s(��
		mode &= ~1;
#else
		if (mode & 1) {
			if (speed < (12 + 6)) {
				speed += 2;
			}
			else {
				mode &= ~1;
			}
		}
		// �V���A�����Z�b�g
		iocore_out8(0x32, 0x00);		// dummy instruction
		iocore_out8(0x32, 0x00);		// dummy instruction
		iocore_out8(0x32, 0x00);		// dummy instruction
		iocore_out8(0x32, 0x40);		// reset
		iocore_out8(0x32, mode);		// mode
		iocore_out8(0x32, CPU_CL);	// cmd
#endif
		iocore_out8(0x77, 0xb6);
		iocore_out8(0x75, (BYTE)rs_speed[speed]);
		iocore_out8(0x75, (BYTE)(rs_speed[speed] >> 8));

		ZeroMemory(&rsb, sizeof(rsb));
		rsb.FLAG = (CPU_AH << 4);
		rsb.CMD = CPU_CL;
		sysport.c &= ~7;
		if (!(CPU_CL & RCMD_IR)) {
			rsb.FLAG |= RFLAG_INIT;
			if (CPU_CL & RCMD_RXE) {
				sysport.c |= 1;
				pic.pi[0].imr &= ~PIC_RS232C;
			}
		}

		rsb.STIME = CPU_BH;
		if (!rsb.STIME) {
			rsb.STIME = 0x04;
		}
		rsb.RTIME = CPU_BL;
		if (!rsb.RTIME) {
			rsb.RTIME = 0x40;
		}
		doff = CPU_DI + sizeof(RSBIOS);
		STOREINTELWORD(rsb.HEADP, doff);
		STOREINTELWORD(rsb.PUTP, doff);
		STOREINTELWORD(rsb.GETP, doff);
		doff += CPU_DX;
		STOREINTELWORD(rsb.TAILP, doff);
		cnt = CPU_DX >> 3;
		STOREINTELWORD(rsb.XOFF, cnt);
		cnt += CPU_DX >> 2;
		STOREINTELWORD(rsb.XON, cnt);

		// �|�C���^�`
		SETBIOSMEM16(MEMW_RS_CH0_OFST, CPU_DI);
		SETBIOSMEM16(MEMW_RS_CH0_SEG, CPU_ES);
		i286_memstr_write(CPU_ES, CPU_DI, &rsb, sizeof(rsb));

		CPU_AH = 0;
	}
	else if (CPU_AH < 7) {
		doff = GETBIOSMEM16(MEMW_RS_CH0_OFST);
		dseg = GETBIOSMEM16(MEMW_RS_CH0_SEG);
		if ((!doff) && (!dseg)) {
			CPU_AH = 1;
			return;
		}
		flag = i286_membyte_read(dseg, doff + R_FLAG);
		if (!(flag & RFLAG_INIT)) {
			CPU_AH = 1;
			return;
		}
		switch(CPU_AH) {
			case 0x02:
				CPU_CX = i286_memword_read(dseg, doff + R_CNT);
				break;

			case 0x03:
				iocore_out8(0x30, CPU_AL);
				break;

			case 0x04:
				cnt = i286_memword_read(dseg, doff + R_CNT);
				if (cnt) {
					UINT16	pos;

					// �f�[�^�������
					pos = i286_memword_read(dseg, doff + R_GETP);
					CPU_CX = i286_memword_read(dseg, pos);

					// ���̃|�C���^���X�g�A
					pos += 2;
					if (pos >= i286_memword_read(dseg, doff + R_TAILP)) {
						pos = i286_memword_read(dseg, doff + R_HEADP);
					}
					i286_memword_write(dseg, doff + R_GETP, pos);

					// �J�E���^���f�N�������g
					cnt--;
					i286_memword_write(dseg, doff + R_CNT, cnt);

					// XON�𑗐M�H
					if ((flag & RFLAG_XOFF) && 
						(cnt < i286_memword_read(dseg, doff + R_XOFF))) {
						iocore_out8(0x30, RSCODE_XON);
						flag &= ~RFLAG_XOFF;
					}
					flag &= ~RFLAG_BOVF;
					CPU_AH = 0;
					i286_membyte_write(dseg, doff + R_FLAG, flag);
					return;
				}
				else {
					CPU_AH = 3;
				}
				break;

			case 0x05:
				iocore_out8(0x32, CPU_AL);
				if (CPU_AL & RCMD_IR) {
					flag &= ~RFLAG_INIT;
					i286_membyte_write(dseg, doff + R_FLAG, flag);
					sysport.c &= ~1;
					pic.pi[0].imr |= PIC_RS232C;
				}
				else if (!(CPU_AL & RCMD_RXE)) {
					sysport.c &= ~1;
					pic.pi[0].imr |= PIC_RS232C;
				}
				else {
					sysport.c |= 1;
					pic.pi[0].imr &= ~PIC_RS232C;
				}
				i286_membyte_write(dseg, doff + R_CMD, CPU_AL);
				break;
			case 0x06:
				CPU_CH = iocore_inp8(0x32);
				CPU_CL = iocore_inp8(0x33);
				break;
		}
		CPU_AH = 0;
		if (flag & RFLAG_BOVF) {
			i286_membyte_write(dseg, doff + R_FLAG,
											(BYTE)(flag & (~RFLAG_BOVF)));
			CPU_AH = 2;
		}
	}
	else {
		CPU_AH = 0;
	}
}

