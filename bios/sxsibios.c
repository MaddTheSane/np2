#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"sxsibios.h"
#include	"scsicmd.h"
#include	"sxsi.h"


typedef REG8 (*SXSIFUNC)(UINT type, SXSIDEV sxsi);

static REG8 sxsi_pos(UINT type, SXSIDEV sxsi, long *ppos) {

	REG8	ret;
	long	pos;

	ret = 0;
	pos = 0;
	if (CPU_AL & 0x80) {
		if ((CPU_DL >= sxsi->sectors) ||
			(CPU_DH >= sxsi->surfaces) ||
			(CPU_CX >= sxsi->cylinders)) {
			ret = 0xd0;
		}
		pos = ((CPU_CX * sxsi->surfaces) + CPU_DH) * sxsi->sectors
															+ CPU_DL;
	}
	else {
		pos = (CPU_DL << 16) | CPU_CX;
		if (type == SXSIBIOS_SASI) {
			pos &= 0x1fffff;
		}
		if (pos >= sxsi->totals) {
			ret = 0xd0;
		}
	}

	*ppos = pos;
	if (sxsi->size > 1024) {
		ret = 0xd0;
	}
	return(ret);
}

static REG8 sxsibios_write(UINT type, SXSIDEV sxsi) {

	REG8	ret;
	UINT32	addr;
	UINT	size;
	long	pos;
	UINT	r;
	BYTE	work[1024];

	addr = (CPU_ES << 4) + CPU_BP;
	size = CPU_BX;
	if (!size) {
		size = 0x10000;
	}
	ret = sxsi_pos(type, sxsi, &pos);
	if (!ret) {
		while(size) {
			r = min(size, sxsi->size);
			i286_memx_read(addr, work, r);
			ret = sxsi_write(CPU_AL, pos, work, r);
			if (ret >= 0x20) {
				break;
			}
			size -= r;
			addr += r;
			pos++;
		}
	}
	return(ret);
}

static REG8 sxsibios_read(UINT type, SXSIDEV sxsi) {

	REG8	ret;
	UINT32	addr;
	UINT	size;
	long	pos;
	UINT	r;
	BYTE	work[1024];

	addr = (CPU_ES << 4) + CPU_BP;
	size = CPU_BX;
	if (!size) {
		size = 0x10000;
	}
	ret = sxsi_pos(type, sxsi, &pos);
	if (!ret) {
		while(size) {
			r = min(size, sxsi->size);
			ret = sxsi_read(CPU_AL, pos, work, r);
			if (ret >= 0x20) {
				break;
			}
			i286_memx_write(addr, work, r);
			size -= r;
			addr += r;
			pos++;
		}
	}
	return(ret);
}

static REG8 sxsibios_format(UINT type, SXSIDEV sxsi) {

	REG8	ret;
	long	pos;

	if (CPU_AH & 0x80) {
		ret = 0xd0;
	}
	else {
		if (CPU_DL) {
			ret = 0x30;
		}
		else {
//			i286_memstr_read(CPU_ES, CPU_BP, work, CPU_BX);
			ret = sxsi_pos(type, sxsi, &pos);
			if (!ret) {
				ret = sxsi_format(CPU_AL, pos);
			}
		}
	}
	return(ret);
}

static REG8 sxsibios_succeed(UINT type, SXSIDEV sxsi) {

	(void)type;
	(void)sxsi;
	return(0x00);
}

static REG8 sxsibios_failed(UINT type, SXSIDEV sxsi) {

	(void)type;
	(void)sxsi;
	return(0x40);
}


// ---- sasi & IDE

static REG8 sasibios_init(UINT type, SXSIDEV sxsi) {

	UINT16	diskequip;
	UINT8	i;
	UINT16	bit;

	diskequip = GETBIOSMEM16(MEMW_DISK_EQUIP);
	diskequip &= 0xf0ff;
	for (i=0x00, bit=0x0100; i<0x02; i++, bit<<=1) {
		sxsi = sxsi_getptr(i);
		if ((sxsi) && (sxsi->fname[0])) {
			diskequip |= bit;
		}
	}
	SETBIOSMEM16(MEMW_DISK_EQUIP, diskequip);

	(void)type;
	return(0x00);
}

static REG8 sasibios_sense(UINT type, SXSIDEV sxsi) {

	if (type == SXSIBIOS_SASI) {
		return((REG8)((sxsi->type >> 8) & 7));
	}
	else {
		if (CPU_AH == 0x84) {
			CPU_BX = sxsi->size;
			CPU_CX = sxsi->cylinders;
			CPU_DH = sxsi->surfaces;
			CPU_DL = sxsi->sectors;
		}
		return(0x00);
	}
}

