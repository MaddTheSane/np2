#include	"compiler.h"
#include	"strres.h"
#include	"textfile.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"


static const UINT8 cd001[7] = {0x01,'C','D','0','0','1',0x01};

typedef struct {
	UINT	type;
	UINT32	pos;
} CDTRK;

typedef struct {
	FILEH	fh;
	UINT	type;
	UINT32	lastpos;
	CDTRK	trk[99];
	OEMCHAR	path[MAX_PATH];
} _CDINFO, *CDINFO;


// ---- セクタ2048

static long issec2048(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = 16 * 2048;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2048_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2048_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2048_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2048_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2048) != 0) {
		goto sec2048_err;
	}
	return(fsize / 2048);

sec2048_err:
	return(-1);
}

static REG8 sec2048_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	FILEH	fh;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * 2048;
	fh = ((CDINFO)sxsi->hdl)->fh;
	if (file_seek(fh, pos, FSEEK_SET) != pos) {
		return(0xd0);
	}
	while(size) {
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}


// ---- セクタ2352

static long issec2352(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = (16 * 2352) + 16;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2352_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2352_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2352_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2352_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2352) != 0) {
		goto sec2352_err;
	}
	return(fsize / 2352);

sec2352_err:
	return(-1);
}

static REG8 sec2352_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	FILEH	fh;
	long	fpos;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	fh = ((CDINFO)sxsi->hdl)->fh;
	while(size) {
		fpos = (pos * 2352) + 16;
		if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}


// ----

