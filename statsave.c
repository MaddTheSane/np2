#include	"compiler.h"
#include	"dosio.h"
#include	"commng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"timemng.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"board14.h"
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
#include	"statsave.h"
#include	"keydisp.h"


typedef struct {
	char	name[16];
	char	vername[28];
	UINT32	ver;
} NP2FHDR;

typedef struct {
	char	index[10];
	UINT16	ver;
	UINT32	size;
} NP2FLAGPART_T;

enum {
	NP2FLAG_BIN			= 0,
	NP2FLAG_TERM,
	NP2FLAG_CLOCK,
	NP2FLAG_DMA,
	NP2FLAG_EGC,
	NP2FLAG_EXT,
	NP2FLAG_EVT,
	NP2FLAG_GIJ,
	NP2FLAG_FM,
	NP2FLAG_MIDI,
	NP2FLAG_DISK
};

typedef struct {
const char	*index;
	UINT16	ver;
	UINT16	type;
	void	*arg1;
	UINT	arg2;
} STENTRY;

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

static void err_append(ERR_BUF *e, char *buf) {

	if ((e) && (buf)) {
		if (e->buf) {
			int len;
			len = strlen(buf);
			if (e->remain >= len) {
				CopyMemory(e->buf, buf, len);
				e->buf += len;
				e->remain -= len;
			}
		}
	}
}


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

typedef struct {
	FILEH		fh;
	long		pos;
	long		bak;
	long		next;
	NP2FHDR		f;
	NP2FLAGPART_T	p;
} NP2FFILE;

static int flagopen(NP2FFILE *f, const char *filename, ERR_BUF *e) {

	if (f) {
		f->fh = file_open(filename);
		if (f->fh == FILEH_INVALID) {
			return(NP2FLAG_FAILURE);
		}
		if (file_read(f->fh, &f->f, sizeof(NP2FHDR))
												!= sizeof(NP2FHDR)) {
			file_close(f->fh);
			return(NP2FLAG_FAILURE);
		}
		f->pos = sizeof(NP2FHDR);
		if (strcmp(f->f.name, np2flagdef.name)) {
			file_close(f->fh);
			return(NP2FLAG_FAILURE);
		}
		if (strcmp(f->f.vername, np2flagdef.vername)) {
#if 1
			file_close(f->fh);
			return(NP2FLAG_FAILURE);
#else
			return(NP2FLAG_VERSION);			// 他形式を読むのであれば…
#endif
		}
		if (f->f.ver != np2flagdef.ver) {
#if 1
			file_close(f->fh);
			return(NP2FLAG_FAILURE);
#else
			return(NP2FLAG_VERSION);			// 他verを読むのであれば…
#endif
		}
		return(NP2FLAG_SUCCESS);
	}
	(void)e;
	return(NP2FLAG_FAILURE);
}

static int flagcreate(NP2FFILE *f, const char *filename) {

	if (f) {
		f->fh = file_create(filename);
		if (f->fh == FILEH_INVALID) {
			return(NP2FLAG_FAILURE);
		}
		if (file_write(f->fh, &np2flagdef, sizeof(NP2FHDR))
														!= sizeof(NP2FHDR)) {
			file_close(f->fh);
			return(NP2FLAG_FAILURE);
		}
		f->pos = sizeof(NP2FHDR);
		return(NP2FLAG_SUCCESS);
	}
	return(NP2FLAG_FAILURE);
}

static int flagload_create(NP2FFILE *f) {

	if (f) {
		ZeroMemory(&f->p, sizeof(NP2FLAGPART_T));
		if (f->pos & 15) {
			f->pos += 15;
			f->pos &= ~0xf;
			if (file_seek(f->fh, f->pos, 0) != f->pos) {
				return(NP2FLAG_FAILURE);
			}
		}
		if (file_read(f->fh, &f->p, sizeof(NP2FLAGPART_T))
												!= sizeof(NP2FLAGPART_T)) {
			return(NP2FLAG_FAILURE);
		}
		f->pos += sizeof(NP2FLAGPART_T);
		f->next = f->pos + f->p.size;
		return(NP2FLAG_SUCCESS);
	}
	return(NP2FLAG_FAILURE);
}

