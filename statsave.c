#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"commng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"board14.h"
#include	"amd98.h"
#include	"bios.h"
#include	"vram.h"
#include	"palettes.h"
#include	"maketext.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"font.h"
#include	"fddfile.h"
#include	"fdd_mtr.h"
#include	"sxsi.h"
#include	"calendar.h"
#include	"keydisp.h"
#include	"sasiio.h"
#include	"scsiio.h"
#include	"hostdrv.h"


#if defined(MACOS)
#define	CRCONST		str_cr
#elif defined(WIN32) || defined(X11) || defined(SLZAURUS)
#define	CRCONST		str_lf
#else
#define	CRCONST		str_crlf
#endif


typedef struct {
	char	name[16];
	char	vername[28];
	UINT32	ver;
} NP2FHDR;

typedef struct {
	char	index[10];
	UINT16	ver;
	UINT32	size;
} NP2FENT;

enum {
	STATFLAG_BIN			= 0,
	STATFLAG_TERM,
#if defined(CGWND_FONTPTR)
	STATFLAG_CGW,
#endif
	STATFLAG_CLOCK,
	STATFLAG_COM,
	STATFLAG_DISK,
	STATFLAG_DMA,
	STATFLAG_EGC,
	STATFLAG_EPSON,
	STATFLAG_EVT,
	STATFLAG_EXT,
	STATFLAG_FM,
	STATFLAG_GIJ,
#if defined(SUPPORT_HOSTDRV)
	STATFLAG_HDRV,
#endif
	STATFLAG_MEM
};

typedef struct {
	UINT32	id;
	void	*proc;
} PROCTBL;

typedef struct {
	UINT32	id;
	int		num;
} ENUMTBL;

#define	PROCID(a, b, c, d)	(((d) << 24) + ((c) << 16) + ((b) << 8) + (a))
#define	PROC2NUM(a, b)		proc2num(&(a), (b), sizeof(b)/sizeof(PROCTBL))
#define	NUM2PROC(a, b)		num2proc(&(a), (b), sizeof(b)/sizeof(PROCTBL))

#include "statsave.tbl"


extern	COMMNG	cm_mpu98;
extern	COMMNG	cm_rs232c;

typedef struct {
	char	*buf;
	int		remain;
} ERR_BUF;


// ----

// 関数ポインタを intに変更。
static BOOL proc2num(void *func, const PROCTBL *tbl, int size) {

	int		i;

	for (i=0; i<size; i++) {
		if (*(long *)func == (long)tbl->proc) {
			*(long *)func = (long)tbl->id;
			return(SUCCESS);
		}
		tbl++;
	}
	return(FAILURE);
}

static BOOL num2proc(void *func, const PROCTBL *tbl, int size) {

	int		i;

	for (i=0; i<size; i++) {
		if (*(long *)func == (long)tbl->id) {
			*(long *)func = (long)tbl->proc;
			return(SUCCESS);
		}
		tbl++;
	}
	return(FAILURE);
}


// ----

enum {
	SFFILEH_WRITE	= 0x0001,
	SFFILEH_BLOCK	= 0x0002,
	SFFILEH_ERROR	= 0x0004
};

typedef struct {
	_STFLAGH	sfh;
	UINT		stat;
	FILEH		fh;
	UINT		secpos;

//	long		pos;
//	long		bak;
//	long		next;
	NP2FHDR		f;
//	NP2FENT		p;
} _NP2FFILE, *NP2FFILE, _SFFILEH, *SFFILEH;

static SFFILEH statflag_open(const char *filename, char *err, int errlen) {

	FILEH	fh;
	SFFILEH	ret;

	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto sfo_err1;
	}
	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfo_err2;
	}
	if ((file_read(fh, &ret->f, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) &&
		(!memcmp(&ret->f, &np2flagdef, sizeof(np2flagdef)))) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		if ((err) && (errlen > 0)) {
			err[0] = '\0';
			ret->sfh.err = err;
			ret->sfh.errlen = errlen;
		}
		return(ret);
	}
	_MFREE(ret);

sfo_err2:
	file_close(fh);

sfo_err1:
	return(NULL);
}

static int statflag_closesection(SFFILEH sffh) {

	UINT	leng;
	BYTE	zero[16];

	if (sffh == NULL) {
		goto sfcs_err1;
	}
	if (sffh->stat == (SFFILEH_BLOCK | SFFILEH_WRITE)) {
		leng = (0 - sffh->sfh.hdr.size) & 15;
		if (leng) {
			ZeroMemory(zero, sizeof(zero));
			if (file_write(sffh->fh, zero, leng) != leng) {
				goto sfcs_err2;
			}
		}
		if ((file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) ||
			(file_write(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												!= sizeof(sffh->sfh.hdr))) {
			goto sfcs_err2;
		}
	}
	if (sffh->stat & SFFILEH_BLOCK) {
		sffh->stat &= ~SFFILEH_BLOCK;
		sffh->secpos += sizeof(sffh->sfh.hdr) +
									((sffh->sfh.hdr.size + 15) & (~15));
		if (file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) {
			goto sfcs_err2;
		}
	}
	return(STATFLAG_SUCCESS);

sfcs_err2:
	sffh->stat = SFFILEH_ERROR;

sfcs_err1:
	return(STATFLAG_FAILURE);
}

static int statflag_readsection(SFFILEH sffh) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if ((sffh->stat == 0) &&
		(file_read(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												== sizeof(sffh->sfh.hdr))) {
		sffh->stat = SFFILEH_BLOCK;
		sffh->sfh.pos = 0;
		return(STATFLAG_SUCCESS);
	}
	sffh->stat = SFFILEH_ERROR;
	return(STATFLAG_FAILURE);
}

int statflag_read(STFLAGH sfh, void *buf, UINT size) {

	if ((sfh == NULL) || (buf == NULL) ||
		((sfh->pos + size) > sfh->hdr.size)) {
		goto sfr_err;
	}
	if (size) {
		if (file_read(((SFFILEH)sfh)->fh, buf, size) != size) {
			goto sfr_err;
		}
		sfh->pos += size;
	}
	return(STATFLAG_SUCCESS);

sfr_err:
	return(STATFLAG_FAILURE);
}

static SFFILEH statflag_create(const char *filename) {

	SFFILEH	ret;
	FILEH	fh;

	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfc_err1;
	}
	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto sfc_err2;
	}
	if (file_write(fh, &np2flagdef, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->stat = SFFILEH_WRITE;
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		return(ret);
	}
	file_close(fh);
	file_delete(filename);

sfc_err2:
	_MFREE(ret);

sfc_err1:
	return(NULL);
}

