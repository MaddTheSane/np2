
/*
	ゲストOS(DOS)からホストOS(Win)にアクセスするの～
	完全にDOS(3.1以上)依存だお(汗
	ネットワークインタフェイス搭載前の繋ぎだけど
	更に、手抜き版だし(マテ
*/

#include	"compiler.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"hostdrv.h"
#include	"hostdrvs.h"
#include	"hostdrv.tbl"


#define IS_PERMITWRITE		(np2cfg.hdrvacc & HDFMODE_WRITE)
#define IS_PERMITDELETE		(np2cfg.hdrvacc & HDFMODE_DELETE)

#define ROOTPATH_NAME		"\\\\HOSTDRV\\"
#define ROOTPATH_SIZE		(sizeof(ROOTPATH_NAME) - 1)

static const char ROOTPATH[ROOTPATH_SIZE] = ROOTPATH_NAME;
static const HDRVDIR hdd_volume = {"_HOSTDRIVE_", 0, 0, 0x08, {0}, {0}};
static const HDRVDIR hdd_owner  = {".          ", 0, 0, 0x10, {0}, {0}};
static const HDRVDIR hdd_parent = {"..         ", 0, 0, 0x10, {0}, {0}};


//	see int2159-BX0000
enum {
	ERR_NOERROR				= 0x00,
	ERR_FILENOTFOUND		= 0x02,
	ERR_PATHNOTFOUND		= 0x03,
	ERR_ACCESSDENIED		= 0x05,
	ERR_ATTEMPTEDCURRDIR	= 0x10,
	ERR_NOMOREFILES			= 0x12,
	ERR_DISKWRITEPROTECTED	= 0x13,
	ERR_WRITEFAULT			= 0x1d,
	ERR_READFAULT			= 0x1e
};

	HOSTDRV		hostdrv;


// ---- i/f

static void succeed(INTRST intrst) {

	intrst->r.b.flag_l &= ~C_FLAG;
	STOREINTELWORD(intrst->r.w.ax, ERR_NOERROR);
}

static void fail(INTRST intrst, UINT16 err_code) {

	intrst->r.b.flag_l |= C_FLAG;
	STOREINTELWORD(intrst->r.w.ax, err_code);
}


static void fetch_if4dos(void) {

	REG16	off;
	REG16	seg;
	IF4DOS	if4dos;

	off = i286_memoryread_w(IF4DOSPTR_ADDR);
	seg = i286_memoryread_w(IF4DOSPTR_ADDR + 2);
	i286_memstr_read(seg, off, &if4dos, sizeof(if4dos));
	hostdrv.drive_no = if4dos.drive_no;
	hostdrv.dosver_major = if4dos.dosver_major;
	hostdrv.dosver_minor = if4dos.dosver_minor;
	hostdrv.sda_off = LOADINTELWORD(if4dos.sda_off);
	hostdrv.sda_seg = LOADINTELWORD(if4dos.sda_seg);

	TRACEOUT(("hostdrv.drive_no = %d", if4dos.drive_no));
	TRACEOUT(("hostdrv.dosver = %d.%.2d", if4dos.dosver_major, if4dos.dosver_minor));
	TRACEOUT(("hostdrv.sda = %.4x:%.4x", hostdrv.sda_seg, hostdrv.sda_off));
}


static void fetch_intr_regs(INTRST is) {

	i286_memstr_read(CPU_SS, CPU_BP, &is->r, sizeof(is->r));
}

static void store_intr_regs(INTRST is) {

	i286_memstr_write(CPU_SS, CPU_BP, &is->r, sizeof(is->r));
}


static void fetch_sda_currcds(SDACDS sc) {

	REG16	off;
	REG16	seg;

	if (hostdrv.dosver_major == 3) {
		i286_memstr_read(hostdrv.sda_seg, hostdrv.sda_off,
										&sc->ver3.sda, sizeof(sc->ver3.sda));
		off = LOADINTELWORD(sc->ver3.sda.cdsptr.off);
		seg = LOADINTELWORD(sc->ver3.sda.cdsptr.seg);
		i286_memstr_read(seg, off, &sc->ver3.cds, sizeof(sc->ver3.cds));
	}
	else {
		i286_memstr_read(hostdrv.sda_seg, hostdrv.sda_off,
										&sc->ver4.sda, sizeof(sc->ver4.sda));
		off = LOADINTELWORD(sc->ver4.sda.cdsptr.off);
		seg = LOADINTELWORD(sc->ver4.sda.cdsptr.seg);
		i286_memstr_read(seg, off, &sc->ver4.cds, sizeof(sc->ver4.cds));
	}
}