static const SXSIFUNC sasifunc[16] = {
			sxsibios_failed,		// SASI 0:
			sxsibios_succeed,		// SASI 1: ベリファイ
			sxsibios_failed,		// SASI 2:
			sasibios_init,			// SASI 3: イニシャライズ
			sasibios_sense,			// SASI 4: センス
			sxsibios_write,			// SASI 5: データの書き込み
			sxsibios_read,			// SASI 6: データの読み込み
			sxsibios_succeed,		// SASI 7: リトラクト
			sxsibios_failed,		// SASI 8:
			sxsibios_failed,		// SASI 9:
			sxsibios_failed,		// SASI a:
			sxsibios_failed,		// SASI b:
			sxsibios_failed,		// SASI c:
			sxsibios_format,		// SASI d: フォーマット
			sxsibios_failed,		// SASI e:
			sxsibios_succeed};		// SASI f: リトラクト

REG8 sasibios_operate(void) {

	SXSIDEV	sxsi;
	UINT	type;

	sxsi = sxsi_getptr(CPU_AL);
	if (sxsi == NULL) {
		return(0x60);
	}
	type = SXSIBIOS_IDE;
#if defined(SUPPORT_SASI)
	if (pccore.hddif & PCHDD_SASI) {
		type = SXSIBIOS_SASI;
	}
#endif
	return((*sasifunc[CPU_AH & 0x0f])(type, sxsi));
}


// ---- scsi

#if defined(SUPPORT_SCSI)
static REG8 scsibios_init(UINT type, SXSIDEV sxsi) {

	UINT8	i;
	UINT8	bit;
	UINT16	w;

	mem[MEMB_DISK_EQUIPS] = 0;
	ZeroMemory(&mem[0x00460], 0x20);
	for (i=0, bit=1; i<4; i++, bit<<=1) {
		sxsi = sxsi_getptr((REG8)(0x20 + i));
		if ((sxsi) && (sxsi->fname[0])) {
			mem[MEMB_DISK_EQUIPS] |= bit;
			mem[0x00460+i*4] = sxsi->sectors;
			mem[0x00461+i*4] = sxsi->surfaces;
			switch(sxsi->size) {
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
			w |= 0xc000;
			w |= sxsi->cylinders;
			SETBIOSMEM16(0x00462+i*4, w);
		}
	}
	(void)type;
	return(0x00);
}

static REG8 scsibios_sense(UINT type, SXSIDEV sxsi) {

	if (CPU_AH == 0x44) {
		CPU_BX = 1;
	}
	else if (CPU_AH == 0x84) {
		CPU_BX = sxsi->size;
		CPU_CX = sxsi->cylinders;
		CPU_DH = sxsi->surfaces;
		CPU_DL = sxsi->sectors;
	}
	(void)type;
	return(0x00);
}

static REG8 scsibios_setsec(UINT type, SXSIDEV sxsi) {

	if (sxsi->size != (128 << (CPU_BH & 3))) {
		return(0x40);
	}
	(void)type;
	return(0x00);
}

static REG8 scsibios_chginf(UINT type, SXSIDEV sxsi) {

	CPU_CX = 0;
	(void)type;
	(void)sxsi;
	return(0x00);
}

static const SXSIFUNC scsifunc[16] = {
			sxsibios_failed,		// SCSI 0:
			sxsibios_succeed,		// SCSI 1: ベリファイ
			sxsibios_failed,		// SCSI 2:
			scsibios_init,			// SCSI 3: イニシャライズ
			scsibios_sense,			// SCSI 4: センス
			sxsibios_write,			// SCSI 5: データの書き込み
			sxsibios_read,			// SCSI 6: データの読み込み
			sxsibios_succeed,		// SCSI 7: リトラクト
			sxsibios_failed,		// SCSI 8:
			sxsibios_failed,		// SCSI 9:
			scsibios_setsec,		// SCSI a: セクタ長設定
			sxsibios_failed,		// SCSI b:
			scsibios_chginf,		// SCSI c: 代替情報取得
			sxsibios_format,		// SCSI d: フォーマット
			sxsibios_failed,		// SCSI e:
			sxsibios_succeed};		// SCSI f: リトラクト

REG8 scsibios_operate(void) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(CPU_AL);
	if (sxsi == NULL) {
		return(0x60);
	}
	return((*scsifunc[CPU_AH & 0x0f])(SXSIBIOS_SCSI, sxsi));
}


// あとで scsicmdから移動
#endif


// ---- np2sysp