static BRESULT cd_reopen(SXSIDEV sxsi) {

	CDINFO	cdinfo;
	FILEH	fh;

	cdinfo = (CDINFO)sxsi->hdl;
	fh = file_open_rb(cdinfo->path);
	if (fh != FILEH_INVALID) {
		cdinfo->fh = fh;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

static void cd_close(SXSIDEV sxsi) {

	CDINFO	cdinfo;

	cdinfo = (CDINFO)sxsi->hdl;
	file_close(cdinfo->fh);
}

static void cd_destroy(SXSIDEV sxsi) {

	_MFREE((CDINFO)sxsi->hdl);
}


// ----

static const OEMCHAR str_cue[] = OEMTEXT("cue");
static const OEMCHAR str_file[] = OEMTEXT("FILE");
static const OEMCHAR str_track[] = OEMTEXT("TRACK");
static const OEMCHAR str_mode1[] = OEMTEXT("MODE1/2352");
static const OEMCHAR str_index[] = OEMTEXT("INDEX");
static const OEMCHAR str_audio[] = OEMTEXT("AUDIO");


static BRESULT openimg(SXSIDEV sxsi, const OEMCHAR *path,
												const CDTRK *trk, UINT trks) {

	FILEH	fh;
	UINT	type;
	long	totals;
	CDINFO	cdinfo;

	fh = file_open_rb(path);
	if (fh == FILEH_INVALID) {
		goto sxsiope_err1;
	}
	type = 2048;
	totals = issec2048(fh);
	if (totals < 0) {
		type = 2352;
		totals = issec2352(fh);
	}
	if (totals < 0) {
		goto sxsiope_err2;
	}
	cdinfo = (CDINFO)_MALLOC(sizeof(_CDINFO), path);
	if (cdinfo == NULL) {
		goto sxsiope_err2;
	}
	ZeroMemory(cdinfo, sizeof(_CDINFO));
	cdinfo->fh = fh;
	cdinfo->type = type;
	cdinfo->lastpos = totals;
	if ((trk != NULL) && (trks != 0)) {
		trks = min(trks, NELEMENTS(cdinfo->trk));
		CopyMemory(cdinfo->trk, trk, trks * sizeof(CDTRK));
	}
	else {
		cdinfo->trk[0].type = 0x14;
//		cdinfo->trk[0].pos = 0;
	}
	file_cpyname(cdinfo->path, path, NELEMENTS(cdinfo->path));

	sxsi->reopen = cd_reopen;
	if (type == 2048) {
		sxsi->read = sec2048_read;
	}
	else {
		sxsi->read = sec2352_read;
	}
	sxsi->close = cd_close;
	sxsi->destroy = cd_destroy;
	sxsi->hdl = (INTPTR)cdinfo;
	sxsi->totals = totals;
	sxsi->cylinders = 0;
	sxsi->size = 2048;
	sxsi->sectors = 1;
	sxsi->surfaces = 1;
	sxsi->headersize = 0;
	sxsi->mediatype = SXSIMEDIA_DATA;
	return(SUCCESS);

sxsiope_err2:
	file_close(fh);

sxsiope_err1:
	return(FAILURE);
}

static BRESULT getint2(const OEMCHAR *str, UINT *val) {

	if ((str[0] < '0') || (str[0] > '9') ||
		(str[1] < '0') || (str[1] > '9')) {
		return(FAILURE);
	}
	if (val) {
		*val = ((str[0] - '0') * 10) + (str[1] - '0');
	}
	return(SUCCESS);
}

static BRESULT getpos(const OEMCHAR *str, UINT32 *pos) {

	UINT	m;
	UINT	s;
	UINT	f;

	if ((getint2(str + 0, &m) != SUCCESS) || (str[2] != ':') ||
		(getint2(str + 3, &s) != SUCCESS) || (str[5] != ':') ||
		(getint2(str + 6, &f) != SUCCESS)) {
		return(FAILURE);
	}
	if (pos) {
		*pos = (((m * 60) + s) * 75) + f;
	}
	return(SUCCESS);
}

static BRESULT opencue(SXSIDEV sxsi, const OEMCHAR *fname) {

	CDTRK		trk[99];
	OEMCHAR		path[MAX_PATH];
	UINT		curtrk;
	TEXTFILEH	tfh;
	OEMCHAR		buf[512];
	OEMCHAR		*argv[8];
	int			argc;
	UINT		type;

	ZeroMemory(trk, sizeof(trk));
	path[0] = '\0';
	curtrk = -1;
	tfh = textfile_open(fname, 0x800);
	if (tfh == NULL) {
		return(FAILURE);
	}
	while(textfile_read(tfh, buf, NELEMENTS(buf)) == SUCCESS) {
		argc = milstr_getarg(buf, argv, NELEMENTS(argv));
		if ((argc >= 3) && (!milstr_cmp(argv[0], str_file))) {
			file_cpyname(path, argv[1], NELEMENTS(path));
		}
		else if ((argc >= 3) && (!milstr_cmp(argv[0], str_track))) {
			curtrk = milstr_solveINT(argv[1]) - 1;
			type = 0;
			if (!milstr_cmp(argv[2], str_mode1)) {
				type = 0x14;
			}
			else if (!milstr_cmp(argv[2], str_audio)) {
				type = 0x10;
			}
			if ((curtrk < NELEMENTS(trk)) && (type != 0)) {
				trk[curtrk].type = type;
			}
		}
		else if ((argc >= 3) && (!milstr_cmp(argv[0], str_index))) {
			if (curtrk < NELEMENTS(trk)) {
				getpos(argv[2], &trk[curtrk].pos);
			}
		}
	}
	textfile_close(tfh);
#if 0
{
	UINT i;
	for (i=0; i<NELEMENTS(trk); i++) {
		if (trk[i].type) {
			TRACEOUT(("%.2d: %.4x %d", i+1, trk[i].type, trk[i].pos));
		}
	}
}
#endif
	return(openimg(sxsi, path, trk, NELEMENTS(trk)));
}

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *fname) {

const OEMCHAR	*ext;

	ext = file_getext(fname);
	if (!file_cmpname(ext, str_cue)) {
		return(opencue(sxsi, fname));
	}
	return(openimg(sxsi, fname, NULL, 0));
}