static void store_sda_currcds(SDACDS sc) {

	REG16	off;
	REG16	seg;

	if (hostdrv.dosver_major == 3) {
		i286_memstr_write(hostdrv.sda_seg, hostdrv.sda_off,
										&sc->ver3.sda, sizeof(sc->ver3.sda));
		off = LOADINTELWORD(sc->ver3.sda.cdsptr.off);
		seg = LOADINTELWORD(sc->ver3.sda.cdsptr.seg);
		i286_memstr_write(seg, off, &sc->ver3.cds, sizeof(sc->ver3.cds));
	}
	else {
		i286_memstr_write(hostdrv.sda_seg, hostdrv.sda_off,
										&sc->ver4.sda, sizeof(sc->ver4.sda));
		off = LOADINTELWORD(sc->ver4.sda.cdsptr.off);
		seg = LOADINTELWORD(sc->ver4.sda.cdsptr.seg);
		i286_memstr_write(seg, off, &sc->ver4.cds, sizeof(sc->ver4.cds));
	}
}


static void fetch_sft(INTRST is, SFTREC sft) {

	REG16	off;
	REG16	seg;

	off = LOADINTELWORD(is->r.w.di);
	seg = LOADINTELWORD(is->r.w.es);
	i286_memstr_read(seg, off, sft, sizeof(_SFTREC));
}

static void store_sft(INTRST is, SFTREC sft) {

	REG16	off;
	REG16	seg;

	off = LOADINTELWORD(is->r.w.di);
	seg = LOADINTELWORD(is->r.w.es);
	i286_memstr_write(seg, off, sft, sizeof(_SFTREC));
}


static void store_srch(INTRST is) {

	SRCHREC	srchrec;

	// SDA内のSRCHRECにセット
	srchrec = is->srchrec_ptr;
	srchrec->drive_no = 0xc0 | hostdrv.drive_no;
	CopyMemory(srchrec->srch_mask, is->fcbname_ptr, 11);
	srchrec->attr_mask = *is->srch_attr_ptr;
	STOREINTELWORD(srchrec->dir_entry_no, ((UINT16)-1));
	STOREINTELWORD(srchrec->dir_sector, ((UINT16)-1));
}

static void store_dir(INTRST is, const HDRVDIR *di) {

	DIRREC	dirrec;
	UINT8	attr;
	UINT16	reg;

	// SDA内のDIRRECにセット
	dirrec = is->dirrec_ptr;
	CopyMemory(dirrec->file_name, di->fcbname, 11);
	attr = (UINT8)(di->attr & 0x3f);
	if (!IS_PERMITWRITE) {
		attr |= 0x01;
	}
	dirrec->file_attr = attr;
	reg = 0;
	if (di->caps & FLICAPS_TIME) {
		reg |= (di->time.hour & 0x1f) << 11;
		reg |= (di->time.minute & 0x3f) << 5;
		reg |= (di->time.second & 0x3e) >> 1;
	}
	STOREINTELWORD(dirrec->file_time, reg);
	reg = 0;
	if (di->caps & FLICAPS_DATE) {
		reg |= ((di->date.year - 1980) & 0x7f) << 9;
		reg |= (di->date.month & 0x0f) << 5;
		reg |= di->date.day & 0x1f;
	}
	STOREINTELWORD(dirrec->file_date, reg);
	STOREINTELWORD(dirrec->start_sector, ((UINT16)-1));
	STOREINTELDWORD(dirrec->file_size, di->size);
}

