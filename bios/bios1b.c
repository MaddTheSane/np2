#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"sxsi.h"


enum {
	CACHE_TABLES	= 4,
	CACHE_BUFFER	= 32768
};


		char	fdmode = 0;
static	BYTE	work[65536];
static	BYTE	mtr_c = 0;
static	UINT	mtr_r = 0;


// ---- FDD

static void init_fdd_equip(void) {

	UINT16	diskequip;

	diskequip = GETBIOSMEM16(MEMW_DISK_EQUIP);
	diskequip &= 0x0f00;
	diskequip |= (UINT16)(~fdmode) & 3;
	diskequip |= (UINT16)fdmode << 12;
	SETBIOSMEM16(MEMW_DISK_EQUIP, diskequip);
}

static void biosfd_setchrn(void) {

	fdc.C = CPU_CL;
	fdc.H = CPU_DH;
	fdc.R = CPU_DL;
	fdc.N = CPU_CH;
}

#if 0
static void biosfd_resultout(UINT32 result) {

	BYTE	*ptr;

	ptr = mem + 0x00564 + (fdc.us*8);
	ptr[0] = (BYTE)(result & 0xff) | (fdc.hd << 2) | fdc.us;
	ptr[1] = (BYTE)(result >> 8);
	ptr[2] = (BYTE)(result >> 16);
	ptr[3] = fdc.C;
	ptr[4] = fdc.H;
	ptr[5] = fdc.R;
	ptr[6] = fdc.N;
	ptr[7] = fdc.ncn;
}
#endif

static BOOL biosfd_seek(BYTE track, BOOL ndensity) {

	if (ndensity) {
		if (track < 42) {
			track <<= 1;
		}
		else {
			track = 42 * 2;
		}
	}
	fdc.ncn = track;
	mtr_c = track;
	if (fdd_seek()) {
		return(FAILURE);
	}
	return(SUCCESS);
}

static UINT16 fdfmt_biospara(BYTE fmt, BYTE rpm) {					// ver0.31

	UINT	seg;
	UINT	off;
	UINT16	n;

	n = fdc.N;
	if (n >= 4) {
		n = 3;
	}
	if (CTRL_FDMEDIA == DISKTYPE_2HD) {
		seg = GETBIOSMEM16(MEMW_F2HD_P_SEG);
		off = GETBIOSMEM16(MEMW_F2HD_P_OFF);
	}
	else {
		seg = GETBIOSMEM16(MEMW_F2DD_P_SEG);
		off = GETBIOSMEM16(MEMW_F2DD_P_OFF);
	}
	if (rpm) {
		off = 0x2361;									// see bios.cpp
	}
	off += fdc.us * 2;
	off = i286_memword_read(seg, off);
	off += n * 8;
	if (!(CPU_AH & 0x40)) {
		off += 4;
	}
	if (fmt) {
		off += 2;
	}
	return(i286_memword_read(seg, LOW16(off)));
}

static void change_rpm(BYTE rpm) {									// ver0.31

	if (np2cfg.usefd144) {
		fdc.rpm = rpm;
	}
}