static int flagload_load(NP2FFILE *f, void *buf, UINT size) {

	if (f && buf && size && (file_read(f->fh, buf, size) == size)) {
		f->pos += size;
		return(NP2FLAG_SUCCESS);
	}
	return(NP2FLAG_FAILURE);
}

static int flagload_close(NP2FFILE *f) {

	if (file_seek(f->fh, f->next, 0) != f->next) {
		return(NP2FLAG_FAILURE);
	}
	f->pos = f->next;
	return(NP2FLAG_SUCCESS);
}

static int flagsave_create(NP2FFILE *f, const STENTRY *t) {

	if (f && t) {
		int		len;
		ZeroMemory(&f->p, sizeof(NP2FLAGPART_T));
		if (f->pos & 15) {
			UINT rem;
			rem = 16 - (f->pos & 15);
			if (file_write(f->fh, &f->p, rem) != rem) {
				return(NP2FLAG_FAILURE);
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
		if (file_write(f->fh, &f->p, sizeof(NP2FLAGPART_T))
												!= sizeof(NP2FLAGPART_T)) {
			return(NP2FLAG_FAILURE);
		}
		f->pos += sizeof(NP2FLAGPART_T);
		return(NP2FLAG_SUCCESS);
	}
	return(NP2FLAG_FAILURE);
}

static int flagsave_save(NP2FFILE *f, void *buf, UINT size) {

	if (f && buf && size && (file_write(f->fh, buf, size) == size)) {
		f->pos += size;
		f->p.size += size;
		return(NP2FLAG_SUCCESS);
	}
	return(NP2FLAG_FAILURE);
}

static int flagsave_close(NP2FFILE *f) {

	if (!f) {
		goto fs_closeerr;
	}
	if (file_seek(f->fh, f->bak, 0) != f->bak) {
		goto fs_closeerr;
	}
	if (file_write(f->fh, &f->p, sizeof(NP2FLAGPART_T))
												!= sizeof(NP2FLAGPART_T)) {
		goto fs_closeerr;
	}
	if (file_seek(f->fh, f->pos, 0) == f->pos) {
		return(NP2FLAG_SUCCESS);
	}

fs_closeerr:
	return(NP2FLAG_FAILURE);
}

static void flagclose(NP2FFILE *f) {

	if (f) {
		file_close(f->fh);
	}
}


// ----

static int flagsave_term(NP2FFILE *f, const STENTRY *t) {

	int		ret;

	ret = flagsave_create(f, t);
	ret |= flagsave_close(f);
	return(ret);
}


// ----

static int flagsave_common(NP2FFILE *f, const STENTRY *t) {

	int		ret;

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, t->arg1, t->arg2);
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int flagload_common(NP2FFILE *f, const STENTRY *t) {

	return(flagload_load(f, t->arg1, t->arg2));
}


// -----

static int flagload_clock(NP2FFILE *f, const STENTRY *t) {

	int		ret;

	ret = flagload_common(f, t);
	sound_changeclock();
	beep_changeclock();
	return(ret);
}


// -----

static int flagsave_dma(NP2FFILE *f, const STENTRY *t) {

	int			ret;
	int			i;
	_DMAC		dmabak;

	dmabak = dmac;
	for (i=0; i<4; i++) {
		if ((PROC2NUM(dmabak.dmach[i].outproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].inproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].extproc, dmaproc))) {
			return(NP2FLAG_FAILURE);
		}
	}
	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, &dmabak, sizeof(dmabak));
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int flagload_dma(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	int		i;

	ret = flagload_load(f, &dmac, sizeof(dmac));

	for (i=0; i<4; i++) {
		if (NUM2PROC(dmac.dmach[i].outproc, dmaproc)) {
			dmac.dmach[i].outproc = dma_dummyout;
			ret |= NP2FLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].inproc, dmaproc)) {
			dmac.dmach[i].inproc = dma_dummyin;
			ret |= NP2FLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].extproc, dmaproc)) {
			dmac.dmach[i].extproc = dma_dummyproc;
			ret |= NP2FLAG_WARNING;
		}
	}
	(void)t;
	return(ret);
}


// -----