#if defined(SUPPORT_SASI) || defined(SUPPORT_SCSI)
typedef struct {
	UINT16	ax;
	UINT16	cx;
	UINT16	dx;
	UINT16	bx;
	UINT16	bp;
	UINT16	si;
	UINT16	di;
	UINT16	es;
	UINT16	ds;
	UINT16	flag;
} REGBAK;

static void reg_push(REGBAK *r) {

	r->ax = CPU_AX;
	r->cx = CPU_CX;
	r->dx = CPU_DX;
	r->bx = CPU_BX;
	r->bp = CPU_BP;
	r->si = CPU_SI;
	r->di = CPU_DI;
	r->es = CPU_ES;
	r->ds = CPU_DS;
	r->flag = CPU_FLAG;
}

static void reg_pop(const REGBAK *r) {

	CPU_AX = r->ax;
	CPU_CX = r->cx;
	CPU_DX = r->dx;
	CPU_BX = r->bx;
	CPU_BP = r->bp;
	CPU_SI = r->si;
	CPU_DI = r->di;
	CPU_ES = r->es;
	CPU_DS = r->ds;
	CPU_FLAG = r->flag;
}

typedef struct {
	BYTE	r_ax[2];
	BYTE	r_bx[2];
	BYTE	r_cx[2];
	BYTE	r_dx[2];
	BYTE	r_bp[2];
	BYTE	r_es[2];
	BYTE	r_di[2];
	BYTE	r_si[2];
	BYTE	r_ds[2];
} B1BREG;

static void reg_load(UINT seg, UINT off) {

	B1BREG	r;

	i286_memstr_read(seg, off, &r, sizeof(r));
	CPU_FLAGL = i286_membyte_read(seg, off + 0x16);
	CPU_AX = LOADINTELWORD(r.r_ax);
	CPU_BX = LOADINTELWORD(r.r_bx);
	CPU_CX = LOADINTELWORD(r.r_cx);
	CPU_DX = LOADINTELWORD(r.r_dx);
	CPU_BP = LOADINTELWORD(r.r_bp);
	CPU_ES = LOADINTELWORD(r.r_es);
	CPU_DI = LOADINTELWORD(r.r_di);
	CPU_SI = LOADINTELWORD(r.r_si);
	CPU_DS = LOADINTELWORD(r.r_ds);
}

static void reg_store(UINT seg, UINT off) {

	B1BREG	r;

	STOREINTELWORD(r.r_ax, CPU_AX);
	STOREINTELWORD(r.r_bx, CPU_BX);
	STOREINTELWORD(r.r_cx, CPU_CX);
	STOREINTELWORD(r.r_dx, CPU_DX);
	STOREINTELWORD(r.r_bp, CPU_BP);
	STOREINTELWORD(r.r_es, CPU_ES);
	STOREINTELWORD(r.r_di, CPU_DI);
	STOREINTELWORD(r.r_si, CPU_SI);
	STOREINTELWORD(r.r_ds, CPU_DS);
	i286_memstr_write(seg, off, &r, sizeof(r));
	i286_membyte_write(seg, off + 0x16, CPU_FLAGL);
}
#endif

#if defined(SUPPORT_SASI)
void np2sysp_sasi(const void *arg1, long arg2) {

	UINT	seg;
	UINT	off;
	REGBAK	regbak;
	REG8	ret;

	seg = CPU_SS;
	off = CPU_BP;
	reg_push(&regbak);
	reg_load(seg, off);

	ret = sasibios_operate();
	CPU_AH = ret;
	CPU_FLAGL &= ~C_FLAG;
	if (ret >= 0x20) {
		CPU_FLAGL |= C_FLAG;
	}

	reg_store(seg, off);
	reg_pop(&regbak);
	(void)arg1;
	(void)arg2;
}
#endif

#if defined(SUPPORT_SCSI)
void np2sysp_scsi(const void *arg1, long arg2) {

	UINT	seg;
	UINT	off;
	REGBAK	regbak;
	REG8	ret;

	seg = CPU_SS;
	off = CPU_BP;
	reg_push(&regbak);
	reg_load(seg, off);

	ret = scsibios_operate();
	CPU_AH = ret;
	CPU_FLAGL &= ~C_FLAG;
	if (ret >= 0x20) {
		CPU_FLAGL |= C_FLAG;
	}

	reg_store(seg, off);
	reg_pop(&regbak);
	(void)arg1;
	(void)arg2;
}

void np2sysp_scsidev(const void *arg1, long arg2) {

	UINT	seg;
	UINT	off;
	REGBAK	regbak;

	seg = CPU_SS;
	off = CPU_BP;
	reg_push(&regbak);
	reg_load(seg, off);

	scsicmd_bios();

	reg_store(seg, off);
	reg_pop(&regbak);
	(void)arg1;
	(void)arg2;
}
#endif