static void fill_sft(INTRST is, SFTREC sft, UINT num, HDRVDIR *di) {

	UINT8	attr;
	UINT16	reg;

	attr = di->attr;
	if (!IS_PERMITWRITE) {
		attr |= 0x01;
	}
	sft->file_attr = attr;
	STOREINTELWORD(sft->start_sector, (UINT16)num);

	reg = 0;
	if (di->caps & FLICAPS_TIME) {
		reg |= (di->time.hour & 0x1f) << 11;
		reg |= (di->time.minute & 0x3f) << 5;
		reg |= (di->time.second & 0x3e) >> 1;
	}
	STOREINTELWORD(sft->file_time, reg);
	reg = 0;
	if (di->caps & FLICAPS_DATE) {
		reg |= ((di->date.year - 1980) & 0x7f) << 9;
		reg |= (di->date.month & 0x0f) << 5;
		reg |= di->date.day & 0x1f;
	}
	STOREINTELWORD(sft->file_date, reg);
	STOREINTELDWORD(sft->file_size, di->size);
	STOREINTELWORD(sft->dir_sector, (UINT16)-1);
	sft->dir_entry_no = (UINT8)-1;
	CopyMemory(sft->file_name, is->fcbname_ptr, 11);
}

static void init_sft(SFTREC sft) {

	if (sft->open_mode[1] & 0x80) {	// fcb mode
		sft->open_mode[0] |= 0xf0;
	}
	else {
		sft->open_mode[0] &= 0x0f;
	}
	sft->dev_info_word[0] = (BYTE)(0x40 | hostdrv.drive_no);
	sft->dev_info_word[1] = 0x80;
	STOREINTELDWORD(sft->dev_drvr_ptr, 0);
	STOREINTELDWORD(sft->file_pos, 0);
	STOREINTELWORD(sft->rel_sector, (UINT16)-1);
	STOREINTELWORD(sft->abs_sector, (UINT16)-1);
	if (sft->open_mode[1] & 0x80) {	// fcb mode
		CPU_FLAG |= C_FLAG;
	}
}


static BOOL is_wildcards(const char *path) {

	int		i;

	for (i=0; i<11; i++) {
		if (path[i] == '?') {
			return(TRUE);
		}
	}
	return(FALSE);
}

static BOOL match2mask(const char *mask, const char *path) {

	int		i;

	for (i=0; i<11; i++) {
		if ((mask[i] != path[i]) && (mask[i] != '?')) {
			return(FALSE);
		}
	}
	return(TRUE);
}


// ぽいんた初期化
static void setup_ptrs(INTRST is, SDACDS sc) {

	char	*rootpath;
	int		off;

	if (hostdrv.dosver_major == 3) {
		is->fcbname_ptr = sc->ver3.sda.fcb_name;
		is->filename_ptr = sc->ver3.sda.file_name + ROOTPATH_SIZE - 1;
		is->fcbname_ptr_2 = sc->ver3.sda.fcb_name_2;
		is->filename_ptr_2 = sc->ver3.sda.file_name_2 + ROOTPATH_SIZE - 1;

		is->srchrec_ptr = &sc->ver3.sda.srchrec;
		is->dirrec_ptr = &sc->ver3.sda.dirrec;
		is->srchrec_ptr_2 = &sc->ver3.sda.rename_srchrec;
		is->dirrec_ptr_2 = &sc->ver3.sda.rename_dirrec;
		is->srch_attr_ptr = &sc->ver3.sda.srch_attr;

		rootpath = sc->ver3.cds.current_path;
		off = LOADINTELWORD(sc->ver3.cds.root_ofs);
		is->root_path = rootpath;
		is->current_path = rootpath + off;
	}
	else {
		is->fcbname_ptr = sc->ver4.sda.fcb_name;
		is->filename_ptr = sc->ver4.sda.file_name + ROOTPATH_SIZE - 1;
		is->fcbname_ptr_2 = sc->ver4.sda.fcb_name_2;
		is->filename_ptr_2 = sc->ver4.sda.file_name_2 + ROOTPATH_SIZE - 1;

		is->srchrec_ptr = &sc->ver4.sda.srchrec;
		is->dirrec_ptr = &sc->ver4.sda.dirrec;
		is->srchrec_ptr_2 = &sc->ver4.sda.rename_srchrec;
		is->dirrec_ptr_2 = &sc->ver4.sda.rename_dirrec;
		is->srch_attr_ptr = &sc->ver4.sda.srch_attr;

		rootpath = sc->ver4.cds.current_path;
		off = LOADINTELWORD(sc->ver4.cds.root_ofs);
		is->root_path = rootpath;
		is->current_path = rootpath + off;
	}
}