static int flagsave_egc(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	_EGC	egcbak;

	egcbak = egc;
	egcbak.inptr -= (long)egc.buf;
	egcbak.outptr -= (long)egc.buf;

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, &egcbak, sizeof(egcbak));
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int flagload_egc(NP2FFILE *f, const STENTRY *t) {

	int		ret;

	ret = flagload_load(f, &egc, sizeof(egc));
	egc.inptr += (long)egc.buf;
	egc.outptr += (long)egc.buf;
	(void)t;
	return(ret);
}


// -----

static int flagsave_ext(NP2FFILE *f, const STENTRY *t) {

	int		ret;

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, &extmem, sizeof(extmem));
		if (extmemmng_size) {
			ret |= flagsave_save(f, extmemmng_ptr, extmemmng_size);
		}
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int flagload_ext(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	int		i;
	UINT	pagemax = 0;

	ret = flagload_load(f, &extmem, sizeof(extmem));
	if (extmem.maxmem) {
		if (extmem.maxmem > (13+1)) {
			extmem.maxmem = (13+1);
		}
		if (!extmemmng_realloc(extmem.maxmem - 1)) {
			pagemax = (extmem.maxmem - 1) << 8;
			if (extmemmng_size) {
				ret |= flagload_load(f, extmemmng_ptr, extmemmng_size);
			}
		}
		else {
			extmem.maxmem = 0;
		}
	}
	for (i=0; i<4; i++) {
		if (extmem.page[i] < pagemax) {
			extmem.pageptr[i] = extmemmng_ptr + (extmem.page[i] << 12);
		}
		else {
			extmem.pageptr[i] = mem + 0xc0000 + (i << 14);
		}
	}
	(void)t;
	return(ret);
}


// -----

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

static int nevent_save(NP2FFILE *f, int num) {

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
	return(flagsave_save(f, &nit, sizeof(nit)));
}

static int flagsave_evt(NP2FFILE *f, const STENTRY *t) {

	NEVTSAVE	nevt;
	int			ret;
	UINT		i;

	nevt.readyevents = nevent.readyevents;
	nevt.waitevents = nevent.waitevents;

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, &nevt, sizeof(nevt));
		for (i=0; i<nevt.readyevents; i++) {
			ret |= nevent_save(f, nevent.level[i]);
		}
		for (i=0; i<nevt.waitevents; i++) {
			ret |= nevent_save(f, nevent.waitevent[i]);
		}
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int nevent_load(NP2FFILE *f, UINT *tbl, UINT *pos) {

	int			ret;
	NEVTITEM	nit;
	UINT		i;
	UINT		num;

	ret = flagload_load(f, &nit, sizeof(nit));

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
			ret |= NP2FLAG_WARNING;
		}
		else {
			tbl[*pos] = num;
			(*pos)++;
		}
	}
	else {
		ret |= NP2FLAG_WARNING;
	}
	return(ret);
}

static int flagload_evt(NP2FFILE *f, const STENTRY *t) {

	int			ret;
	NEVTSAVE	nevt;
	UINT		i;

	ret = flagload_load(f, &nevt, sizeof(nevt));

	nevent.readyevents = 0;
	nevent.waitevents = 0;

	for (i=0; i<nevt.readyevents; i++) {
		ret |= nevent_load(f, nevent.level, &nevent.readyevents);
	}
	for (i=0; i<nevt.waitevents; i++) {
		ret |= nevent_load(f, nevent.waitevent, &nevent.waitevents);
	}
	(void)t;
	return(ret);
}


// ----