static int statflag_createsection(SFFILEH sffh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if (sffh->stat != SFFILEH_WRITE) {
		sffh->stat = SFFILEH_ERROR;
		return(STATFLAG_FAILURE);
	}
	CopyMemory(sffh->sfh.hdr.index, tbl->index, sizeof(sffh->sfh.hdr.index));
	sffh->sfh.hdr.ver = tbl->ver;
	sffh->sfh.hdr.size = 0;
	return(STATFLAG_SUCCESS);
}

int statflag_write(STFLAGH sfh, const void *buf, UINT size) {

	SFFILEH	sffh;

	if (sfh == NULL) {
		goto sfw_err1;
	}
	sffh = (SFFILEH)sfh;
	if (!(sffh->stat & SFFILEH_WRITE)) {
		goto sfw_err2;
	}
	if (!(sffh->stat & SFFILEH_BLOCK)) {
		sffh->stat |= SFFILEH_BLOCK;
		sfh->pos = 0;
		if (file_write(sffh->fh, &sfh->hdr, sizeof(sfh->hdr))
														!= sizeof(sfh->hdr)) {
			goto sfw_err2;
		}
	}
	if (size) {
		if ((buf == NULL) || (file_write(sffh->fh, buf, size) != size)) {
			goto sfw_err2;
		}
		sfh->pos += size;
		if (sfh->hdr.size < sfh->pos) {
			sfh->hdr.size = sfh->pos;
		}
	}
	return(STATFLAG_SUCCESS);

sfw_err2:
	sffh->stat = SFFILEH_ERROR;

sfw_err1:
	return(STATFLAG_FAILURE);
}

static void statflag_close(SFFILEH sffh) {

	if (sffh) {
		statflag_closesection(sffh);
		file_close(sffh->fh);
		_MFREE(sffh);
	}
}

void statflag_seterr(STFLAGH sfh, const char *str) {

	if ((sfh) && (sfh->errlen)) {
		milstr_ncat(sfh->err, str, sfh->errlen);
		milstr_ncat(sfh->err, CRCONST, sfh->errlen);
	}
}




#if 0
static int flagopen(NP2FFILE f, const char *filename, ERR_BUF *e) {

	if (f) {
		f->fh = file_open(filename);
		if (f->fh == FILEH_INVALID) {
			return(STATFLAG_FAILURE);
		}
		if (file_read(f->fh, &f->f, sizeof(NP2FHDR))
												!= sizeof(NP2FHDR)) {
			file_close(f->fh);
			return(STATFLAG_FAILURE);
		}
		f->pos = sizeof(NP2FHDR);
		if (strcmp(f->f.name, np2flagdef.name)) {
			file_close(f->fh);
			return(STATFLAG_FAILURE);
		}
		if (strcmp(f->f.vername, np2flagdef.vername)) {
#if 1
			file_close(f->fh);
			return(STATFLAG_FAILURE);
#else
			return(STATFLAG_VERSION);			// 他形式を読むのであれば…
#endif
		}
		if (f->f.ver != np2flagdef.ver) {
#if 1
			file_close(f->fh);
			return(STATFLAG_FAILURE);
#else
			return(STATFLAG_VERSION);			// 他verを読むのであれば…
#endif
		}
		return(STATFLAG_SUCCESS);
	}
	(void)e;
	return(STATFLAG_FAILURE);
}

