/**
 * @file	bios1b.c
 * @brief	Implementation of DISK-BIOS
 */

#include "compiler.h"
#include "bios.h"
#include "biosmem.h"
#include "diskbios.h"
#include "sxsibios.h"
#include "cpucore.h"
#include "pccore.h"
#include "io/iocore.h"
#include "cbus/scsicmd.h"
#include "fdd/fddfile.h"
#include "fdd/fdd_mtr.h"
#include "fdd/sxsi.h"

enum
{
	CACHE_TABLES	= 4,
	CACHE_BUFFER	= 32768
};


/* ---- FDD */

static BRESULT setfdcmode(REG8 drv, REG8 type, REG8 rpm)
{
	if (drv >= 4)
	{
		return FAILURE;
	}
	if ((rpm) && (!g_fdc.support144))
	{
		return FAILURE;
	}
	if ((g_fdc.chgreg ^ type) & 1)
	{
		return FAILURE;
	}
	g_fdc.chgreg = type;
	g_fdc.rpm[drv] = rpm;
	if (type & 2)
	{
		CTRL_FDMEDIA = DISKTYPE_2HD;
	}
	else
	{
		CTRL_FDMEDIA = DISKTYPE_2DD;
	}
	return SUCCESS;
}

void fddbios_equip(REG8 type, BOOL clear)
{
	REG16 diskequip;

	diskequip = GETBIOSMEM16(MEMW_DISK_EQUIP);
	if (clear)
	{
		diskequip &= 0x0f00;
	}
	if (type & 1)
	{
		diskequip &= 0xfff0;
		diskequip |= (g_fdc.equip & 0x0f);
	}
	else
	{
		diskequip &= 0x0fff;
		diskequip |= (g_fdc.equip & 0x0f) << 12;
	}
	SETBIOSMEM16(MEMW_DISK_EQUIP, diskequip);
}

static void biosfd_setchrn(void)
{
	g_fdc.C = CPU_CL;
	g_fdc.H = CPU_DH;
	g_fdc.R = CPU_DL;
	g_fdc.N = CPU_CH;
}

#if 0
static void biosfd_resultout(UINT32 result)
{
	UINT8	*ptr;

	ptr = mem + 0x00564 + (g_fdc.us * 8);
	ptr[0] = (UINT8)(result & 0xff) | (g_fdc.hd << 2) | g_fdc.us;
	ptr[1] = (UINT8)(result >> 8);
	ptr[2] = (UINT8)(result >> 16);
	ptr[3] = g_fdc.C;
	ptr[4] = g_fdc.H;
	ptr[5] = g_fdc.R;
	ptr[6] = g_fdc.N;
	ptr[7] = g_fdc.ncn;
}
#endif

static BRESULT biosfd_seek(REG8 track, BOOL ndensity)
{
	if (ndensity)
	{
		if (track < 42)
		{
			track <<= 1;
		}
		else
		{
			track = 42 * 2;
		}
	}
	g_fdc.ncn = track;
	if (fdd_seek())
	{
		return FAILURE;
	}
	return SUCCESS;
}

static UINT16 fdfmt_biospara(REG8 type, REG8 rpm, REG8 fmt)
{
	UINT	seg;
	UINT	off;
	UINT16	n;

	n = g_fdc.N;
	if (n >= 4)
	{
		n = 3;
	}
	if (type & 2)
	{
		seg = GETBIOSMEM16(MEMW_F2HD_P_SEG);
		off = GETBIOSMEM16(MEMW_F2HD_P_OFF);
	}
	else
	{
		seg = GETBIOSMEM16(MEMW_F2DD_P_SEG);
		off = GETBIOSMEM16(MEMW_F2DD_P_OFF);
	}
	if (rpm)
	{
		off = 0x2361;									/* see bios.cpp */
	}
	off += g_fdc.us * 2;
	off = MEMR_READ16(seg, off);
	off += n * 8;
	if (!(CPU_AH & 0x40))
	{
		off += 4;
	}
	if (fmt)
	{
		off += 2;
	}
	return MEMR_READ16(seg, off);
}