static int flagsave_gij(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	int		i;
	int		j;
	BYTE	*fnt;

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		for (i=0; i<2; i++) {
			fnt = font + ((0x56 + (i << 7)) << 4);
			for (j=0; j<0x80; j++) {
				ret |= flagsave_save(f, fnt, 32);
				fnt += 0x1000;
			}
		}
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int flagload_gij(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	int		i;
	int		j;
	BYTE	*fnt;

	ret = 0;
	for (i=0; i<2; i++) {
		fnt = font + ((0x56 + (i << 7)) << 4);
		for (j=0; j<0x80; j++) {
			ret |= flagload_load(f, fnt, 32);
			fnt += 0x1000;
		}
	}
	(void)t;
	return(ret);
}


// -----

enum {
	FLAG_MG			= 0x0001,
	FLAG_FM1A		= 0x0002,
	FLAG_FM1B		= 0x0004,
	FLAG_FM2A		= 0x0008,
	FLAG_FM2B		= 0x0010,
	FLAG_PSG1		= 0x0020,
	FLAG_PSG2		= 0x0040,
	FLAG_RHYTHM		= 0x0080,
	FLAG_ADPCM		= 0x0100,
	FLAG_PCM86		= 0x0200,
	FLAG_CS4231		= 0x0400
};

typedef struct {
	BYTE	keyreg[OPNCH_MAX];
	BYTE	extop[4];
} OPNKEY;

static int flagsave_fm(NP2FFILE *f, const STENTRY *t) {

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

		default:
			saveflg = 0;
			break;
	}

	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		ret |= flagsave_save(f, &usesound, sizeof(usesound));

		if (saveflg & FLAG_MG) {
			ret |= flagsave_save(f, &musicgen, sizeof(musicgen));
		}
		if (saveflg & FLAG_FM1A) {
			ret |= flagsave_save(f, &fmtimer, sizeof(fmtimer));
			ret |= flagsave_save(f, &opn, sizeof(opn));
			CopyMemory(opnkey.keyreg, opngen.keyreg, sizeof(opngen.keyreg));
			opnkey.extop[0] = opnch[2].extop;
			opnkey.extop[1] = opnch[5].extop;
			opnkey.extop[2] = opnch[8].extop;
			opnkey.extop[3] = opnch[11].extop;
			ret |= flagsave_save(f, &opnkey, sizeof(opnkey));
		}
		if (saveflg & FLAG_PSG1) {
			ret |= flagsave_save(f, &psg1.reg, sizeof(PSGREG));
		}
		if (saveflg & FLAG_PSG2) {
			ret |= flagsave_save(f, &psg2.reg, sizeof(PSGREG));
		}
		if (saveflg & FLAG_ADPCM) {
			ret |= flagsave_save(f, &adpcm, sizeof(adpcm));
		}
		if (saveflg & FLAG_PCM86) {
			ret |= flagsave_save(f, &pcm86, sizeof(pcm86));
		}
		if (saveflg & FLAG_CS4231) {
			ret |= flagsave_save(f, &cs4231, sizeof(cs4231));
		}
		ret |= flagsave_close(f);
	}
	return(ret);
}