enum {
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

static void fdd_int(int result) {

	if (result == FDCBIOS_NORESULT) {						// ver0.29
		return;
	}
	switch(CPU_AH & 0x0f) {
		case 0x00:								// シーク
		case 0x01:								// ベリファイ
		case 0x02:								// 診断の為の読み込み
		case 0x05:								// データの書き込み
		case 0x06:								// データの読み込み
//		case 0x07:								// シリンダ０へシーク
		case 0x0a:								// READ ID
		case 0x0d:								// フォーマット
			break;
		default:
			return;
	}
	fdc.stat[fdc.us] = (fdc.hd << 2) | fdc.us;
	switch(result) {
		case FDCBIOS_SUCCESS:
			fdcsend_success7();
			break;

		case FDCBIOS_SEEKSUCCESS:
		case FDCBIOS_SEEKERROR:
			fdc.stat[fdc.us] |= FDCRLT_SE;
			fdc_interrupt();
			fdc.event = FDCEVENT_NEUTRAL;
			fdc.status = FDCSTAT_RQM;
			break;

		case FDCBIOS_READERROR:
			fdc.stat[fdc.us] |= FDCRLT_IC0 | FDCRLT_ND;
			fdcsend_error7();
			break;

		case FDCBIOS_WRITEERROR:
			fdc.stat[fdc.us] |= FDCRLT_IC0 | FDCRLT_EN;
			fdcsend_error7();
			break;

		case FDCBIOS_NONREADY:
			fdc.stat[fdc.us] |= FDCRLT_IC0 | FDCRLT_NR;
			fdcsend_error7();
			break;

		case FDCBIOS_WRITEPROTECT:
			fdc.stat[fdc.us] |= FDCRLT_IC0 | FDCRLT_NW;
			fdcsend_error7();
			break;
	}
}

#if 1
static struct {
	BOOL	flg;
	UINT16	cx;
	UINT16	dx;
	UINT	pos;
//	UINT	cnt;
} b0p;

static void b0patch(void) {

	if ((!b0p.flg) || (b0p.cx != CPU_CX) || (b0p.dx != CPU_DX)) {
		b0p.flg = TRUE;
		b0p.pos = 0;
		b0p.cx = CPU_CX;
		b0p.dx = CPU_DX;
	}
	else {
		if (!b0p.pos) {
			UINT32	addr;
			UINT	size;
			UINT	cnt;
			REG8	c;
			REG8	cl;
			REG8	last;
			addr = ES_BASE + CPU_BP;
			size = CPU_BX;
			cnt = 0;
			last = 0;
			while(size--) {
				c = i286_memoryread(addr++);
				cl = 0;
				do {
					REG8 now = c & 0x80;
					c <<= 1;
					b0p.pos++;
					if (now == last) {
						cnt++;
						if (cnt > 4) {
							break;
						}
					}
					else {
						cnt = 0;
						last = now;
					}
					cl++;
				} while(cl < 8);
				if (cnt > 4) {
					break;
				}
			}
		}
		if ((b0p.pos >> 3) < CPU_BX) {
			UINT32 addr;
			REG8 c;
			addr = ES_BASE + CPU_BP + (b0p.pos >> 3);
			c = i286_memoryread(addr);
			c ^= (1 << (b0p.pos & 7));
			b0p.pos++;
			i286_memorywrite(addr, c);
		}
	}
}

static void b0clr(void) {
	b0p.flg = 0;
}
#endif

static BYTE fdd_operate(BYTE type, BOOL ndensity, BYTE rpm) {		// ver0.31

	BYTE	ret_ah = 0x60;
	UINT16	size;
	UINT16	pos;
	UINT16	accesssize;
	UINT16	secsize;
	UINT16	para;
	BYTE	s;
	BYTE	ID[4];
	BYTE	hd;
	int		result = FDCBIOS_NORESULT;
	UINT32	addr;

	mtr_c = 0xff;
	mtr_r = 0;


	// とりあえずBIOSの時は無視する
	fdc.mf = 0xff;						// ver0.29

//	TRACE_("int 1Bh", CPU_AH);

	change_rpm(rpm);												// ver0.31
	if ((CPU_AH & 0x0f) != 0x0a) {
		fdc.crcn = 0;
	}
	if ((CPU_AH & 0x0f) != 0x03) {
		CTRL_FDMEDIA = type;
		switch(type) {
			case DISKTYPE_2HD:
				if (pic.pi[1].imr & PIC_INT42) {
					return(0xd0);
				}
				break;
			case DISKTYPE_2DD:
				if (pic.pi[1].imr & PIC_INT41) {
					return(0xd0);
				}
				break;
		}
		if (fdc.us != (CPU_AL & 0x03)) {
			fdc.us = CPU_AL & 0x03;
			fdc.crcn = 0;
		}
		hd = ((CPU_DH) ^ (CPU_AL >> 2)) & 1;
		if (fdc.hd != hd) {
			fdc.hd = hd;
			fdc.crcn = 0;
		}
		if (!fdd_diskready(fdc.us)) {
			fdd_int(FDCBIOS_NONREADY);
			if (CPU_AH == 0x84) {								// ver0.28
				return(0x68);			// 新センスは 両用ドライブ情報も
			}
			if (CPU_AH == 0xc4) {								// ver0.31
				if (np2cfg.usefd144) {
					return(0x6c);
				}
				return(0x68);
			}
			return(0x60);
		}
	}

	// 2DDのモード選択							// ver0.29
	if (type == DISKTYPE_2DD) {
		if (!(mem[MEMB_F2DD_MODE] & (0x10 << fdc.us))) {
			ndensity = 1;
		}
	}

	switch(CPU_AH & 0x0f) {
		case 0x00:								// シーク
			if (CPU_AH & 0x10) {
				if (!biosfd_seek(CPU_CL, ndensity)) {
					result = FDCBIOS_SEEKSUCCESS;
				}
				else {
					result = FDCBIOS_SEEKERROR;
				}
			}
			ret_ah = 0x00;
			break;

		case 0x01:								// ベリファイ
			if (CPU_AH & 0x10) {
				if (!biosfd_seek(CPU_CL, ndensity)) {
					result = FDCBIOS_SEEKSUCCESS;
				}
				else {
					ret_ah = 0xe0;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(0, rpm);
			if (!para) {
				ret_ah = 0xd0;
				break;
			}
			if (fdc.N < 8) {
				secsize = 128 << fdc.N;
			}
			else {
				secsize = 128 << 8;
			}
			size = CPU_BX;
			while(size) {
				if (size > secsize) {
					accesssize = secsize;
				}
				else {
					accesssize = size;
				}
				if (fdd_read()) {
					break;
				}
				size -= accesssize;
				mtr_r += accesssize;						// ver0.26
				if ((fdc.R++ == (BYTE)para) && (CPU_AH & 0x80) && (!fdc.hd)) {
					fdc.hd = 1;
					fdc.H = 1;
					fdc.R = 1;
					if (biosfd_seek(fdc.treg[fdc.us], 0)) {
						break;
					}
				}
			}
			if (!size) {
				ret_ah = 0x00;
				result = FDCBIOS_SUCCESS;
			}
			else {
				ret_ah = 0xc0;
				result = FDCBIOS_READERROR;
			}
			break;

		case 0x03:								// 初期化
			init_fdd_equip();
			ret_ah = 0x00;
			break;

		case 0x04:								// センス
			ret_ah = 0x00;
			if (fdd_diskaccess()) {
				ret_ah = 0xc0;
			}
			else if (fdd_diskprotect(fdc.us)) {
				ret_ah = 0x10;
			}
			if (CPU_AL & 0x80) {				// 2HD
				ret_ah |= 0x01;
			}
			else {								// 2DD
				if (mem[0x005ca] & (0x01 << fdc.us)) {
					ret_ah++;
				}
				if (mem[0x005ca] & (0x10 << fdc.us)) {				// ver0.30
					ret_ah |= 0x04;
				}
			}
			if (CPU_AH & 0x80) {				// ver0.30
				ret_ah |= 8;					// 1MB/640KB両用ドライブ
				if ((CPU_AH & 0x40) &&
					(np2cfg.usefd144)) {		// ver0.31
					ret_ah |= 4;				// 1.44対応ドライブ
				}
			}
			break;

		case 0x05:								// データの書き込み
			if (CPU_AH & 0x10) {
				if (!biosfd_seek(CPU_CL, ndensity)) {
					result = FDCBIOS_SEEKSUCCESS;
				}
				else {
					ret_ah = 0xe0;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(0, rpm);
			if (!para) {
				ret_ah = 0xd0;
				break;
			}
			if (fdd_diskprotect(fdc.us)) {
				ret_ah = 0x70;
				result = FDCBIOS_WRITEPROTECT;
				break;
			}
			if (fdc.N < 8) {
				secsize = 128 << fdc.N;
			}
			else {
				secsize = 128 << 8;
			}
			size = CPU_BX;
			addr = ES_BASE + CPU_BP;
			while(size) {
				if (size > secsize) {
					accesssize = secsize;
				}
				else {
					accesssize = size;
				}
				i286_memx_read(addr, fdc.buf, accesssize);
				if (fdd_write()) {
					break;
				}
				addr += accesssize;
				size -= accesssize;
				mtr_r += accesssize;						// ver0.26
				if ((fdc.R++ == (BYTE)para) && (CPU_AH & 0x80) && (!fdc.hd)) {
					fdc.hd = 1;
					fdc.H = 1;
					fdc.R = 1;
					if (biosfd_seek(fdc.treg[fdc.us], 0)) {
						break;
					}
				}
			}
			if (!size) {
				ret_ah = 0x00;
				result = FDCBIOS_SUCCESS;
			}
			else {
				ret_ah = fddlasterror;			// 0xc0		// ver0.28
				result = FDCBIOS_WRITEERROR;
			}
			break;

		case 0x02:								// 診断の為の読み込み
		case 0x06:								// データの読み込み
			if (CPU_AH & 0x10) {
				if (!biosfd_seek(CPU_CL, ndensity)) {
					result = FDCBIOS_SEEKSUCCESS;
				}
				else {
					ret_ah = 0xe0;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			biosfd_setchrn();
			para = fdfmt_biospara(0, rpm);
			if (!para) {
				ret_ah = 0xd0;
				break;
			}
#if 0
			if (fdc.R >= 0xf4) {
				ret_ah = 0xb0;
				break;
			}
#endif
			if (fdc.N < 8) {
				secsize = 128 << fdc.N;
			}
			else {
				secsize = 128 << 8;
			}
			size = CPU_BX;
			addr = ES_BASE + CPU_BP;
			while(size) {
				if (size > secsize) {
					accesssize = secsize;
				}
				else {
					accesssize = size;
				}
				if (fdd_read()) {
					break;
				}
				i286_memx_write(addr, fdc.buf, accesssize);
				addr += accesssize;
				size -= accesssize;
				mtr_r += accesssize;						// ver0.26
				if (fdc.R++ == (BYTE)para) {
					if ((CPU_AH & 0x80) && (!fdc.hd)) {
						fdc.hd = 1;
						fdc.H = 1;
						fdc.R = 1;
						if (biosfd_seek(fdc.treg[fdc.us], 0)) {
							break;
						}
					}
#if 1
					else {
						fdc.C++;
						fdc.R = 1;
						break;
					}
#endif
				}
			}
			if (!size) {
				ret_ah = fddlasterror;				// 0x00;
				result = FDCBIOS_SUCCESS;
#if 1
				if (ret_ah == 0xb0) {
					b0patch();
				}
				else {
					b0clr();
				}
#endif
			}
#if 1
			else if ((CPU_AH & 0x0f) == 0x02) {		// ARS対策…
				ret_ah = 0x00;
				result = FDCBIOS_READERROR;
			}
#endif
			else {
				ret_ah = fddlasterror;				// 0xc0;	// ver0.28
				result = FDCBIOS_READERROR;
			}
			break;

		case 0x07:						// シリンダ０へシーク
			biosfd_seek(0, 0);
			ret_ah = 0x00;
			result = FDCBIOS_SEEKSUCCESS;
			break;

		case 0x0a:						// READ ID
			fdc.mf = CPU_AH & 0x40;							// ver0.29
			if (CPU_AH & 0x10) {							// ver0.28
				if (!biosfd_seek(CPU_CL, ndensity)) {
					result = FDCBIOS_SEEKSUCCESS;
				}
				else {
					ret_ah = 0xe0;
					result = FDCBIOS_SEEKERROR;
					break;
				}
			}
			if (fdd_readid()) {
				ret_ah = fddlasterror;			// 0xa0;	// ver0.28
				break;
			}
			if (fdc.N < 8) {								// ver0.26
				mtr_r += 128 << fdc.N;
			}
			else {
				mtr_r += 128 << 8;
			}
			ret_ah = 0x00;
			CPU_CL = fdc.C;
			CPU_DH = fdc.H;
			CPU_DL = fdc.R;
			CPU_CH = fdc.N;
			result = FDCBIOS_SUCCESS;
			break;

		case 0x0d:						// フォーマット
			if (CPU_AH & 0x10) {
				biosfd_seek(CPU_CL, ndensity);
			}
			if (fdd_diskprotect(fdc.us)) {
				ret_ah = 0x70;
				break;
			}
			fdc.d = CPU_DL;
			fdc.N = CPU_CH;
			para = fdfmt_biospara(1, rpm);
			if (!para) {
				ret_ah = 0xd0;
				break;
			}
			fdc.sc = (BYTE)para;
			fdd_formatinit();
			pos = CPU_BP;
			for (s=0; s<fdc.sc; s++) {
				i286_memstr_read(CPU_ES, pos, ID, 4);
				fdd_formating(ID);
				pos += 4;
			}
			ret_ah = 0x00;
			break;
	}
	fdd_int(result);
	fddmtr_seek(fdc.us, mtr_c, mtr_r);
	CPU_IP = BIOSOFST_WAIT;											// ver0.30
	return(ret_ah);
}


// ---- SASI

static void init_sasi_equip(void) {

	UINT16	diskequip;
	UINT	i;
	UINT16	bit;

	diskequip = GETBIOSMEM16(MEMW_DISK_EQUIP);
	diskequip &= 0xf0ff;
	for (i=0, bit=0x0100; i<2; i++, bit<<=1) {
		if (sxsi_hd[i].fname[0]) {
			diskequip |= bit;
		}
	}
	SETBIOSMEM16(MEMW_DISK_EQUIP, diskequip);
}

static void init_scsi_equip(void) {

	UINT	i;
	BYTE	bit;
	UINT16	w;

	mem[MEMB_DISK_EQUIPS] = 0;
	ZeroMemory(&mem[0x00460], 0x20);
	for (i=0, bit=1; i<2; i++, bit<<=1) {
		if (sxsi_hd[i+2].fname[0]) {
			mem[MEMB_DISK_EQUIPS] |= bit;
			mem[0x00460+i*4] = sxsi_hd[i+2].sectors;
			mem[0x00461+i*4] = sxsi_hd[i+2].surfaces;
			switch(sxsi_hd[i+2].size) {
				case 256:
					w = 0 << 12;
					break;

				case 512:
					w = 1 << 12;
					break;

				default:
					w = 2 << 12;
					break;
			}
			w |= sxsi_hd[i+2].tracks;
			SETBIOSMEM16(0x00462+i*4, w);
		}
	}
}

static BYTE sxsi_pos(long *pos) {

	SXSIHDD	sxsi;
	int		np2drv;

	*pos = 0;
	np2drv = (CPU_AL & 0x20) >> 4;
	if ((CPU_AL & 0x0f) >= 2) {
		return(0x60);
	}
	np2drv |= (CPU_AL & 1);
	sxsi = &sxsi_hd[np2drv];
	if (CPU_AL & 0x80) {
		if ((CPU_DL >= sxsi->sectors) ||
			(CPU_DH >= sxsi->surfaces) ||
			(CPU_CX >= sxsi->tracks)) {
			return(0xd0);
		}
		(*pos) = ((CPU_CX * sxsi->surfaces) + CPU_DH) * sxsi->sectors
															+ CPU_DL;
	}
	else {
		*pos = (CPU_DL << 16) | CPU_CX;
		if (!(CPU_AL & 0x20)) {
			(*pos) &= 0x1fffff;
		}
		if ((*pos) >= sxsi->totals) {
			return(0xd0);
		}
	}
	return(0x00);
}


UINT8 sxsi_operate(UINT8 type) {

	BYTE	ret_ah = 0x00;
	BYTE	drv;
	long	pos;
//	int		i;

	drv = (CPU_AL & 0x20) >> 4;
	if ((CPU_AL & 0x0f) >= 2) {
		return(0x60);
	}
	drv |= (CPU_AL & 1);

	switch(CPU_AH & 0x0f) {
		case 0x01:						// ベリファイ
		case 0x07:						// リトラクト
		case 0x0f:						// リトラクト
			break;

   		case 0x03:						// イニシャライズ
			if (type == HDDTYPE_SASI) {
				init_sasi_equip();
			}
			else if (type == HDDTYPE_SCSI) {
				init_scsi_equip();
			}
			break;

   		case 0x04:						// センス
			ret_ah = 0x00;
			if ((CPU_AH == 0x04) && (type == HDDTYPE_SASI)) {
				ret_ah = 0x04;
			}
			else if ((CPU_AH == 0x44) && (type == HDDTYPE_SCSI)) {
				CPU_BX = 1;
			}
			else if (CPU_AH == 0x84) {
				CPU_BX = sxsi_hd[drv].size;
				CPU_CX = sxsi_hd[drv].tracks;
				CPU_DH = sxsi_hd[drv].surfaces;
				CPU_DL = sxsi_hd[drv].sectors;
			}
			break;

		case 0x05:						// データの書き込み
			i286_memx_read(ES_BASE + CPU_BP, work, CPU_BX);
			ret_ah = sxsi_pos(&pos);
			if (!ret_ah) {
				ret_ah = sxsi_write(CPU_AL, pos, work, CPU_BX);
			}
			break;

		case 0x06:						// データの読み込み
			ret_ah = sxsi_pos(&pos);
			if (!ret_ah) {
				ret_ah = sxsi_read(CPU_AL, pos, work, CPU_BX);
				if (ret_ah < 0x20) {
					i286_memx_write(ES_BASE + CPU_BP, work, CPU_BX);
				}
			}
			break;

		case 0x0d:						// フォーマット
			if (CPU_DL) {
				ret_ah = 0x30;
				break;
			}
			i286_memstr_read(CPU_ES, CPU_BP, work, CPU_BX);
			ret_ah = sxsi_pos(&pos);
			if (!ret_ah) {
				ret_ah = sxsi_format(CPU_AL, pos);
			}
			break;

		default:
			ret_ah = 0x40;
			break;
	}
	return(ret_ah);
}


// -------------------------------------------------------------------- BIOS

static UINT16 boot_fd1(BYTE rpm) {									// ver0.31

	UINT	remain;
	UINT	size;
	UINT32	pos;
	UINT16	bootseg;

	change_rpm(rpm);												// ver0.31
	if (biosfd_seek(0, 0)) {
		return(0);
	}
	fdc.hd = 0;
	fdc.mf = 0x40;			// とりあえず MFMモードでリード
	if (fdd_readid()) {
		fdc.mf = 0x00;		// FMモードでリトライ
		if (fdd_readid()) {
			return(0);
		}
	}
	remain = 0x400;
	pos = 0x1fc00;
	if ((!fdc.N) || (!fdc.mf) || (rpm)) {
		pos = 0x1fe00;
		remain = 0x200;
	}
	fdc.R = 1;
	bootseg = (UINT16)(pos >> 4);
	while(remain) {
		if (fdd_read()) {
			return(0);
		}
		if (fdc.N < 3) {
			size = 128 << fdc.N;
		}
		else {
			size = 128 << 3;
		}
		if (remain < size) {
			CopyMemory(mem + pos, fdc.buf, remain);
			break;
		}
		else {
			CopyMemory(mem + pos, fdc.buf, size);
			pos += size;
			remain -= size;
			fdc.R++;
		}
	}
	return(bootseg);
}

static UINT16 boot_fd(BYTE drv, BYTE type) {						// ver0.27

	UINT16	bootseg;

	if (drv >= 4) {
		return(0);
	}
	fdc.us = drv & 3;
	if (!fdd_diskready(fdc.us)) {
		return(0);
	}

	// 2HD
	if (type & 1) {
		CTRL_FDMEDIA = DISKTYPE_2HD;
		// 1.25MB
		bootseg = boot_fd1(0);
		if (bootseg) {
			mem[MEMB_DISK_BOOT] = (BYTE)(0x90+drv);
			return(bootseg);
		}
		// 1.44MB
		bootseg = boot_fd1(1);
		if (bootseg) {
			mem[MEMB_DISK_BOOT] = (BYTE)(0x30+drv);
			return(bootseg);
		}
	}
	if (type & 2) {										// ver0.29
		// 2DD
		CTRL_FDMEDIA = DISKTYPE_2DD;
		bootseg = boot_fd1(0);
		if (bootseg) {
			mem[MEMB_DISK_BOOT] = (BYTE)(0x70+drv);
			fdmode = 3;
			return(bootseg);
		}
	}
	return(0);
}

static UINT16 boot_hd(BYTE drv) {									// ver0.27

	BYTE	ret;

	ret = sxsi_read(drv, 0, mem + 0x1fc00, 0x400);
	if (ret < 0x20) {
		mem[MEMB_DISK_BOOT] = drv;
		return(0x1fc0);
	}
	return(0);
}

UINT16 bootstrapload(void) {										// ver0.27

	BYTE	i;
	UINT16	bootseg;

	fdmode = 0;
	bootseg = 0;
	switch(mem[MEMB_MSW5] & 0xf0) {		// うぐぅ…本当はALレジスタの値から
		case 0x00:					// ノーマル
			break;
		case 0x20:					// 640KB FDD
			for (i=0; (i<4) && (!bootseg); i++) {
				if (fdd_diskready(i)) {
					bootseg = boot_fd(i, 2);
				}
			}
			break;
		case 0x40:					// 1.2MB FDD
			for (i=0; (i<4) && (!bootseg); i++) {
				if (fdd_diskready(i)) {
					bootseg = boot_fd(i, 1);
				}
			}
			break;
		case 0x60:					// MO
			break;
		case 0xa0:					// SASI 1
			if (sxsi_hd[0].fname[0]) {
				bootseg = boot_hd(0x80);
			}
			break;
		case 0xb0:					// SASI 2
			if (sxsi_hd[1].fname[0]) {
				bootseg = boot_hd(0x81);
			}
			break;
		case 0xc0:					// SCSI
			for (i=0; (i<2) && (!bootseg); i++) {
				if (sxsi_hd[i+2].fname[0]) {
					bootseg = boot_hd((BYTE)(0xa0 | i));
				}
			}
			break;
		default:					// ROM
			return(0);
	}
	for (i=0; (i<4) && (!bootseg); i++) {
		if (fdd_diskready(i)) {
			bootseg = boot_fd(i, 3);
		}
	}
	for (i=0; (i<2) && (!bootseg); i++) {
		if (sxsi_hd[i].fname[0]) {
			bootseg = boot_hd((BYTE)(0x80 | i));
		}
	}
	for (i=0; (i<2) && (!bootseg); i++) {
		if (sxsi_hd[i+2].fname[0]) {
			bootseg = boot_hd((BYTE)(0xa0 | i));
		}
	}

	init_fdd_equip();
	init_sasi_equip();
	init_scsi_equip();
	return(bootseg);
}

// --------------------------------------------------------------------------

void bios0x1b(void) {

	BYTE	ret_ah;
	REG8	flag;

#if 0			// bypass to disk bios
	REG8	seg;
	UINT	sp;

	seg = mem[0x004b0 + (CPU_AL >> 4)];
	if (seg) {
		TRACEOUT(("call by %.4x:%.4x",
							i286_memword_read(CPU_SS, CPU_SP+2),
							i286_memword_read(CPU_SS, CPU_SP)));
		sp = CPU_SP;
		i286_memword_write(CPU_SS, sp - 2, CPU_DS);
		i286_memword_write(CPU_SS, sp - 4, CPU_SI);
		i286_memword_write(CPU_SS, sp - 6, CPU_DI);
		i286_memword_write(CPU_SS, sp - 8, CPU_ES);			// +a
		i286_memword_write(CPU_SS, sp - 10, CPU_BP);		// +8
		i286_memword_write(CPU_SS, sp - 12, CPU_DX);		// +6
		i286_memword_write(CPU_SS, sp - 14, CPU_CX);		// +4
		i286_memword_write(CPU_SS, sp - 16, CPU_BX);		// +2
		i286_memword_write(CPU_SS, sp - 18, CPU_AX);		// +0
		TRACEOUT(("bypass to %.4x:0018", seg << 8));
		TRACEOUT(("AX=%04x BX=%04x %02x:%02x:%02x:%02x ES=%04x BP=%04x",
							CPU_AX, CPU_BX, CPU_CL, CPU_DH, CPU_DL, CPU_CH,
							CPU_ES, CPU_BP));
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

	switch(CPU_AL & 0xf0) {
		case 0x90:
			ret_ah = fdd_operate(DISKTYPE_2HD, 0, 0);
			break;

		case 0x30:
		case 0xb0:
			ret_ah = fdd_operate(DISKTYPE_2HD, 0, 1);
			break;

		case 0x10:
		case 0x70:
		case 0xf0:
			ret_ah = fdd_operate(DISKTYPE_2DD, 0, 0);
			break;

		case 0x50:
			ret_ah = fdd_operate(DISKTYPE_2DD, 1, 0);
			break;

		case 0x00:
		case 0x80:
			ret_ah = sxsi_operate(HDDTYPE_SASI);
			break;
#if 0
		case 0x20:
		case 0xa0:
			ret_ah = sxsi_operate(HDDTYPE_SCSI);
			break;
#endif

		default:
			ret_ah = 0x40;
			break;
	}
#if 0
	{
		static BYTE p = 0;
		if ((CPU_CL == 0x4d) && (ret_ah == 0xe0)) {
			if (!p) {
				trace_sw = 1;
				p++;
				debug_status();
				memorydump();
			}
		}
	}
#endif
#if 0
	TRACEOUT(("%04x:%04x AX=%04x BX=%04x %02x:%02x:%02x:%02x\n"	\
						"ES=%04x BP=%04x \nret=%02x",
							i286_memword_read(CPU_SS, CPU_SP+2),
							i286_memword_read(CPU_SS, CPU_SP),
							CPU_AX, CPU_BX, CPU_CL, CPU_DH, CPU_DL, CPU_CH,
							CPU_ES, CPU_BP, ret_ah));
#endif
	CPU_AH = ret_ah;
	flag = i286_membyte_read(CPU_SS, CPU_SP+4) & 0xfe;
	if (ret_ah >= 0x20) {
		flag += 1;
	}
	i286_membyte_write(CPU_SS, CPU_SP + 4, flag);
}