static int flagcreate(NP2FFILE f, const char *filename) {

	if (f) {
		f->fh = file_create(filename);
		if (f->fh == FILEH_INVALID) {
			return(STATFLAG_FAILURE);
		}
		if (file_write(f->fh, &np2flagdef, sizeof(NP2FHDR))
														!= sizeof(NP2FHDR)) {
			file_close(f->fh);
			return(STATFLAG_FAILURE);
		}
		f->pos = sizeof(NP2FHDR);
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagload_create(NP2FFILE f) {

	if (f) {
		ZeroMemory(&f->p, sizeof(NP2FENT));
		if (f->pos & 15) {
			f->pos += 15;
			f->pos &= ~0xf;
			if (file_seek(f->fh, f->pos, 0) != f->pos) {
				return(STATFLAG_FAILURE);
			}
		}
		if (file_read(f->fh, &f->p, sizeof(NP2FENT)) != sizeof(NP2FENT)) {
			return(STATFLAG_FAILURE);
		}
		f->pos += sizeof(NP2FENT);
		f->next = f->pos + f->p.size;
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagload_load(NP2FFILE f, void *buf, UINT size) {

	if (f && buf && size && (file_read(f->fh, buf, size) == size)) {
		f->pos += size;
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagload_close(NP2FFILE f) {

	if (file_seek(f->fh, f->next, 0) != f->next) {
		return(STATFLAG_FAILURE);
	}
	f->pos = f->next;
	return(STATFLAG_SUCCESS);
}

static int flagsave_create(NP2FFILE f, const SFENTRY *t) {

	if (f && t) {
		int		len;
		ZeroMemory(&f->p, sizeof(NP2FENT));
		if (f->pos & 15) {
			UINT rem;
			rem = 16 - (f->pos & 15);
			if (file_write(f->fh, &f->p, rem) != rem) {
				return(STATFLAG_FAILURE);
			}
			f->pos += rem;
		}
		f->bak = f->pos;

		len = strlen(t->index);
		if (len >= 10) {
			len = 10;
		}
		if (len) {
			CopyMemory(f->p.index, t->index, len);
		}
		f->p.ver = t->ver;
		if (file_write(f->fh, &f->p, sizeof(NP2FENT)) != sizeof(NP2FENT)) {
			return(STATFLAG_FAILURE);
		}
		f->pos += sizeof(NP2FENT);
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagsave_save(NP2FFILE f, void *buf, UINT size) {

	if (f && buf && size && (file_write(f->fh, buf, size) == size)) {
		f->pos += size;
		f->p.size += size;
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagsave_close(NP2FFILE f) {

	if (!f) {
		goto fs_closeerr;
	}
	if (file_seek(f->fh, f->bak, 0) != f->bak) {
		goto fs_closeerr;
	}
	if (file_write(f->fh, &f->p, sizeof(NP2FENT)) != sizeof(NP2FENT)) {
		goto fs_closeerr;
	}
	if (file_seek(f->fh, f->pos, 0) == f->pos) {
		return(STATFLAG_SUCCESS);
	}

fs_closeerr:
	return(STATFLAG_FAILURE);
}

static void flagclose(NP2FFILE f) {

	if (f) {
		file_close(f->fh);
	}
}
#endif


// ---- common

static int flagsave_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_write(sfh, tbl->arg1, tbl->arg2));
}

static int flagload_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_read(sfh, tbl->arg1, tbl->arg2));
}


// ----- clock

static int flagload_clock(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, tbl->arg1, tbl->arg2);
	sound_changeclock();
	beep_changeclock();
	return(ret);
}


// ---- memory

static int flagsave_mem(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_write(sfh, mem, 0x110000);
	ret |= statflag_write(sfh, mem + VRAM1_B, 0x18000);
	ret |= statflag_write(sfh, mem + VRAM1_E, 0x8000);
	(void)tbl;
	return(ret);
}

static int flagload_mem(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, mem, 0x110000);
	ret |= statflag_read(sfh, mem + VRAM1_B, 0x18000);
	ret |= statflag_read(sfh, mem + VRAM1_E, 0x8000);
	(void)tbl;
	return(ret);
}


// ---- ext memory

static int flagsave_ext(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_write(sfh, &extmem, sizeof(extmem));
	if (CPU_EXTMEM) {
		ret = statflag_write(sfh, CPU_EXTMEM, CPU_EXTMEMSIZE);
	}
	(void)tbl;
	return(ret);
}

static int flagload_ext(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	UINT	pagemax = 0;

	ret = statflag_read(sfh, &extmem, sizeof(extmem));
	if (extmem.maxmem) {
		if (extmem.maxmem > (13+1)) {
			extmem.maxmem = (13+1);
		}
		if (!extmemmng_realloc(extmem.maxmem - 1)) {
			pagemax = (extmem.maxmem - 1) << 8;
			if (CPU_EXTMEM) {
				ret |= statflag_read(sfh, CPU_EXTMEM, CPU_EXTMEMSIZE);
			}
		}
		else {
			extmem.maxmem = 0;
		}
	}
	for (i=0; i<4; i++) {
		if (extmem.page[i] < pagemax) {
			extmem.pageptr[i] = CPU_EXTMEM + (extmem.page[i] << 12);
		}
		else {
			extmem.pageptr[i] = mem + 0xc0000 + (i << 14);
		}
	}
	(void)tbl;
	return(ret);
}


// ---- cg window

#if defined(CGWND_FONTPTR)
static int flagsave_cgwnd(STFLAGH sfh, const SFENTRY *tbl) {

	_CGWINDOW	cgwnd;

	cgwnd = cgwindow;
	cgwnd.fontlow -= (long)fontrom;
	cgwnd.fonthigh -= (long)fontrom;
	(void)tbl;
	return(statflag_write(sfh, &cgwindow, sizeof(cgwindow)));
}

static int flagload_cgwnd(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, &cgwindow, sizeof(cgwindow));
	cgwindow.fontlow += (long)fontrom;
	cgwindow.fonthigh += (long)fontrom;
	(void)tbl;
	return(ret);
}
#endif


// ---- dma

static int flagsave_dma(STFLAGH sfh, const SFENTRY *tbl) {

	int			i;
	_DMAC		dmabak;

	dmabak = dmac;
	for (i=0; i<4; i++) {
		if ((PROC2NUM(dmabak.dmach[i].proc.outproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].proc.inproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].proc.extproc, dmaproc))) {
			return(STATFLAG_FAILURE);
		}
	}
	(void)tbl;
	return(statflag_write(sfh, &dmabak, sizeof(dmabak)));
}

static int flagload_dma(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;

	ret = statflag_read(sfh, &dmac, sizeof(dmac));

	for (i=0; i<4; i++) {
		if (NUM2PROC(dmac.dmach[i].proc.outproc, dmaproc)) {
			dmac.dmach[i].proc.outproc = dma_dummyout;
			ret |= STATFLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].proc.inproc, dmaproc)) {
			dmac.dmach[i].proc.inproc = dma_dummyin;
			ret |= STATFLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].proc.extproc, dmaproc)) {
			dmac.dmach[i].proc.extproc = dma_dummyproc;
			ret |= STATFLAG_WARNING;
		}
	}
	(void)tbl;
	return(ret);
}