static void play_fmreg(BYTE num) {

	UINT	chbase;
	UINT	reg;
	UINT	i;

	chbase = num * 3;
	reg = num * 0x100;
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

static int flagload_fm(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	UINT	saveflg;
	OPNKEY	opnkey;

	opngen_reset();
	psggen_reset(&psg1);
	psggen_reset(&psg2);
	psggen_reset(&psg3);
	rhythm_reset(&rhythm);
	adpcm_reset(&adpcm);
	pcm86_reset();
	cs4231_reset();

	ret = flagload_load(f, &usesound, sizeof(usesound));
	fmboard_reset((BYTE)usesound);

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

		default:
			saveflg = 0;
			break;
	}

	if ((saveflg & FLAG_MG) && (f->p.ver != 0)) {
		ret |= flagload_load(f, &musicgen, sizeof(musicgen));
		board14_allkeymake();
	}

	if (saveflg & FLAG_FM1A) {
		ret |= flagload_load(f, &fmtimer, sizeof(fmtimer));
		ret |= flagload_load(f, &opn, sizeof(opn));
		ret |= flagload_load(f, &opnkey, sizeof(opnkey));
		CopyMemory(opngen.keyreg, &opnkey.keyreg, sizeof(opngen.keyreg));
		opnch[2].extop = opnkey.extop[0];
		opnch[5].extop = opnkey.extop[1];
		opnch[8].extop = opnkey.extop[2];
		opnch[11].extop = opnkey.extop[3];
	}
	if (saveflg & FLAG_PSG1) {
		ret |= flagload_load(f, &psg1.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_PSG2) {
		ret |= flagload_load(f, &psg2.reg, sizeof(PSGREG));
	}
	if (saveflg & FLAG_ADPCM) {
		ret |= flagload_load(f, &adpcm, sizeof(adpcm));
	}
	if (saveflg & FLAG_PCM86) {
		ret |= flagload_load(f, &pcm86, sizeof(pcm86));
	}
	if (saveflg & FLAG_CS4231) {
		ret |= flagload_load(f, &cs4231, sizeof(cs4231));
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
		play_fmreg(0);
	}
	if (saveflg & FLAG_FM1B) {
		play_fmreg(1);
	}
	if (saveflg & FLAG_FM2A) {
		play_fmreg(2);
	}
	if (saveflg & FLAG_FM2B) {
		play_fmreg(3);
	}
	if (saveflg & FLAG_PSG1) {
		play_psgreg(&psg1);
	}
	if (saveflg & FLAG_PSG2) {
		play_psgreg(&psg2);
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

static int disksave(NP2FFILE *f, const char *path, int readonly) {

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
	return(flagsave_save(f, &st, sizeof(st)));
}

static int flagsave_disk(NP2FFILE *f, const STENTRY *t) {

	int		ret;
	BYTE	i;

	sxsi_flash();
	ret = flagsave_create(f, t);
	if (ret != NP2FLAG_FAILURE) {
		for (i=0; i<4; i++) {
			ret |= disksave(f, fdd_diskname(i), fdd_diskprotect(i));
		}
		for (i=0x00; i<0x02; i++) {
			ret |= disksave(f, sxsi_getname(i), 0);
		}
		for (i=0x20; i<0x22; i++) {
			ret |= disksave(f, sxsi_getname(i), 0);
		}
		ret |= flagsave_close(f);
	}
	return(ret);
}

static int diskcheck(NP2FFILE *f, const char *name, ERR_BUF *e) {

	int			ret;
	FILEH		fh;
	STATDISK	st;
	char		buf[256];
	DOSDATE		date;
	DOSTIME		time;

	ret = flagload_load(f, &st, sizeof(st));
	if (st.path[0]) {
		fh = file_open_rb(st.path);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &date, &time);
			file_close(fh);
			if ((memcmp(&st.date, &date, sizeof(date))) ||
				(memcmp(&st.time, &time, sizeof(time)))) {
				ret |= NP2FLAG_DISKCHG;
				SPRINTF(buf, "%s: updated\n", name);
				err_append(e, buf);
			}
		}
		else {
			ret |= NP2FLAG_DISKCHG;
			SPRINTF(buf, "%s: not found\n", name);
			err_append(e, buf);
		}
	}
	return(ret);
}

static int flagcheck_disk(NP2FFILE *f, const STENTRY *t, ERR_BUF *e) {

	int		ret;
	int		i;
	char	buf[8];

	ret = 0;
	for (i=0; i<4; i++) {
		SPRINTF(buf, "FDD%d", i+1);
		ret |= diskcheck(f, buf, e);
	}
	sxsi_flash();
	for (i=0; i<2; i++) {
		SPRINTF(buf, "SASI%u", i+1);
		ret |= diskcheck(f, buf, e);
	}
	for (i=0; i<2; i++) {
		SPRINTF(buf, "SCSI%d", i+1);
		ret |= diskcheck(f, buf, e);
	}
	(void)t;
	return(ret);
}

static int flagload_disk(NP2FFILE *f, const STENTRY *t) {

	int			ret;
	BYTE		i;
	STATDISK	st;

	ret = 0;
	for (i=0; i<4; i++) {
		ret |= flagload_load(f, &st, sizeof(st));
		if (st.path[0]) {
			fdd_set(i, st.path, FTYPE_NONE, st.readonly);
		}
	}
	sxsi_trash();
	for (i=0x00; i<0x02; i++) {
		ret |= flagload_load(f, &st, sizeof(st));
		if (st.path[0]) {
			sxsi_hddopen(i, st.path);
		}
	}
	for (i=0x20; i<0x22; i++) {
		ret |= flagload_load(f, &st, sizeof(st));
#if 0
		if (file[0]) {
			bios0x1b_scsiopen(i, file);
		}
#endif
	}
	(void)t;
	return(ret);
}


// -----

#ifdef _MIDICH
static int flagsave_midi(NP2FFILE *f, const STENTRY *t) {

	UINT	device;
	COMMNG	cm;
	int		ret;
	_MIDICH	mch[16];

	device = (UINT)t->arg1;
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
	ret = NP2FLAG_SUCCESS;
	if ((cm != NULL) && (cm->msg(cm, COMMSG_MIDISTATGET, (long)mch))) {
		ret = flagsave_create(f, t);
		if (ret != NP2FLAG_FAILURE) {
			ret |= flagsave_save(f, mch, sizeof(mch));
			ret |= flagsave_close(f);
		}
	}
	return(ret);
}

static int flagload_midi(NP2FFILE *f, const STENTRY *t) {

	_MIDICH	mch[16];
	UINT	device;
	COMMNG	cm;
	int		ret;

	ret = flagload_load(f, mch, sizeof(mch));
	if (ret != NP2FLAG_FAILURE) {
		device = (UINT)t->arg1;
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
			cm->msg(cm, COMMSG_MIDISTATSET, (long)mch);
		}
	}
	return(ret);
}
#endif