static BOOL pathishostdrv(INTRST is, SDACDS sc) {

	fetch_sda_currcds(sc);
	setup_ptrs(is, sc);

	if (memcmp(is->root_path, ROOTPATH, ROOTPATH_SIZE)) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return(FAILURE);
	}
	if (is->is_chardev) {
		fail(is, ERR_ACCESSDENIED);
		return(FAILURE);
	}
	return(SUCCESS);
}


static BOOL read_data(UINT num, UINT32 pos, UINT size, UINT seg, UINT off) {

	HDRVFILE	hdf;
	FILEH		fh;
	BYTE		work[1024];
	UINT		r;

	hdf = (HDRVFILE)listarray_getitem(hostdrv.fhdl, num);
	if (hdf == NULL) {
		return(FAILURE);
	}
	fh = (FILEH)hdf->hdl;
	if (file_seek(fh, (long)pos, FSEEK_SET) != (long)pos) {
		return(FAILURE);
	}
	while(size) {
		r = min(size, sizeof(work));
		if (file_read(fh, work, r) != r) {
			return(FAILURE);
		}
		i286_memstr_write(seg, off, work, r);
		off += r;
		size -= r;
	}
	return(SUCCESS);
}

static BOOL write_data(UINT num, UINT32 pos, UINT size, UINT seg, UINT off) {

	HDRVFILE	hdf;
	FILEH		fh;
	BYTE		work[1024];
	UINT		r;

	hdf = (HDRVFILE)listarray_getitem(hostdrv.fhdl, num);
	if (hdf == NULL) {
		return(FAILURE);
	}
	fh = (FILEH)hdf->hdl;
	if (file_seek(fh, (long)pos, FSEEK_SET) != (long)pos) {
		return(FAILURE);
	}
	if (!size) {
		file_write(fh, work, 0);
	}
	else {
		do {
			r = min(size, sizeof(work));
			i286_memstr_read(seg, off, work, r);
			if (file_write(fh, work, r) != r) {
				return(FAILURE);
			}
			off += r;
			size -= r;
		} while(size);
	}
	return(SUCCESS);
}


static BOOL find_file1(INTRST is, const HDRVDIR *di) {

	UINT8	attrmask;
	UINT	attr;

	attrmask = *is->srch_attr_ptr;
	attr = (di->attr) & (~(attrmask));
	if (attr & 0x16) {
		return(FAILURE);
	}
	if (!match2mask(is->srchrec_ptr->srch_mask, di->fcbname)) {
		return(FAILURE);
	}
	store_dir(is, di);
	return(SUCCESS);
}

static BOOL find_file(INTRST is) {

	BOOL		ret;
	UINT		pos;
	HDRVLST		hdl;
const HDRVDIR	*di;

	store_srch(is);

	ret = FAILURE;
	pos = hostdrv.flistpos;
	do {
		if (pos == 0) {
			di = &hdd_owner;
		}
		else if (pos == 1) {
			di = &hdd_parent;
		}
		else {
			hdl = listarray_getitem(hostdrv.flist, pos - 2);
			if (hdl == NULL) {
				break;
			}
			di = &hdl->di;
		}
		pos++;
		ret = find_file1(is, di);
	} while(ret != SUCCESS);
	hostdrv.flistpos = pos;
	return(ret);
}


// ----

/* cmd in int2f11 */
/* 00 */
static void inst_check(INTRST intrst) {

	intrst->r.b.flag_l &= ~C_FLAG;
	intrst->r.b.al = 0xff;					// インストール済み。追加OKだお
}