enum
{
	FDCBIOS_NORESULT,
	FDCBIOS_SUCCESS,
	FDCBIOS_SEEKSUCCESS,
	FDCBIOS_ERROR,
	FDCBIOS_SEEKERROR,
	FDCBIOS_READERROR,
	FDCBIOS_WRITEERROR,
	FDCBIOS_NONREADY,
	FDCBIOS_WRITEPROTECT
};

static void fdd_int(int result)
{
	if (result == FDCBIOS_NORESULT)
	{
		return;
	}
	switch (CPU_AH & 0x0f)
	{
		case 0x00:								/* �V�[�N */
		case 0x01:								/* �x���t�@�C */
		case 0x02:								/* �f�f�ׂ̈̓ǂݍ��� */
		case 0x05:								/* �f�[�^�̏������� */
		case 0x06:								/* �f�[�^�̓ǂݍ��� */
//		case 0x07:								/* �V�����_�O�փV�[�N */
		case 0x0a:								/* READ ID */
		case 0x0d:								/* �t�H�[�}�b�g */
			break;

		default:
			return;
	}
	g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
	switch (result)
	{
		case FDCBIOS_SUCCESS:
			fdcsend_success7();
			break;

		case FDCBIOS_SEEKSUCCESS:
		case FDCBIOS_SEEKERROR:
			g_fdc.stat[g_fdc.us] |= FDCRLT_SE;
			fdc_interrupt();
			g_fdc.event = FDCEVENT_NEUTRAL;
			g_fdc.status = FDCSTAT_RQM;
			break;

		case FDCBIOS_READERROR:
			g_fdc.stat[g_fdc.us] |= FDCRLT_IC0 | FDCRLT_ND;
			fdcsend_error7();
			break;

		case FDCBIOS_WRITEERROR:
			g_fdc.stat[g_fdc.us] |= FDCRLT_IC0 | FDCRLT_EN;
			fdcsend_error7();
			break;

		case FDCBIOS_NONREADY:
			g_fdc.stat[g_fdc.us] |= FDCRLT_IC0 | FDCRLT_NR;
			fdcsend_error7();
			break;

		case FDCBIOS_WRITEPROTECT:
			g_fdc.stat[g_fdc.us] |= FDCRLT_IC0 | FDCRLT_NW;
			fdcsend_error7();
			break;

		default:
			return;
	}
	if (g_fdc.chgreg & 1)
	{
		mem[MEMB_DISK_INTL] &= ~(0x01 << g_fdc.us);
	}
	else
	{
		mem[MEMB_DISK_INTH] &= ~(0x10 << g_fdc.us);
	}
	CPU_IP = BIOSOFST_WAIT;
}

#if 1
static struct
{
	BOOL	flg;
	UINT16	cx;
	UINT16	dx;
	UINT	pos;
} b0p;

static void b0patch(void)
{
	if ((!b0p.flg) || (b0p.cx != CPU_CX) || (b0p.dx != CPU_DX))
	{
		b0p.flg = TRUE;
		b0p.pos = 0;
		b0p.cx = CPU_CX;
		b0p.dx = CPU_DX;
	}
	else
	{
		if (!b0p.pos)
		{
			UINT32	addr;
			UINT	size;
			UINT	cnt;
			REG8	c;
			REG8	cl;
			REG8	last;
			addr = CPU_BP;
			size = CPU_BX;
			cnt = 0;
			last = 0;
			while (size--)
			{
				c = MEMR_READ8(CPU_ES, addr++);
				cl = 0;
				do
				{
					REG8 now = c & 0x80;
					c <<= 1;
					b0p.pos++;
					if (now == last)
					{
						cnt++;
						if (cnt > 4)
						{
							break;
						}
					}
					else
					{
						cnt = 0;
						last = now;
					}
					cl++;
				} while (cl < 8);
				if (cnt > 4)
				{
					break;
				}
			}
		}
		if ((b0p.pos >> 3) < CPU_BX)
		{
			UINT addr;
			REG8 c;
			addr = CPU_BP + (b0p.pos >> 3);
			c = MEMR_READ8(CPU_ES, addr);
			c ^= (1 << (b0p.pos & 7));
			b0p.pos++;
			MEMR_WRITE8(CPU_ES, addr, c);
		}
	}
}