// ---- egc

static int flagsave_egc(STFLAGH sfh, const SFENTRY *tbl) {

	_EGC	egcbak;

	egcbak = egc;
	egcbak.inptr -= (long)egc.buf;
	egcbak.outptr -= (long)egc.buf;
	(void)tbl;
	return(statflag_write(sfh, &egcbak, sizeof(egcbak)));
}

static int flagload_egc(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, &egc, sizeof(egc));
	egc.inptr += (long)egc.buf;
	egc.outptr += (long)egc.buf;
	(void)tbl;
	return(ret);
}


// ---- epson

static int flagsave_epson(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	if (!(pccore.model & PCMODEL_EPSON)) {
		return(STATFLAG_SUCCESS);
	}
	ret = statflag_write(sfh, &epsonio, sizeof(epsonio));
	ret |= statflag_write(sfh, mem + 0x1c0000, 0x8000);
	ret |= statflag_write(sfh, mem + 0x1e8000, 0x18000);
	(void)tbl;
	return(ret);
}

static int flagload_epson(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, &epsonio, sizeof(epsonio));
	ret |= statflag_read(sfh, mem + 0x1c0000, 0x8000);
	ret |= statflag_read(sfh, mem + 0x1e8000, 0x18000);
	(void)tbl;
	return(ret);
}


// ---- event

typedef struct {
	UINT		readyevents;
	UINT		waitevents;
} NEVTSAVE;

typedef struct {
	UINT32		id;
	SINT32		clock;
	UINT32		flag;
	NEVENTCB	proc;
} NEVTITEM;

static int nevent_write(STFLAGH sfh, int num) {

	NEVTITEM	nit;
	UINT		i;

	ZeroMemory(&nit, sizeof(nit));
	for (i=0; i<sizeof(evtnum)/sizeof(ENUMTBL); i++) {
		if (evtnum[i].num == num) {
			nit.id = evtnum[i].id;
			break;
		}
	}
	nit.clock = nevent.item[num].clock;
	nit.flag = nevent.item[num].flag;
	nit.proc = nevent.item[num].proc;
	if (PROC2NUM(nit.proc, evtproc)) {
		nit.proc = NULL;
	}
	return(statflag_write(sfh, &nit, sizeof(nit)));
}

static int flagsave_evt(STFLAGH sfh, const SFENTRY *tbl) {

	NEVTSAVE	nevt;
	int			ret;
	UINT		i;

	nevt.readyevents = nevent.readyevents;
	nevt.waitevents = nevent.waitevents;

	ret = statflag_write(sfh, &nevt, sizeof(nevt));
	for (i=0; i<nevt.readyevents; i++) {
		ret |= nevent_write(sfh, nevent.level[i]);
	}
	for (i=0; i<nevt.waitevents; i++) {
		ret |= nevent_write(sfh, nevent.waitevent[i]);
	}
	(void)tbl;
	return(ret);
}

static int nevent_read(STFLAGH sfh, UINT *tbl, UINT *pos) {

	int			ret;
	NEVTITEM	nit;
	UINT		i;
	UINT		num;

	ret = statflag_read(sfh, &nit, sizeof(nit));

	for (i=0; i<sizeof(evtnum)/sizeof(ENUMTBL); i++) {
		if (nit.id == evtnum[i].id) {
			break;
		}
	}
	if (i < (sizeof(evtnum)/sizeof(ENUMTBL))) {
		num = evtnum[i].num;
		nevent.item[num].clock = nit.clock;
		nevent.item[num].flag = nit.flag;
		nevent.item[num].proc = nit.proc;
		if (NUM2PROC(nevent.item[num].proc, evtproc)) {
			ret |= STATFLAG_WARNING;
		}
		else {
			tbl[*pos] = num;
			(*pos)++;
		}
	}
	else {
		ret |= STATFLAG_WARNING;
	}
	return(ret);
}

static int flagload_evt(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	NEVTSAVE	nevt;
	UINT		i;

	ret = statflag_read(sfh, &nevt, sizeof(nevt));

	nevent.readyevents = 0;
	nevent.waitevents = 0;

	for (i=0; i<nevt.readyevents; i++) {
		ret |= nevent_read(sfh, nevent.level, &nevent.readyevents);
	}
	for (i=0; i<nevt.waitevents; i++) {
		ret |= nevent_read(sfh, nevent.waitevent, &nevent.waitevents);
	}
	(void)tbl;
	return(ret);
}