// ----

static int flagcheck_versize(NP2FFILE *f, const STENTRY *t, ERR_BUF *e) {

	if ((f) && (t)) {
		if ((f->p.ver == t->ver) && (f->p.size == t->arg2)) {
			return(NP2FLAG_SUCCESS);
		}
		return(NP2FLAG_VERSION);
	}
	(void)e;
	return(NP2FLAG_FAILURE);
}

static int flagcheck_veronly(NP2FFILE *f, const STENTRY *t, ERR_BUF *e) {

	if ((f) && (t)) {
		if (f->p.ver == t->ver) {
			return(NP2FLAG_SUCCESS);
		}
		return(NP2FLAG_VERSION);
	}
	(void)e;
	return(NP2FLAG_FAILURE);
}


// ----

int statsave_save(const char *filename) {

	NP2FFILE	f;
	int			ret;
	UINT		i;

	ret = flagcreate(&f, filename);
	if (ret == NP2FLAG_FAILURE) {
		return(ret);
	}
	for (i=0; i<sizeof(np2tbl)/sizeof(STENTRY); i++) {
		switch(np2tbl[i].type) {
			case NP2FLAG_BIN:
			case NP2FLAG_CLOCK:
				ret |= flagsave_common(&f, &np2tbl[i]);
				break;

			case NP2FLAG_TERM:
				ret |= flagsave_term(&f, &np2tbl[i]);
				break;

			case NP2FLAG_DMA:
				ret |= flagsave_dma(&f, &np2tbl[i]);
				break;

			case NP2FLAG_EGC:
				ret |= flagsave_egc(&f, &np2tbl[i]);
				break;

			case NP2FLAG_EXT:
				ret |= flagsave_ext(&f, &np2tbl[i]);
				break;

			case NP2FLAG_EVT:
				ret |= flagsave_evt(&f, &np2tbl[i]);
				break;

			case NP2FLAG_GIJ:
				ret |= flagsave_gij(&f, &np2tbl[i]);
				break;

			case NP2FLAG_FM:
				ret |= flagsave_fm(&f, &np2tbl[i]);
				break;

			case NP2FLAG_DISK:
				ret |= flagsave_disk(&f, &np2tbl[i]);
				break;

#if defined(MIDICH)
			case NP2FLAG_MIDI:
				ret |= flagsave_midi(&f, &np2tbl[i]);
				break;
#endif
		}
	}
	flagclose(&f);
	return(ret);
}