/* 05 */
static void change_currdir(INTRST intrst) {

	_SDACDS		sc;
	char		*ptr;
	HDRVPATH	hdp;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}

	ptr = intrst->filename_ptr;
	TRACEOUT(("change_currdir %s", intrst->filename_ptr));
	if (ptr[0] == '\0') {							// るーと
		strcpy(intrst->filename_ptr, "\\");
		strcpy(intrst->current_path, intrst->filename_ptr);
		store_sda_currcds(&sc);
		succeed(intrst);
		return;
	}
	if ((strlen(intrst->filename_ptr) >= (67 - ROOTPATH_SIZE)) ||
		(is_wildcards(intrst->fcbname_ptr) != FALSE) ||
		(hostdrvs_getrealpath(&hdp, ptr) != SUCCESS) ||
		(hdp.di.fcbname[0] == ' ') || (!(hdp.di.attr & 0x10))) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	strcpy(intrst->current_path, intrst->filename_ptr);
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 06 */
static void close_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	UINT16		handle_count;
	UINT16		start_sector;
	HDRVFILE	hdf;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);
	setup_ptrs(intrst, &sc);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
	handle_count = LOADINTELWORD(sft.handle_count);

	if (handle_count) {
		handle_count--;
	}
	if (handle_count == 0) {
		start_sector = LOADINTELWORD(sft.start_sector);
		hdf = listarray_getitem(hostdrv.fhdl, start_sector);
		if (hdf) {
			file_close((FILEH)hdf->hdl);
			hdf->hdl = (long)FILEH_INVALID;
			hdf->path[0] = '\0';
		}
	}
	STOREINTELWORD(sft.handle_count, handle_count);
	store_sft(intrst, &sft);
	succeed(intrst);
}

/* 07 */
static void commit_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}

	// なんもしないよー
	succeed(intrst);
}

/* 08 */
static void read_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	UINT16		cx;
	UINT		file_size;
	UINT32		file_pos;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);
	setup_ptrs(intrst, &sc);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
	if (sft.open_mode[0] & 1) {
		fail(intrst, ERR_ACCESSDENIED);
		return;
	}

	cx = LOADINTELWORD(intrst->r.w.cx);
	file_size = LOADINTELDWORD(sft.file_size);
	file_pos = LOADINTELDWORD(sft.file_pos);
	if (cx > (file_size - file_pos)) {
		cx = (UINT16)(file_size - file_pos);
		STOREINTELWORD(intrst->r.w.cx, cx);
	}
	if (cx == 0) {
		succeed(intrst);
		return;
	}
	if (read_data(LOADINTELWORD(sft.start_sector), file_pos, cx,
					LOADINTELWORD(sc.ver3.sda.current_dta.seg),
					LOADINTELWORD(sc.ver3.sda.current_dta.off)) != SUCCESS) {
		fail(intrst, ERR_READFAULT);
		return;
	}
	file_pos += cx;
	STOREINTELDWORD(sft.file_pos, file_pos);

	store_sft(intrst, &sft);
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 09 */
static void write_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	UINT16		cx;
	UINT		file_size;
	UINT32		file_pos;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);
	setup_ptrs(intrst, &sc);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}

	if ((!IS_PERMITWRITE) ||
		(!(sft.open_mode[0] & 3))) {	// read only
		fail(intrst, ERR_ACCESSDENIED);
		return;
	}

	cx = LOADINTELWORD(intrst->r.w.cx);
	file_size = LOADINTELDWORD(sft.file_size);
	file_pos = LOADINTELDWORD(sft.file_pos);
	if (write_data(LOADINTELWORD(sft.start_sector), file_pos, cx,
					LOADINTELWORD(sc.ver3.sda.current_dta.seg),
					LOADINTELWORD(sc.ver3.sda.current_dta.off)) != SUCCESS) {
		fail(intrst, ERR_WRITEFAULT);
		return;
	}
	if (cx) {
		file_pos += cx;
		if (file_size < file_pos) {
			file_size = file_pos;
		}
	}
	else {
		file_size = file_pos;
	}

	STOREINTELDWORD(sft.file_size, file_size);
	STOREINTELDWORD(sft.file_pos, file_pos);
	store_sft(intrst, &sft);
	succeed(intrst);
}

/* 0A */
static void lock_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
	// 未実装
	TRACEOUT(("hostdrv: lock_file"));
}