// ---- gaiji

static int flagsave_gij(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	int		j;
const BYTE	*fnt;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<2; i++) {
		fnt = fontrom + ((0x56 + (i << 7)) << 4);
		for (j=0; j<0x80; j++) {
			ret |= statflag_write(sfh, fnt, 32);
			fnt += 0x1000;
		}
	}
	(void)tbl;
	return(ret);
}

static int flagload_gij(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	int		j;
	BYTE	*fnt;

	ret = 0;
	for (i=0; i<2; i++) {
		fnt = fontrom + ((0x56 + (i << 7)) << 4);
		for (j=0; j<0x80; j++) {
			ret |= statflag_read(sfh, fnt, 32);
			fnt += 0x1000;
		}
	}
	(void)tbl;
	return(ret);
}


// ---- FM

enum {
	FLAG_MG			= 0x0001,
	FLAG_FM1A		= 0x0002,
	FLAG_FM1B		= 0x0004,
	FLAG_FM2A		= 0x0008,
	FLAG_FM2B		= 0x0010,
	FLAG_PSG1		= 0x0020,
	FLAG_PSG2		= 0x0040,
	FLAG_PSG3		= 0x0080,
	FLAG_RHYTHM		= 0x0100,
	FLAG_ADPCM		= 0x0200,
	FLAG_PCM86		= 0x0400,
	FLAG_CS4231		= 0x0800
};

typedef struct {
	BYTE	keyreg[OPNCH_MAX];
	BYTE	extop[4];
} OPNKEY;