static void b0clr(void)
{
	b0p.flg = FALSE;
}
#endif

static REG8 fdd_operate(REG8 type, REG8 rpm, BOOL ndensity)
{
	REG8	ret_ah = DISK_NOREADY;
	UINT16	size;
	UINT16	pos;
	UINT16	accesssize;
	UINT16	secsize;
	UINT16	para;
	UINT8	s;
	UINT8	ID[4];
	UINT8	hd;
	int		result = FDCBIOS_NORESULT;
	UINT32	addr;
	UINT8	mtr_c;
	UINT	mtr_r;
	UINT	fmode;

	mtr_c = g_fdc.ncn;
	mtr_r = 0;

	/* �Ƃ肠����BIOS�̎��͖������� */
	g_fdc.mf = 0xff;

//	TRACE_("int 1Bh", CPU_AH);

	if (setfdcmode((REG8)(CPU_AL & 3), type, rpm) != SUCCESS)
	{
		return DISK_EQUIPMENTCHECK;
	}

	if ((CPU_AH & 0x0f) != 0x0a)
	{
		g_fdc.crcn = 0;
	}
	if ((CPU_AH & 0x0f) != 0x03)
	{
		if (type & 2)
		{
			if (pic.pi[1].imr & PIC_INT42)
			{
				return DISK_EQUIPMENTCHECK;
			}
		}
		else
		{
			if (pic.pi[1].imr & PIC_INT41)
			{
				return DISK_EQUIPMENTCHECK;
			}
		}
		if (g_fdc.us != (CPU_AL & 0x03))
		{
			g_fdc.us = CPU_AL & 0x03;
			g_fdc.crcn = 0;
		}
		hd = ((CPU_DH) ^ (CPU_AL >> 2)) & 1;
		if (g_fdc.hd != hd)
		{
			g_fdc.hd = hd;
			g_fdc.crcn = 0;
		}
		if (!fdd_diskready(g_fdc.us))
		{
			fdd_int(FDCBIOS_NONREADY);
			ret_ah = DISK_NOREADY;
			if ((CPU_AX & 0x8f40) == 0x8400)
			{
				ret_ah |= 8;					/* 1MB/640KB���p�h���C�u */
				if ((CPU_AH & 0x40) && (g_fdc.support144))
				{
					ret_ah |= 4;				/* 1.44�Ή��h���C�u */
				}
			}
			return ret_ah;
		}
	}

	/* ���[�h�I�� */												/* ver0.78 */
	fmode = (type & 1) ? MEMB_F2HD_MODE : MEMB_F2DD_MODE;
	if (!(CPU_AL & 0x80))
	{
		if (!(mem[fmode] & (0x10 << g_fdc.us)))
		{
			ndensity = TRUE;
		}
	}

	switch (CPU_AH & 0x0f)
	{
		case 0x00:								/* �V�[�N */
			if (CPU_AH & 0x10)
			{
				if (biosfd_seek(CPU_CL, ndensity) == SUCCESS)
				{
					result = FDCBIOS_SEEKSUCCESS;
				}
				else
				{
					result = FDCBIOS_SEEKERROR;
				}
			}
			ret_ah = DISK_READY;
			break;

		case 0x01:								/* �x���t�@�C */
			if (CPU_AH & 0x10)
			{
				if (biosfd_seek(CPU_CL, ndensity) == SUCCESS)
				{
					result = FDCBIOS_SEEKSUCCESS;
				}
				else
				{
					ret_ah = DISK_MISSINGID;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(type, rpm, 0);
			if (!para)
			{
				ret_ah = DISK_BADCYLINDER;
				break;
			}
			if (g_fdc.N < 8)
			{
				secsize = 128 << g_fdc.N;
			}
			else
			{
				secsize = 128 << 8;
			}
			size = CPU_BX;
			while (size)
			{
				if (size > secsize)
				{
					accesssize = secsize;
				}
				else
				{
					accesssize = size;
				}
				if (fdd_read())
				{
					break;
				}
				size -= accesssize;
				mtr_r += accesssize;
				if ((g_fdc.R++ == (UINT8)para) && (CPU_AH & 0x80) && (!g_fdc.hd))
				{
					g_fdc.hd = 1;
					g_fdc.H = 1;
					g_fdc.R = 1;
					if (biosfd_seek(g_fdc.treg[g_fdc.us], 0) != SUCCESS)
					{
						break;
					}
				}
			}
			if (!size)
			{
				ret_ah = DISK_READY;
				result = FDCBIOS_SUCCESS;
			}
			else
			{
				ret_ah = DISK_NODATA;
				result = FDCBIOS_READERROR;
			}
			break;

		case 0x03:								/* ������ */
			fddbios_equip(type, FALSE);
			ret_ah = DISK_READY;
			break;

		case 0x04:								/* �Z���X */
			ret_ah = DISK_READY;
			if (fdd_diskprotect(g_fdc.us))
			{
				ret_ah = DISK_WRITEPROTECT;
			}
			if (CPU_AL & 0x80)					/* 2HD */
			{
				ret_ah |= 0x01;
			}
			else								/* 2DD */
			{
				if (mem[fmode] & (0x01 << g_fdc.us))
				{
					ret_ah |= 0x01;
				}
				if (mem[fmode] & (0x10 << g_fdc.us))
				{
					ret_ah |= 0x04;
				}
			}
			if ((CPU_AX & 0x8f40) == 0x8400)
			{
				ret_ah |= 8;					/* 1MB/640KB���p�h���C�u */
				if ((CPU_AH & 0x40) && (g_fdc.support144))
				{
					ret_ah |= 4;				/* 1.44�Ή��h���C�u */
				}
			}
			break;

		case 0x05:								/* �f�[�^�̏������� */
			if (CPU_AH & 0x10)
			{
				if (biosfd_seek(CPU_CL, ndensity) == SUCCESS)
				{
					result = FDCBIOS_SEEKSUCCESS;
				}
				else
				{
					ret_ah = DISK_MISSINGID;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(type, rpm, 0);
			if (!para)
			{
				ret_ah = DISK_BADCYLINDER;
				break;
			}
			if (fdd_diskprotect(g_fdc.us))
			{
				ret_ah = DISK_NOTWRITABLE;
				result = FDCBIOS_WRITEPROTECT;
				break;
			}
			if (g_fdc.N < 8)
			{
				secsize = 128 << g_fdc.N;
			}
			else
			{
				secsize = 128 << 8;
			}
			size = CPU_BX;
			addr = ES_BASE + CPU_BP;
			while (size)
			{
				if (size > secsize)
				{
					accesssize = secsize;
				}
				else
				{
					accesssize = size;
				}
				MEML_READS(addr, g_fdc.buf, accesssize);
				if (fdd_write())
				{
					break;
				}
				addr += accesssize;
				size -= accesssize;
				mtr_r += accesssize;
				if ((g_fdc.R++ == (UINT8)para) && (CPU_AH & 0x80) && (!g_fdc.hd))
				{
					g_fdc.hd = 1;
					g_fdc.H = 1;
					g_fdc.R = 1;
					if (biosfd_seek(g_fdc.treg[g_fdc.us], 0) != SUCCESS)
					{
						break;
					}
				}
			}
			if (!size)
			{
				ret_ah = DISK_READY;
				result = FDCBIOS_SUCCESS;
			}
			else
			{
				ret_ah = fddlasterror;			/* 0xc0 */
				result = FDCBIOS_WRITEERROR;
			}
			break;

		case 0x02:								/* �f�f�ׂ̈̓ǂݍ��� */
		case 0x06:								/* �f�[�^�̓ǂݍ��� */
			if (CPU_AH & 0x10)
			{
				if (biosfd_seek(CPU_CL, ndensity) == SUCCESS)
				{
					result = FDCBIOS_SEEKSUCCESS;
				}
				else
				{
					ret_ah = DISK_MISSINGID;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(type, rpm, 0);
			if (!para)
			{
				ret_ah = DISK_BADCYLINDER;
				break;
			}
#if 0
			if (g_fdc.R >= 0xf4)
			{
				ret_ah = DISK_DATAERROR;
				break;
			}
#endif
			if (g_fdc.N < 8)
			{
				secsize = 128 << g_fdc.N;
			}
			else
			{
				secsize = 128 << 8;
			}
			size = CPU_BX;
			addr = ES_BASE + CPU_BP;
			while (size)
			{
				if (size > secsize)
				{
					accesssize = secsize;
				}
				else
				{
					accesssize = size;
				}
				if (fdd_read())
				{
					break;
				}
				MEML_WRITES(addr, g_fdc.buf, accesssize);
				addr += accesssize;
				size -= accesssize;
				mtr_r += accesssize;
				if (g_fdc.R++ == (UINT8)para)
				{
					if ((CPU_AH & 0x80) && (!g_fdc.hd))
					{
						g_fdc.hd = 1;
						g_fdc.H = 1;
						g_fdc.R = 1;
						if (biosfd_seek(g_fdc.treg[g_fdc.us], 0) != SUCCESS)
						{
							break;
						}
					}
#if 1
					else
					{
						g_fdc.C++;
						g_fdc.R = 1;
						break;
					}
#endif
				}
			}
			if (!size)
			{
				ret_ah = fddlasterror;				/* 0x00 */
				result = FDCBIOS_SUCCESS;
#if 1
				if (ret_ah == DISK_DATAERROR)
				{
					b0patch();
				}
				else
				{
					b0clr();
				}
#endif
			}
#if 1
			else if ((CPU_AH & 0x0f) == 0x02)		/* ARS�΍�c */
			{
				ret_ah = DISK_READY;
				result = FDCBIOS_READERROR;
			}
#endif
			else
			{
				ret_ah = fddlasterror;				/* 0xc0 */
				result = FDCBIOS_READERROR;
			}
			break;

		case 0x07:						/* �V�����_�O�փV�[�N */
			biosfd_seek(0, 0);
			ret_ah = DISK_READY;
			result = FDCBIOS_SEEKSUCCESS;
			break;

		case 0x0a:						/* READ ID */
			g_fdc.mf = CPU_AH & 0x40;
			if (CPU_AH & 0x10)
			{
				if (biosfd_seek(CPU_CL, ndensity) == SUCCESS)
				{
					result = FDCBIOS_SEEKSUCCESS;
				}
				else
				{
					ret_ah = DISK_MISSINGID;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			if (fdd_readid())
			{
				ret_ah = fddlasterror;			/* 0xa0 */
				break;
			}
			if (g_fdc.N < 8)
			{
				mtr_r += 128 << g_fdc.N;
			}
			else
			{
				mtr_r += 128 << 8;
			}
			ret_ah = DISK_READY;
			CPU_CL = g_fdc.C;
			CPU_DH = g_fdc.H;
			CPU_DL = g_fdc.R;
			CPU_CH = g_fdc.N;
			result = FDCBIOS_SUCCESS;
			break;

		case 0x0d:						/* �t�H�[�}�b�g */
			if (CPU_AH & 0x10)
			{
				biosfd_seek(CPU_CL, ndensity);
			}
			if (fdd_diskprotect(g_fdc.us))
			{
				ret_ah = DISK_NOTWRITABLE;
				break;
			}
			g_fdc.d = CPU_DL;
			g_fdc.N = CPU_CH;
			para = fdfmt_biospara(type, rpm, 1);
			if (!para)
			{
				ret_ah = DISK_BADCYLINDER;
				break;
			}
			g_fdc.sc = (UINT8)para;
			fdd_formatinit();
			pos = CPU_BP;
			for (s = 0; s < g_fdc.sc; s++)
			{
				MEMR_READS(CPU_ES, pos, ID, 4);
				fdd_formating(ID);
				pos += 4;
				if (ID[3] < 8)
				{
					mtr_r += 128 << ID[3];
				}
				else
				{
					mtr_r += 128 << 8;
				}
			}
			ret_ah = DISK_READY;
			break;

		case 0x0e:													/* ver0.78 */
			if (CPU_AH & 0x80) 				/* ���x�ݒ� */
			{
				mem[fmode] &= 0x0f;
				mem[fmode] |= (UINT8)((CPU_AH & 0x0f) << 4);
			}
			else							/* �ʐݒ� */
			{
				mem[fmode] &= 0xf0;
				mem[fmode] |= (UINT8)(CPU_AH & 0x0f);
			}
			ret_ah = DISK_READY;
			break;
	}
	fdd_int(result);
	if (mtr_c != g_fdc.ncn)
	{
		fddmtr_seek(g_fdc.us, mtr_c, mtr_r);
	}
	return ret_ah;
}


/* -------------------------------------------------------------------- BIOS */

static UINT16 boot_fd1(REG8 type, REG8 rpm)
{
	UINT	remain;
	UINT	size;
	UINT32	pos;
	UINT16	bootseg;

	if (setfdcmode(g_fdc.us, type, rpm) != SUCCESS)
	{
		return 0;
	}
	if (biosfd_seek(0, 0) != SUCCESS)
	{
		return 0;
	}
	g_fdc.hd = 0;
	g_fdc.mf = 0x40;			/* �Ƃ肠���� MFM���[�h�Ń��[�h */
	if (fdd_readid())
	{
		g_fdc.mf = 0x00;		/* FM���[�h�Ń��g���C */
		if (fdd_readid())
		{
			return 0;
		}
	}
	remain = 0x400;
	pos = 0x1fc00;
	if ((!g_fdc.N) || (!g_fdc.mf) || (rpm))
	{
		pos = 0x1fe00;
		remain = 0x200;
	}
	g_fdc.R = 1;
	bootseg = (UINT16)(pos >> 4);
	while (remain)
	{
		if (fdd_read())
		{
			return 0;
		}
		if (g_fdc.N < 3)
		{
			size = 128 << g_fdc.N;
		}
		else
		{
			size = 128 << 3;
		}
		if (remain < size)
		{
			CopyMemory(mem + pos, g_fdc.buf, remain);
			break;
		}
		else
		{
			CopyMemory(mem + pos, g_fdc.buf, size);
			pos += size;
			remain -= size;
			g_fdc.R++;
		}
	}
	return bootseg;
}

static UINT16 boot_fd(REG8 drv, REG8 type)
{
	UINT16	bootseg;

	if (drv >= 4)
	{
		return 0;
	}
	g_fdc.us = drv;
	if (!fdd_diskready(g_fdc.us))
	{
		return 0;
	}

	/* 2HD */
	if (type & 1)
	{
		g_fdc.chgreg |= 0x01;
		/* 1.25MB */
		bootseg = boot_fd1(3, 0);
		if (bootseg)
		{
			mem[MEMB_DISK_BOOT] = (UINT8)(0x90 + drv);
			fddbios_equip(3, TRUE);
			return bootseg;
		}
		/* 1.44MB */
		bootseg = boot_fd1(3, 1);
		if (bootseg)
		{
			mem[MEMB_DISK_BOOT] = (UINT8)(0x30 + drv);
			fddbios_equip(3, TRUE);
			return bootseg;
		}
	}
	if (type & 2)
	{
		g_fdc.chgreg &= ~0x01;
		/* 2DD */
		bootseg = boot_fd1(0, 0);
		if (bootseg)
		{
			mem[MEMB_DISK_BOOT] = (UINT8)(0x70 + drv);
			fddbios_equip(0, TRUE);
			return bootseg;
		}
	}
	g_fdc.chgreg |= 0x01;
	return 0;
}

static REG16 boot_hd(REG8 drv)
{
	REG8	ret;

	ret = sxsi_read(drv, 0, mem + 0x1fc00, 0x400);
	if (ret < 0x20)
	{
		mem[MEMB_DISK_BOOT] = drv;
		return 0x1fc0;
	}
	return 0;
}

REG16 bootstrapload(void)
{
	UINT8	i;
	REG16	bootseg;

//	fdmode = 0;
	bootseg = 0;
	switch (mem[MEMB_MSW5] & 0xf0)		/* �������c�{����AL���W�X�^�̒l���� */
	{
		case 0x00:					/* �m�[�}�� */
			break;

		case 0x20:					/* 640KB FDD */
			for (i = 0; (i < 4) && (!bootseg); i++)
			{
				if (fdd_diskready(i))
				{
					bootseg = boot_fd(i, 2);
				}
			}
			break;

		case 0x40:					/* 1.2MB FDD */
			for (i = 0; (i < 4) && (!bootseg); i++)
			{
				if (fdd_diskready(i))
				{
					bootseg = boot_fd(i, 1);
				}
			}
			break;

		case 0x60:					/* MO */
			break;

		case 0xa0:					/* SASI 1 */
			bootseg = boot_hd(0x80);
			break;

		case 0xb0:					/* SASI 2 */
			bootseg = boot_hd(0x81);
			break;

		case 0xc0:					/* SCSI */
			for (i = 0; (i < 4) && (!bootseg); i++)
			{
				bootseg = boot_hd((REG8)(0xa0 + i));
			}
			break;

		default:					/* ROM */
			return 0;
	}
	for (i = 0; (i < 4) && (!bootseg); i++)
	{
		if (fdd_diskready(i))
		{
			bootseg = boot_fd(i, 3);
		}
	}
	for (i = 0; (i < 2) && (!bootseg); i++)
	{
		bootseg = boot_hd((REG8)(0x80 + i));
	}
	for (i = 0; (i < 4) && (!bootseg); i++)
	{
		bootseg = boot_hd((REG8)(0xa0 + i));
	}
	return bootseg;
}


/* -------------------------------------------------------------------------- */

void bios0x1b(void)
{
	REG8	ret_ah;
	REG8	flag;

#if 1			/* bypass to disk bios */
	REG8	seg;
	UINT	sp;

	seg = mem[MEMX_DISK_XROM + (CPU_AL >> 4)];
	if (seg)
	{
		sp = CPU_SP;
		MEMR_WRITE16(CPU_SS, sp - 2, CPU_DS);
		MEMR_WRITE16(CPU_SS, sp - 4, CPU_SI);
		MEMR_WRITE16(CPU_SS, sp - 6, CPU_DI);
		MEMR_WRITE16(CPU_SS, sp - 8, CPU_ES);
		MEMR_WRITE16(CPU_SS, sp - 10, CPU_BP);
		MEMR_WRITE16(CPU_SS, sp - 12, CPU_DX);
		MEMR_WRITE16(CPU_SS, sp - 14, CPU_CX);
		MEMR_WRITE16(CPU_SS, sp - 16, CPU_BX);
		MEMR_WRITE16(CPU_SS, sp - 18, CPU_AX);
#if 0
		TRACEOUT(("call by %.4x:%.4x",
							MEMR_READ16(CPU_SS, CPU_SP+2),
							MEMR_READ16(CPU_SS, CPU_SP)));
		TRACEOUT(("bypass to %.4x:0018", seg << 8));
		TRACEOUT(("AX=%04x BX=%04x %02x:%02x:%02x:%02x ES=%04x BP=%04x",
							CPU_AX, CPU_BX, CPU_CL, CPU_DH, CPU_DL, CPU_CH,
							CPU_ES, CPU_BP));
#endif
		sp -= 18;
		CPU_SP = sp;
		CPU_BP = sp;
		CPU_DS = 0x0000;
		CPU_BX = 0x04B0;
		CPU_AX = seg << 8;
		CPU_CS = seg << 8;
		CPU_IP = 0x18;
		return;
	}
#endif

#if defined(SUPPORT_SCSI)
	if ((CPU_AL & 0xf0) == 0xc0)
	{
		TRACEOUT(("%.4x:%.4x AX=%.4x BX=%.4x CX=%.4x DX=%.4 ES=%.4x BP=%.4x",
							MEMR_READ16(CPU_SS, CPU_SP+2),
							MEMR_READ16(CPU_SS, CPU_SP),
							CPU_AX, CPU_BX, CPU_CX, CPU_DX, CPU_ES, CPU_BP));
		scsicmd_bios();
		return;
	}
#endif

	switch (CPU_AL & 0xf0)
	{
		case 0x90:
			ret_ah = fdd_operate(3, 0, FALSE);
			break;

		case 0x30:
		case 0xb0:
			ret_ah = fdd_operate(3, 1, FALSE);
			break;

		case 0x10:
			ret_ah = fdd_operate(1, 0, FALSE);
			break;

		case 0x70:
		case 0xf0:
			ret_ah = fdd_operate(0, 0, FALSE);
			break;

		case 0x50:
			ret_ah = fdd_operate(0, 0, TRUE);
			break;

		case 0x00:
		case 0x80:
			ret_ah = sasibios_operate();
			break;

#if defined(SUPPORT_SCSI)
		case 0x20:
		case 0xa0:
			ret_ah = scsibios_operate();
			break;
#endif

		default:
			ret_ah = DISK_EQUIPMENTCHECK;
			break;
	}
#if 0
	TRACEOUT(("%04x:%04x AX=%04x BX=%04x %02x:%02x:%02x:%02x\n"	\
						"ES=%04x BP=%04x \nret=%02x",
							MEMR_READ16(CPU_SS, CPU_SP+2),
							MEMR_READ16(CPU_SS, CPU_SP),
							CPU_AX, CPU_BX, CPU_CL, CPU_DH, CPU_DL, CPU_CH,
							CPU_ES, CPU_BP, ret_ah));
#endif
	CPU_AH = ret_ah;
	flag = MEMR_READ8(CPU_SS, CPU_SP + 4) & (~C_FLAG);
	if (ret_ah >= 0x20)
	{
		flag |= C_FLAG;
	}
	MEMR_WRITE8(CPU_SS, CPU_SP + 4, flag);
}

UINT bios0x1b_wait(void)
{
	UINT	addr;
	REG8	bit;

	if (fddmtr.busy)
	{
		CPU_REMCLOCK = -1;
	}
	else
	{
		if (g_fdc.chgreg & 1)
		{
			addr = MEMB_DISK_INTL;
			bit = 0x01;
		}
		else
		{
			addr = MEMB_DISK_INTH;
			bit = 0x10;
		}
		bit <<= g_fdc.us;
		if (mem[addr] & bit)
		{
			mem[addr] &= ~bit;
			return 0;
		}
		else
		{
			CPU_REMCLOCK -= 1000;
		}
	}
	CPU_IP--;
	return 1;
}