/* 0B */
static void unlock_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
	// 未実装
	TRACEOUT(("hostdrv: unlock_file"));
}

/* 0E */
static void set_fileattr(INTRST intrst) {

	_SDACDS		sc;
	HDRVPATH	hdp;
	REG16		attr;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}
	if ((is_wildcards(intrst->fcbname_ptr)) ||
		(hostdrvs_getrealpath(&hdp, intrst->filename_ptr) != SUCCESS)) {
		fail(intrst, ERR_FILENOTFOUND);
		return;
	}
	if (!IS_PERMITWRITE) {
		fail(intrst, ERR_ACCESSDENIED);
		return;
	}
	attr = i286_memword_read(CPU_SS, CPU_BP + sizeof(IF4INTR)) & 0x37;

	// 成功したことにする...
	succeed(intrst);
}

/* 0F */
static void get_fileattr(INTRST intrst) {

	_SDACDS		sc;
	HDRVPATH	hdp;
	UINT16		ax;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}

	TRACEOUT(("get_fileattr: ->%s", intrst->fcbname_ptr));
	if ((is_wildcards(intrst->fcbname_ptr)) ||
		(hostdrvs_getrealpath(&hdp, intrst->filename_ptr) != SUCCESS)) {
		fail(intrst, ERR_FILENOTFOUND);
		return;
	}
	TRACEOUT(("get_fileattr: %s - %x", hdp.path, hdp.di.attr));
	ax = hdp.di.attr & 0x37;
	if (!IS_PERMITWRITE) {
		ax |= 0x01;
	}
	intrst->r.b.flag_l &= ~C_FLAG;
	STOREINTELWORD(intrst->r.w.ax, ax);
}

/* 11 */
static void rename_file(INTRST intrst) {

	_SDACDS		sc;
	HDRVPATH	hdp1;
	HDRVPATH	hdp2;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}

	// ワイルドカードくるんで要修正…
	if ((hostdrvs_getrealpath(&hdp1, intrst->filename_ptr) != SUCCESS) ||
		(hostdrvs_getrealpath(&hdp2, intrst->filename_ptr_2) != SUCCESS)) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	TRACEOUT(("rename_file %s to %s - failed", hdp1.path, hdp2.path));
	fail(intrst, ERR_ACCESSDENIED);
}

/* 13 */
static void delete_file(INTRST intrst) {

	_SDACDS		sc;
	HDRVPATH	hdp;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}

	// ワイルドカードくるんで要修正…
	if ((hostdrvs_getrealpath(&hdp, intrst->filename_ptr) != SUCCESS) ||
		(hdp.di.attr & 0x10)) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	TRACEOUT(("delete_file %s - failed", hdp.path));
	fail(intrst, ERR_ACCESSDENIED);
}