static int flagsave_fm(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	UINT	saveflg;
	OPNKEY	opnkey;

	switch(usesound) {
		case 0x01:
			saveflg = FLAG_MG;
			break;

		case 0x02:
			saveflg = FLAG_FM1A | FLAG_PSG1;
			break;

		case 0x04:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_PCM86;
			break;

		case 0x06:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_FM2A | FLAG_PSG1 |
										FLAG_PSG2 | FLAG_RHYTHM | FLAG_PCM86;
			break;

		case 0x08:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_CS4231;
			break;

		case 0x14:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_ADPCM | FLAG_PCM86;
			break;

		case 0x20:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_ADPCM;
			break;

		case 0x40:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_FM2A | FLAG_FM2B |
										FLAG_PSG1 | FLAG_RHYTHM | FLAG_ADPCM;
			break;

		case 0x80:
			saveflg = FLAG_PSG1 | FLAG_PSG2 | FLAG_PSG3;
			break;

		default:
			saveflg = 0;
			break;
	}

	ret = statflag_write(sfh, &usesound, sizeof(usesound));
	if (saveflg & FLAG_MG) {
		ret |= statflag_write(sfh, &musicgen, sizeof(musicgen));
	}
	if (saveflg & FLAG_FM1A) {
		ret |= statflag_write(sfh, &fmtimer, sizeof(fmtimer));
		ret |= statflag_write(sfh, &opn, sizeof(opn));
		CopyMemory(opnkey.keyreg, opngen.keyreg, sizeof(opngen.keyreg));
		opnkey.extop[0] = opnch[2].extop;
		opnkey.extop[1] = opnch[5].extop;
		opnkey.extop[2] = opnch[8].extop;
		opnkey.extop[3] = opnch[11].extop;
		ret |= statflag_write(sfh, &opnkey, sizeof(opnkey));
	}
	if (saveflg & FLAG_PSG1) {
		ret |= statflag_write(sfh, &psg1.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_PSG2) {
		ret |= statflag_write(sfh, &psg2.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_PSG3) {
		ret |= statflag_write(sfh, &psg3.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_ADPCM) {
		ret |= statflag_write(sfh, &adpcm, sizeof(adpcm));
	}
	if (saveflg & FLAG_PCM86) {
		ret |= statflag_write(sfh, &pcm86, sizeof(pcm86));
	}
	if (saveflg & FLAG_CS4231) {
		ret |= statflag_write(sfh, &cs4231, sizeof(cs4231));
	}
	(void)tbl;
	return(ret);
}

static void play_fmreg(BYTE num, UINT reg) {

	UINT	chbase;
	UINT	i;

	chbase = num * 3;
	for (i=0x30; i<0xa0; i++) {
		opngen_setreg((BYTE)chbase, (BYTE)i, opn.reg[reg + i]);
	}
	for (i=0xb7; i>=0xa0; i--) {
		opngen_setreg((BYTE)chbase, (BYTE)i, opn.reg[reg + i]);
	}
	for (i=0; i<3; i++) {
		opngen_keyon(chbase + i, opngen.keyreg[chbase + i]);
	}
}

static void play_psgreg(PSGGEN psg) {

	BYTE	i;

	for (i=0; i<0x0e; i++) {
		psggen_setreg(psg, i, ((BYTE *)&psg->reg)[i]);
	}
}

static int flagload_fm(STFLAGH sfh, const SFENTRY *t) {

	int		ret;
	UINT	saveflg;
	OPNKEY	opnkey;
	UINT	fmreg1a;
	UINT	fmreg1b;
	UINT	fmreg2a;
	UINT	fmreg2b;

	opngen_reset();
	psggen_reset(&psg1);
	psggen_reset(&psg2);
	psggen_reset(&psg3);
	rhythm_reset(&rhythm);
	adpcm_reset(&adpcm);
	pcm86_reset();
	cs4231_reset();

	ret = statflag_read(sfh, &usesound, sizeof(usesound));
	fmboard_reset((BYTE)usesound);

	fmreg1a = 0x000;
	fmreg1b = 0x100;
	fmreg2a = 0x200;
	fmreg2b = 0x300;
	switch(usesound) {
		case 0x01:
			saveflg = FLAG_MG;
			break;

		case 0x02:
			saveflg = FLAG_FM1A | FLAG_PSG1;
			break;

		case 0x04:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_PCM86;
			break;

		case 0x06:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_FM2A | FLAG_PSG1 |
										FLAG_PSG2 | FLAG_RHYTHM | FLAG_PCM86;
			fmreg1a = 0x200;	// 逆転してるのん…
			fmreg1b = 0x000;
			fmreg2a = 0x100;
			break;

		case 0x08:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_CS4231;
			break;

		case 0x14:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_ADPCM | FLAG_PCM86;
			break;

		case 0x20:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_PSG1 | FLAG_RHYTHM |
										FLAG_ADPCM;
			break;

		case 0x40:
			saveflg = FLAG_FM1A | FLAG_FM1B | FLAG_FM2A | FLAG_FM2B |
										FLAG_PSG1 | FLAG_RHYTHM | FLAG_ADPCM;
			break;

		case 0x80:
			saveflg = FLAG_PSG1 | FLAG_PSG2 | FLAG_PSG3;
			break;

		default:
			saveflg = 0;
			break;
	}

	if (saveflg & FLAG_MG) {
		ret |= statflag_read(sfh, &musicgen, sizeof(musicgen));
		board14_allkeymake();
	}

	if (saveflg & FLAG_FM1A) {
		ret |= statflag_read(sfh, &fmtimer, sizeof(fmtimer));
		ret |= statflag_read(sfh, &opn, sizeof(opn));
		ret |= statflag_read(sfh, &opnkey, sizeof(opnkey));
		CopyMemory(opngen.keyreg, &opnkey.keyreg, sizeof(opngen.keyreg));
		opnch[2].extop = opnkey.extop[0];
		opnch[5].extop = opnkey.extop[1];
		opnch[8].extop = opnkey.extop[2];
		opnch[11].extop = opnkey.extop[3];
	}
	if (saveflg & FLAG_PSG1) {
		ret |= statflag_read(sfh, &psg1.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_PSG2) {
		ret |= statflag_read(sfh, &psg2.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_PSG3) {
		ret |= statflag_read(sfh, &psg3.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_ADPCM) {
		ret |= statflag_read(sfh, &adpcm, sizeof(adpcm));
	}
	if (saveflg & FLAG_PCM86) {
		ret |= statflag_read(sfh, &pcm86, sizeof(pcm86));
	}
	if (saveflg & FLAG_CS4231) {
		ret |= statflag_read(sfh, &cs4231, sizeof(cs4231));
		cs4231.proc = cs4231dec[cs4231.reg.datafmt >> 4];
	}

	// 復元。
	rhythm_update(&rhythm);
	adpcm_update(&adpcm);
	pcm86gen_update();
	if (saveflg & FLAG_PCM86) {
		fmboard_extenable((BYTE)(pcm86.extfunc & 1));
	}
	if (saveflg & FLAG_CS4231) {
		fmboard_extenable((BYTE)(cs4231.extfunc & 1));
	}

	if (saveflg & FLAG_FM1A) {
		play_fmreg(0, fmreg1a);
	}
	if (saveflg & FLAG_FM1B) {
		play_fmreg(1, fmreg1b);
	}
	if (saveflg & FLAG_FM2A) {
		play_fmreg(2, fmreg2a);
	}
	if (saveflg & FLAG_FM2B) {
		play_fmreg(3, fmreg2b);
	}
	if (saveflg & FLAG_PSG1) {
		play_psgreg(&psg1);
	}
	if (saveflg & FLAG_PSG2) {
		play_psgreg(&psg2);
	}
	if (saveflg & FLAG_PSG3) {
		play_psgreg(&psg3);
	}
	(void)t;
	return(ret);
}


// ---- disk

typedef struct {
	char	path[MAX_PATH];
	int		readonly;
	DOSDATE	date;
	DOSTIME	time;
} STATDISK;

static const char str_fddx[] = "FDD%u";
static const char str_sasix[] = "SASI%u";
static const char str_scsix[] = "SCSI%u";
static const char str_updated[] = "%s: updated";
static const char str_notfound[] = "%s: not found";

static int disksave(STFLAGH sfh, const char *path, int readonly) {

	STATDISK	st;
	FILEH		fh;

	ZeroMemory(&st, sizeof(st));
	if ((path) && (path[0])) {
		file_cpyname(st.path, path, sizeof(st.path));
		st.readonly = readonly;
		fh = file_open_rb(path);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &st.date, &st.time);
			file_close(fh);
		}
	}
	return(statflag_write(sfh, &st, sizeof(st)));
}

static int flagsave_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	BYTE	i;

	sxsi_flash();
	ret = STATFLAG_SUCCESS;
	for (i=0; i<4; i++) {
		ret |= disksave(sfh, fdd_diskname(i), fdd_diskprotect(i));
	}
	for (i=0x00; i<0x02; i++) {
		ret |= disksave(sfh, sxsi_getname(i), 0);
	}
	for (i=0x20; i<0x24; i++) {
		ret |= disksave(sfh, sxsi_getname(i), 0);
	}
	(void)tbl;
	return(ret);
}

static int diskcheck(STFLAGH sfh, const char *name) {

	int			ret;
	FILEH		fh;
	STATDISK	st;
	char		buf[256];
	DOSDATE		date;
	DOSTIME		time;

	ret = statflag_read(sfh, &st, sizeof(st));
	if (st.path[0]) {
		fh = file_open_rb(st.path);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &date, &time);
			file_close(fh);
			if ((memcmp(&st.date, &date, sizeof(date))) ||
				(memcmp(&st.time, &time, sizeof(time)))) {
				ret |= STATFLAG_DISKCHG;
				SPRINTF(buf, str_updated, name);
				statflag_seterr(sfh, buf);
			}
		}
		else {
			ret |= STATFLAG_DISKCHG;
			SPRINTF(buf, str_notfound, name);
			statflag_seterr(sfh, buf);
		}
	}
	return(ret);
}

static int flagcheck_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	char	buf[8];

	ret = 0;
	for (i=0; i<4; i++) {
		SPRINTF(buf, str_fddx, i+1);
		ret |= diskcheck(sfh, buf);
	}
	sxsi_flash();
	for (i=0; i<2; i++) {
		SPRINTF(buf, str_sasix, i+1);
		ret |= diskcheck(sfh, buf);
	}
	for (i=0; i<4; i++) {
		SPRINTF(buf, str_scsix, i);
		ret |= diskcheck(sfh, buf);
	}
	(void)tbl;
	return(ret);
}