int statsave_check(const char *filename, char *buf, int size) {

	NP2FFILE	f;
	int			ret;
	UINT		i;
	BOOL		done;
	ERR_BUF		e;

	e.buf = buf;
	e.remain = size;
	if (e.remain) {
		e.remain--;
		if (buf) {
			*buf = '\0';
		}
	}
	else {
		e.buf = NULL;
	}

	ret = flagopen(&f, filename, &e);
	if (ret != NP2FLAG_FAILURE) {
		done = FALSE;
		while((!done) && (ret != NP2FLAG_FAILURE)) {
			char index[11];
			ret |= flagload_create(&f);
			CopyMemory(index, f.p.index, sizeof(f.p.index));
			index[10] = 0;
			for (i=0; i<sizeof(np2tbl)/sizeof(STENTRY); i++) {
				if (!strcmp(index, np2tbl[i].index)) {
					break;
				}
			}
			if (i < (sizeof(np2tbl)/sizeof(STENTRY))) {
				switch(np2tbl[i].type) {
					case NP2FLAG_BIN:
					case NP2FLAG_CLOCK:
						ret |= flagcheck_versize(&f, &np2tbl[i], &e);
						break;

					case NP2FLAG_TERM:
						done = TRUE;
						break;

					case NP2FLAG_DMA:
					case NP2FLAG_EGC:
					case NP2FLAG_EXT:
					case NP2FLAG_EVT:
					case NP2FLAG_GIJ:
#if defined(MIDICH)
					case NP2FLAG_MIDI:
#endif
						ret |= flagcheck_veronly(&f, &np2tbl[i], &e);
						break;

					case NP2FLAG_DISK:
						ret |= flagcheck_disk(&f, &np2tbl[i], &e);
						break;

					case NP2FLAG_FM:							// ver0.31
						break;

					default:
						ret |= NP2FLAG_WARNING;
						break;
				}
			}
			else {
				ret |= NP2FLAG_WARNING;
			}
			ret |= flagload_close(&f);
		}
		flagclose(&f);
	}
	if (e.buf) {
		*e.buf = '\0';
	}
	return(ret);
}

int statsave_load(const char *filename) {

	NP2FFILE	f;
	int			ret;
	UINT		i;
	BOOL		done;

	ret = flagopen(&f, filename, NULL);
	if (ret == NP2FLAG_FAILURE) {
		return(ret);
	}

	soundmng_stop();
	rs232c_midipanic();
	mpu98ii_midipanic();
	pc9861k_midipanic();
	sound_reset();
	fmboard_reset(0);

	done = FALSE;
	while((!done) && (ret != NP2FLAG_FAILURE)) {
		char index[11];
		ret |= flagload_create(&f);
		CopyMemory(index, f.p.index, sizeof(f.p.index));
		index[10] = 0;
		for (i=0; i<sizeof(np2tbl)/sizeof(STENTRY); i++) {
			if (!strcmp(index, np2tbl[i].index)) {
				break;
			}
		}
		if (i < (sizeof(np2tbl)/sizeof(STENTRY))) {
			switch(np2tbl[i].type) {
				case NP2FLAG_BIN:
					ret |= flagload_common(&f, &np2tbl[i]);
					break;

				case NP2FLAG_TERM:
					done = TRUE;
					break;

				case NP2FLAG_CLOCK:
					ret |= flagload_clock(&f, &np2tbl[i]);
					break;

				case NP2FLAG_DMA:
					ret |= flagload_dma(&f, &np2tbl[i]);
					break;

				case NP2FLAG_EGC:
					ret |= flagload_egc(&f, &np2tbl[i]);
					break;

				case NP2FLAG_EXT:
					ret |= flagload_ext(&f, &np2tbl[i]);
					break;

				case NP2FLAG_EVT:
					ret |= flagload_evt(&f, &np2tbl[i]);
					break;

				case NP2FLAG_GIJ:
					ret |= flagload_gij(&f, &np2tbl[i]);
					break;

				case NP2FLAG_FM:
					ret |= flagload_fm(&f, &np2tbl[i]);
					break;

				case NP2FLAG_DISK:
					ret |= flagload_disk(&f, &np2tbl[i]);
					break;

#if defined(MIDICH)
				case NP2FLAG_MIDI:
					ret |= flagload_midi(&f, &np2tbl[i]);
					break;
#endif

				default:
					ret |= NP2FLAG_WARNING;
					break;
			}
		}
		else {
			ret |= NP2FLAG_WARNING;
		}
		ret |= flagload_close(&f);
	}
	flagclose(&f);

	// I/O作り直し
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
	i286_vram_dispatch(vramop.operate);
	soundmng_play();

	return(ret);
}