/* 16 */
static void open_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	HDRVPATH	hdp;
	UINT		mode;
	FILEH		fh;
	HDRVFILE	hdf;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}
	fetch_sft(intrst, &sft);

	TRACEOUT(("open_file: %s %d", hdp.path, sft.open_mode[0] & 7));
	if ((is_wildcards(intrst->fcbname_ptr)) ||
		(hostdrvs_getrealpath(&hdp, intrst->filename_ptr) != SUCCESS) ||
		(hdp.di.attr & 0x10)) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	TRACEOUT(("open_file: %s -> %s %d", intrst->filename_ptr,
										hdp.path, sft.open_mode[0] & 7));
	switch(sft.open_mode[0] & 7) {
		case 1:	// write only
			mode = HDFMODE_WRITE;
			break;

		case 2:	// read/write
			mode = HDFMODE_READ | HDFMODE_WRITE;
			break;

		default:
			mode = HDFMODE_READ;
			break;
	}

	if (mode & HDFMODE_WRITE) {
		if (!IS_PERMITWRITE) {
			fail(intrst, ERR_ACCESSDENIED);
			return;
		}
		fh = file_open(hdp.path);
	}
	else {
		fh = file_open_rb(hdp.path);
	}
	if (fh == FILEH_INVALID) {
		TRACEOUT(("file open error!"));
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}

	hdf = hostdrvs_fhdlsea(hostdrv.fhdl);
	if (hdf == NULL) {
		file_close(fh);
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}

	hdf->hdl = (long)fh;
	hdf->mode = mode;
	file_cpyname(hdf->path, hdp.path, sizeof(hdf->path));

	fill_sft(intrst, &sft, listarray_getpos(hostdrv.fhdl, hdf), &hdp.di);
	init_sft(&sft);

	store_sft(intrst, &sft);
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 17 */
static void create_file(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	HDRVPATH	hdp;
	HDRVFILE	hdf;
	FILEH		fh;

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}
	fetch_sft(intrst, &sft);

	if ((is_wildcards(intrst->fcbname_ptr)) ||
		(hostdrvs_newrealpath(&hdp, intrst->filename_ptr) != SUCCESS) ||
		(hdp.di.attr & 0x10)) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	TRACEOUT(("create_file: %s -> %s %d", intrst->filename_ptr,
										hdp.path, sft.open_mode[0] & 7));

	if (!IS_PERMITWRITE) {
		fail(intrst, ERR_ACCESSDENIED);
		return;
	}

	hdf = hostdrvs_fhdlsea(hostdrv.fhdl);
	if (hdf == NULL) {
		fail(intrst, ERR_PATHNOTFOUND);
		return;
	}
	fh = file_create(hdp.path);
	if (fh == FILEH_INVALID) {
		TRACEOUT(("file create error!"));
		fail(intrst, ERR_ACCESSDENIED);
		return;
	}
	hdf->hdl = (long)fh;
	hdf->mode = HDFMODE_READ | HDFMODE_WRITE;
	file_cpyname(hdf->path, hdp.path, sizeof(hdf->path));

	fill_sft(intrst, &sft, listarray_getpos(hostdrv.fhdl, hdf), &hdp.di);
	init_sft(&sft);

	store_sft(intrst, &sft);
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 1B */
static void find_first(INTRST intrst) {

	_SDACDS		sc;
	HDRVPATH	hdp;
	LISTARRAY	flist;

	flist = hostdrv.flist;
	if (flist) {
		hostdrv.flist = NULL;
		hostdrv.flistpos = 0;
		listarray_destroy(flist);
	}

	if (pathishostdrv(intrst, &sc) != SUCCESS) {
		return;
	}

	if (*intrst->srch_attr_ptr == 0x08) {		// ボリュームラベル
		store_srch(intrst);
		store_dir(intrst, &hdd_volume);
	}
	else {
		if ((hostdrvs_getrealpath(&hdp, intrst->current_path) != SUCCESS) ||
			(!(hdp.di.attr & 0x10))) {
			fail(intrst, ERR_PATHNOTFOUND);
			return;
		}
		TRACEOUT(("find_first %s -> %s", intrst->current_path, hdp.path));
		hostdrv.flist = hostdrvs_getpathlist(hdp.path);
		hostdrv.flistpos = 0;
		if (find_file(intrst) != SUCCESS) {
			fail(intrst, ERR_PATHNOTFOUND);
			return;
		}
	}
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 1C */
static void find_next(INTRST intrst) {

	_SDACDS		sc;
	SRCHREC		srchrec;
//	char		*curpath;

	fetch_sda_currcds(&sc);
	setup_ptrs(intrst, &sc);

	srchrec = intrst->srchrec_ptr;
	if ((!(srchrec->drive_no & 0x40)) ||
		((srchrec->drive_no & 0x1f) != hostdrv.drive_no)) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
//	curpath = intrst->current_path;
	if (find_file(intrst) != SUCCESS) {
		fail(intrst, ERR_NOMOREFILES);
		return;
	}
	store_sda_currcds(&sc);
	succeed(intrst);
}

/* 21 */
// dos4以降呼ばれることはあんまない・・・
static void seek_fromend(INTRST intrst) {

	_SDACDS		sc;
	_SFTREC		sft;
	UINT16		reg;
	UINT32 		pos;
	UINT		file_size;

	fetch_sda_currcds(&sc);
	fetch_sft(intrst, &sft);

	if ((sft.dev_info_word[0] & 0x3f) != hostdrv.drive_no) {
		CPU_FLAG &= ~Z_FLAG;	// chain
		return;
	}
	reg = LOADINTELWORD(intrst->r.w.cx);
	pos = reg << 16;
	reg = LOADINTELWORD(intrst->r.w.dx);
	pos += reg;
	file_size = LOADINTELDWORD(sft.file_size);
	if (pos > file_size) {
		pos = file_size;
	}
	reg = (UINT16)(pos >> 16);
	STOREINTELWORD(intrst->r.w.dx, reg);
	reg = (UINT16)pos;
	STOREINTELWORD(intrst->r.w.ax, reg);
	pos = file_size - pos;
	STOREINTELDWORD(sft.file_pos, pos);

	store_sft(intrst, &sft);
	intrst->r.b.flag_l &= ~C_FLAG;
}


// ----

typedef void (*HDINTRFN)(INTRST intrst);

static const HDINTRFN intr_func[] = {
		inst_check,			/* 00 */
		NULL,	//	remove_dir,			/* 01 */
		NULL,
		NULL,	//	make_dir,			/* 03 */
		NULL,
		change_currdir,		/* 05 */
		close_file,			/* 06 */
		commit_file,		/* 07 */
		read_file,			/* 08 */
		write_file,			/* 09 */
		lock_file,			/* 0A */
		unlock_file,		/* 0B */
		NULL,	//	get_diskspace,		/* 0C */
		NULL,
		set_fileattr,		/* 0E */
		get_fileattr,		/* 0F */
		NULL,
		rename_file,		/* 11 */
		NULL,
		delete_file,		/* 13 */
		NULL,
		NULL,
		open_file,			/* 16 */
		create_file,		/* 17 */
		NULL,
		NULL,
		NULL,
		find_first,			/* 1B */
		find_next,			/* 1C */
		NULL,
		NULL,
		NULL,
		NULL,
		seek_fromend,		/* 21 */
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,	//	unknownfunc_2d,		/* 2D */
		NULL,	//	ext_openfile		/* 2E */
};


// ----

// 始めに一回だけ呼んでね(はーと
void hostdrv_initialize(void) {

	ZeroMemory(&hostdrv, sizeof(hostdrv));
	hostdrv.fhdl = listarray_new(sizeof(_HDRVFILE), 16);
	TRACEOUT(("hostdrv_initialize"));
}

// 終わりに一回だけ呼んでね(はーと
void hostdrv_deinitialize(void) {

	listarray_destroy(hostdrv.flist);
	hostdrvs_fhdlallclose(hostdrv.fhdl);
	listarray_destroy(hostdrv.fhdl);
	TRACEOUT(("hostdrv_deinitialize"));
}

// リセットルーチンで呼ぶべしべし
void hostdrv_reset(void) {

	hostdrv_deinitialize();
	hostdrv_initialize();
}


// ---- for np2sysp

BOOL hostdrv_mount(void) {

	if ((np2cfg.hdrvroot[0] == '\0') || (hostdrv.is_mount)) {
		return(FAILURE);
	}
	hostdrv.is_mount = TRUE;
	fetch_if4dos();
	return(SUCCESS);
}

void hostdrv_unmount(void) {

	if (hostdrv.is_mount) {
		hostdrv_reset();
	}
}

void hostdrv_intr(void) {

	_INTRST	intrst;

	ZeroMemory(&intrst, sizeof(intrst));
	intrst.is_chardev = (CPU_FLAG & C_FLAG) == 0;
	CPU_FLAG &= ~(C_FLAG | Z_FLAG);				// not fcb / chain

	if (!hostdrv.is_mount) {
		return;
	}

	fetch_intr_regs(&intrst);

	TRACEOUT(("hostdrv: AL=%.2x", intrst.r.b.al));

	if ((intrst.r.b.al >= sizeof(intr_func) / sizeof(HDINTRFN)) ||
		(intr_func[intrst.r.b.al] == NULL)) {
		return;
	}

	CPU_FLAG |= Z_FLAG;							// not chain
	(*intr_func[intrst.r.b.al])(&intrst);

	store_intr_regs(&intrst);
}