static int flagload_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	UINT8		i;
	STATDISK	st;

	ret = 0;
	for (i=0; i<4; i++) {
		ret |= statflag_read(sfh, &st, sizeof(st));
		if (st.path[0]) {
			fdd_set(i, st.path, FTYPE_NONE, st.readonly);
		}
	}
	sxsi_trash();
	for (i=0x00; i<0x02; i++) {
		ret |= statflag_read(sfh, &st, sizeof(st));
		if (st.path[0]) {
			sxsi_hddopen(i, st.path);
		}
	}
	for (i=0x20; i<0x24; i++) {
		ret |= statflag_read(sfh, &st, sizeof(st));
		if (st.path[0]) {
			sxsi_hddopen(i, st.path);
		}
	}
	(void)tbl;
	return(ret);
}


// ---- com

static int flagsave_com(STFLAGH sfh, const SFENTRY *tbl) {

	UINT	device;
	COMMNG	cm;
	int		ret;
	COMFLAG	flag;

	device = (UINT)(long)tbl->arg1;
	switch(device) {
		case 0:
			cm = cm_mpu98;
			break;

		case 1:
			cm = cm_rs232c;
			break;

		default:
			cm = NULL;
			break;
	}
	ret = STATFLAG_SUCCESS;
	if (cm) {
		flag = (COMFLAG)cm->msg(cm, COMMSG_GETFLAG, 0);
		if (flag) {
			ret |= statflag_write(sfh, flag, flag->size);
			_MFREE(flag);
		}
	}
	return(ret);
}

static int flagload_com(STFLAGH sfh, const SFENTRY *tbl) {

	UINT		device;
	COMMNG		cm;
	int			ret;
	_COMFLAG	fhdr;
	COMFLAG		flag;

	ret = statflag_read(sfh, &fhdr, sizeof(fhdr));
	if (ret != STATFLAG_SUCCESS) {
		goto flcom_err1;
	}
	if (fhdr.size < sizeof(fhdr)) {
		goto flcom_err1;
	}
	flag = (COMFLAG)_MALLOC(fhdr.size, "com stat flag");
	if (flag == NULL) {
		goto flcom_err1;
	}
	CopyMemory(flag, &fhdr, sizeof(fhdr));
	ret |= statflag_read(sfh, flag + 1, fhdr.size - sizeof(fhdr));
	if (ret != STATFLAG_SUCCESS) {
		goto flcom_err2;
	}

	device = (UINT)(long)tbl->arg1;
	switch(device) {
		case 0:
			commng_destroy(cm_mpu98);
			cm = commng_create(COMCREATE_MPU98II);
			cm_mpu98 = cm;
			break;

		case 1:
			commng_destroy(cm_rs232c);
			cm = commng_create(COMCREATE_SERIAL);
			cm_rs232c = cm;
			break;

		default:
			cm = NULL;
			break;
	}
	if (cm) {
		cm->msg(cm, COMMSG_SETFLAG, (long)flag);
	}

flcom_err2:
	_MFREE(flag);

flcom_err1:
	return(ret);
}


// ----

static int flagcheck_versize(STFLAGH sfh, const SFENTRY *tbl) {

	if ((sfh->hdr.ver == tbl->ver) && (sfh->hdr.size == tbl->arg2)) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagcheck_veronly(STFLAGH sfh, const SFENTRY *tbl) {

	if (sfh->hdr.ver == tbl->ver) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}


// ----

int statsave_save(const char *filename) {

	SFFILEH		sffh;
	int			ret;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_create(filename);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	ret = STATFLAG_SUCCESS;
	tbl = np2tbl;
	tblterm = tbl + (sizeof(np2tbl)/sizeof(SFENTRY));
	while(tbl < tblterm) {
		ret |= statflag_createsection(sffh, tbl);
		switch(tbl->type) {
			case STATFLAG_BIN:
			case STATFLAG_CLOCK:
			case STATFLAG_TERM:
				ret |= flagsave_common(&sffh->sfh, tbl);
				break;

#if defined(CGWND_FONTPTR)
			case STATFLAG_CGW:
				ret |= flagsave_cgwnd(&sffh->sfh, tbl);
				break;
#endif

			case STATFLAG_COM:
				ret |= flagsave_com(&sffh->sfh, tbl);
				break;

			case STATFLAG_DISK:
				ret |= flagsave_disk(&sffh->sfh, tbl);
				break;

			case STATFLAG_DMA:
				ret |= flagsave_dma(&sffh->sfh, tbl);
				break;

			case STATFLAG_EGC:
				ret |= flagsave_egc(&sffh->sfh, tbl);
				break;

			case STATFLAG_EPSON:
				ret |= flagsave_epson(&sffh->sfh, tbl);
				break;

			case STATFLAG_EVT:
				ret |= flagsave_evt(&sffh->sfh, tbl);
				break;

			case STATFLAG_EXT:
				ret |= flagsave_ext(&sffh->sfh, tbl);
				break;

			case STATFLAG_FM:
				ret |= flagsave_fm(&sffh->sfh, tbl);
				break;

			case STATFLAG_GIJ:
				ret |= flagsave_gij(&sffh->sfh, tbl);
				break;

#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
				ret |= hostdrv_sfsave(&sffh->sfh, tbl);
				break;
#endif

			case STATFLAG_MEM:
				ret |= flagsave_mem(&sffh->sfh, tbl);
				break;
		}
		tbl++;
	}
	statflag_close(sffh);
	return(ret);
}

int statsave_check(const char *filename, char *buf, int size) {

	SFFILEH		sffh;
	int			ret;
	BOOL		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, buf, size);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	done = FALSE;
	ret = STATFLAG_SUCCESS;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = np2tbl;
		tblterm = tbl + (sizeof(np2tbl)/sizeof(SFENTRY));
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, 10)) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
#if defined(CGWND_FONTPTR)
				case STATFLAG_CGW:
#endif
				case STATFLAG_CLOCK:
				case STATFLAG_MEM:
					ret |= flagcheck_versize(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

				case STATFLAG_COM:
				case STATFLAG_DMA:
				case STATFLAG_EGC:
				case STATFLAG_EPSON:
				case STATFLAG_EVT:
				case STATFLAG_EXT:
				case STATFLAG_GIJ:
				case STATFLAG_FM:
#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
#endif
					ret |= flagcheck_veronly(&sffh->sfh, tbl);
					break;

				case STATFLAG_DISK:
					ret |= flagcheck_disk(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);
	return(ret);
}

int statsave_load(const char *filename) {

	SFFILEH		sffh;
	int			ret;
	BOOL		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, NULL, 0);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	soundmng_stop();
	rs232c_midipanic();
	mpu98ii_midipanic();
	pc9861k_midipanic();
	sound_reset();
#if defined(SUPPORT_WAVEMIX)
	wavemix_bind();
#endif
	fmboard_reset(0);

	done = FALSE;
	ret = STATFLAG_SUCCESS;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = np2tbl;
		tblterm = tbl + (sizeof(np2tbl)/sizeof(SFENTRY));
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, 10)) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
					ret |= flagload_common(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

#if defined(CGWND_FONTPTR)
				case STATFLAG_CGW:
					ret |= flagload_cgwnd(&sffh->sfh, tbl);
					break;
#endif

				case STATFLAG_CLOCK:
					ret |= flagload_clock(&sffh->sfh, tbl);
					break;

				case STATFLAG_COM:
					ret |= flagload_com(&sffh->sfh, tbl);
					break;

				case STATFLAG_DISK:
					ret |= flagload_disk(&sffh->sfh, tbl);
					break;

				case STATFLAG_DMA:
					ret |= flagload_dma(&sffh->sfh, tbl);
					break;

				case STATFLAG_EGC:
					ret |= flagload_egc(&sffh->sfh, tbl);
					break;

				case STATFLAG_EPSON:
					ret |= flagload_epson(&sffh->sfh, tbl);
					break;

				case STATFLAG_EVT:
					ret |= flagload_evt(&sffh->sfh, tbl);
					break;

				case STATFLAG_EXT:
					ret |= flagload_ext(&sffh->sfh, tbl);
					break;

				case STATFLAG_FM:
					ret |= flagload_fm(&sffh->sfh, tbl);
					break;

				case STATFLAG_GIJ:
					ret |= flagload_gij(&sffh->sfh, tbl);
					break;

#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
					ret |= hostdrv_sfload(&sffh->sfh, tbl);
					break;
#endif

				case STATFLAG_MEM:
					ret |= flagload_mem(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);

	// I/O作り直し
	i286_memorymap((pccore.model & PCMODEL_EPSON)?1:0);
	iocore_build();
	iocore_bind();
	cbuscore_bind();
	fmboard_bind();

	gdcs.textdisp |= GDCSCRN_EXT;
	gdcs.textdisp |= GDCSCRN_ALLDRAW2;
	gdcs.grphdisp |= GDCSCRN_EXT;
	gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
	gdcs.palchange = GDCSCRN_REDRAW;
	tramflag.renewal = 1;
	cgwindow.writable |= 0x80;
#if defined(CPUSTRUC_FONTPTR)
	FONTPTR_LOW = fontrom + cgwindow.low;
	FONTPTR_HIGH = fontrom + cgwindow.high;
#endif
	i286_vram_dispatch(vramop.operate);
	soundmng_play();

	return(ret);
}

