#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"textfile.h"
#include	"midiout.h"


#define	CFG_MAXAMP		400
#define	MAX_NAME		64

enum {
	CFG_DIR		 = 0,
	CFG_SOURCE,
	CFG_DEFAULT,
	CFG_BANK,
	CFG_DRUM
};

static const char str_dir[] = "dir";
static const char str_source[] = "source";
static const char str_default[] = "default";
static const char str_bank[] = "bank";
static const char str_drumset[] = "drumset";
static const char *cfgstr[] = {str_dir, str_source, str_default,
								str_bank, str_drumset};

static const char str_amp[] = "amp";
static const char str_keep[] = "keep";
static const char str_note[] = "note";
static const char str_pan[] = "pan";
static const char str_strip[] = "strip";
static const char str_left[] = "left";
static const char str_center[] = "center";
static const char str_right[] = "right";
static const char str_env[] = "env";
static const char str_loop[] = "loop";
static const char str_tail[] = "tail";
static const char file_timiditycfg[] = "timidity.cfg";


static void pathadd(MIDIMOD mod, const char *path) {

	_PATHLIST	pl;
	PATHLIST	p;

	ZeroMemory(&pl, sizeof(pl));
	if (path) {
		pl.path[0] = '\0';
		file_catname(pl.path, path, sizeof(pl.path));	// separator change!
		if (path[0]) {
			file_setseparator(pl.path, sizeof(pl.path));
		}
	}

	pl.next = mod->pathlist;
	p = pl.next;
	while(p) {
		if (!strcmp(p->path, pl.path)) {
			return;
		}
		p = p->next;
	}
	p = (PATHLIST)listarray_append(mod->pathtbl, &pl);
	if (p) {
		mod->pathlist = p;
	}
}

static int cfggetarg(char *str, char *arg[], int maxarg) {

	int		ret;
	BOOL	quot;
	char	*p;
	BYTE	c;

	ret = 0;
	while(maxarg--) {
		quot = FALSE;
		while(1) {
			c = *str;
			if ((c == 0) || (c == 0x23)) {
				goto cga_done;
			}
			if (c > 0x20) {
				break;
			}
			str++;
		}
		arg[ret++] = str;
		p = str;
		while(1) {
			c = *str;
			if (c == 0) {
				break;
			}
			str++;
			if (c == 0x22) {
				quot = !quot;
			}
			else if (quot) {
				*p++ = c;
			}
			else if (c == 0x23) {
				*p = '\0';
				goto cga_done;
			}
			else if (c > 0x20) {
				*p++ = c;
			}
			else {
				break;
			}
		}
		*p = '\0';
	}

cga_done:
	return(ret);
}

static char *seachr(char *str, char sepa) {

	char	c;

	while(1) {
		c = *str;
		if (c == '\0') {
			break;
		}
		if (c == sepa) {
			return(str);
		}
		str++;
	}
	return(NULL);
}

enum {
	VAL_EXIST	= 1,
	VAL_SIGN	= 2
};

static BOOL cfggetval(const char *str, int *val) {

	int		ret;
	int		flag;
	int		c;

	ret = 0;
	flag = 0;
	c = *str;
	if (c == '+') {
		str++;
	}
	else if (c == '-') {
		str++;
		flag |= VAL_SIGN;
	}
	while(1) {
		c = *str++;
		c -= '0';
		if ((unsigned)c < 10) {
			ret *= 10;
			ret += c;
			flag |= VAL_EXIST;
		}
		else {
			break;
		}
	}
	if (flag & VAL_EXIST) {
		if (flag & VAL_SIGN) {
			ret *= -1;
		}
		if (val) {
			*val = ret;
		}
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}


// ----

static void settone(MIDIMOD mod, int bank, int argc, char *argv[]) {

	int		val;
	TONECFG	tone;
	char	*name;
	int		i;
	char	*key;
	char	*data;
	BYTE	flag;

	if ((bank < 0) || (bank >= (MIDI_BANKS * 2)) || (argc < 2) ||
		(cfggetval(argv[0], &val) != SUCCESS) || (val < 0) || (val >= 128)) {
		return;
	}
	tone = mod->tonecfg[bank];
	if (tone == NULL) {
		tone = (TONECFG)_MALLOC(sizeof(_TONECFG) * 128, "tone cfg");
		if (tone == NULL) {
			return;
		}
		mod->tonecfg[bank] = tone;
		ZeroMemory(tone, sizeof(_TONECFG) * 128);
	}
	tone += val;
	name = tone->name;
	if (name == NULL) {
		name = (char *)listarray_append(mod->namelist, NULL);
		tone->name = name;
	}
	if (name) {
		name[0] = '\0';
		file_catname(name, argv[1], MAX_NAME);		// separator change!
	}
	flag = TONECFG_EXIST;
	tone->amp = TONECFG_AUTOAMP;
	tone->pan = TONECFG_VARIABLE;

	if (!(bank & 1)) {					// for tone
		tone->note = TONECFG_VARIABLE;
	}
	else {								// for drums
		flag |= TONECFG_NOLOOP | TONECFG_NOENV;
		tone->note = (BYTE)val;
	}

	for (i=2; i<argc; i++) {
		key = argv[i];
		data = seachr(key, '=');
		if (data == NULL) {
			continue;
		}
		*data++ = '\0';
		if (!milstr_cmp(key, str_amp)) {
			if (cfggetval(data, &val) == SUCCESS) {
				if (val < 0) {
					val = 0;
				}
				else if (val > CFG_MAXAMP) {
					val = CFG_MAXAMP;
				}
				tone->amp = val;
			}
		}
		else if (!milstr_cmp(key, str_keep)) {
			if (!milstr_cmp(data, str_env)) {
				flag &= ~TONECFG_NOENV;
				flag |= TONECFG_KEEPENV;
			}
			else if (!milstr_cmp(data, str_loop)) {
				flag &= ~TONECFG_NOLOOP;
			}
		}
		else if (!milstr_cmp(key, str_note)) {
			if ((cfggetval(data, &val) == SUCCESS) &&
				(val >= 0) && (val < 128)) {
				tone->note = (BYTE)val;
			}
		}
		else if (!milstr_cmp(key, str_pan)) {
			if (!milstr_cmp(data, str_left)) {
				val = 0;
			}
			else if (!milstr_cmp(data, str_center)) {
				val = 64;
			}
			else if (!milstr_cmp(data, str_right)) {
				val = 127;
			}
			else if (cfggetval(data, &val) == SUCCESS) {
				if (val < -100) {
					val = -100;
				}
				else if (val > 100) {
					val = 100;
				}
				val = val + 100;
				val *= 127;
				val += 100;
				val /= 200;
			}
			else {
				continue;
			}
			tone->pan = (BYTE)val;
		}
		else if (!milstr_cmp(key, str_strip)) {
			if (!milstr_cmp(data, str_env)) {
				flag &= ~TONECFG_KEEPENV;
				flag |= TONECFG_NOENV;
			}
			else if (!milstr_cmp(data, str_loop)) {
				flag |= TONECFG_NOLOOP;
			}
			else if (!milstr_cmp(data, str_tail)) {
				flag |= TONECFG_NOTAIL;
			}
		}
	}
	tone->flag = flag;
}


// ----

BOOL cfgfile_getfile(MIDIMOD mod, const char *filename,
													char *path, int size) {

	PATHLIST	p;
	short		attr;

	if ((filename == NULL) || (filename[0] == '\0') ||
		(path == NULL) || (size == 0)) {
		goto fpgf_exit;
	}
	p = mod->pathlist;
	while(p) {
		file_cpyname(path, p->path, size);
		file_catname(path, filename, size);
		attr = file_attr(path);
		if (attr != -1) {
			return(SUCCESS);
		}
		p = p->next;
	}

fpgf_exit:
	return(FAILURE);
}

BOOL cfgfile_load(MIDIMOD mod, const char *filename, int depth) {

	TEXTFILEH	tfh;
	char		buf[1024];
	int			bank;
	int			i;
	int			argc;
	char		*argv[16];
	int			val;
	UINT		cfg;

	bank = -1;

	if ((depth >= 16) ||
		(cfgfile_getfile(mod, filename, buf, sizeof(buf)) != SUCCESS)) {
		goto cfl_err;
	}
//	TRACEOUT(("open: %s", buf));
	tfh = textfile_open(buf, 0x1000);
	if (tfh == NULL) {
		goto cfl_err;
	}
	while(textfile_read(tfh, buf, sizeof(buf)) == SUCCESS) {
		argc = cfggetarg(buf, argv, sizeof(argv)/sizeof(char *));
		if (argc < 2) {
			continue;
		}
		cfg = 0;
		while(cfg < (sizeof(cfgstr)/sizeof(char *))) {
			if (!milstr_cmp(argv[0], cfgstr[cfg])) {
				break;
			}
			cfg++;
		}
		switch(cfg) {
			case CFG_DIR:
				for (i=1; i<argc; i++) {
					pathadd(mod, argv[i]);
				}
				break;

			case CFG_SOURCE:
				for (i=1; i<argc; i++) {
					depth++;
					cfgfile_load(mod, argv[i], depth);
					depth--;
				}
				break;

			case CFG_DEFAULT:
				break;

			case CFG_BANK:
			case CFG_DRUM:
				if ((cfggetval(argv[1], &val) == SUCCESS) &&
					(val >= 0) && (val < 128)) {
					val <<= 1;
					if (cfg == CFG_DRUM) {
						val++;
					}
					bank = val;
				}
				break;

			default:
				settone(mod, bank, argc, argv);
				break;
		}
	}
	textfile_close(tfh);
	return(SUCCESS);

cfl_err:
	return(FAILURE);
}


// ----

MIDIMOD midimod_create(UINT samprate) {

	UINT	size;
	MIDIMOD	ret;
	BOOL	r;

	size = sizeof(_MIDIMOD);
	size += sizeof(INSTRUMENT) * 128 * 2;
	size += sizeof(_TONECFG) * 128 * 2;
	ret = (MIDIMOD)_MALLOC(size, "MIDIMOD");
	if (ret == NULL) {
		goto mmcre_err1;
	}
	ZeroMemory(ret, size);
	ret->samprate = samprate;
	ret->tone[0] = (INSTRUMENT *)(ret + 1);
	ret->tone[1] = ret->tone[0] + 128;
	ret->tonecfg[0] = (TONECFG)(ret->tone[1] + 128);
	ret->tonecfg[1] = ret->tonecfg[0] + 128;
	ret->pathtbl = listarray_new(sizeof(_PATHLIST), 64);
	pathadd(ret, NULL);
	pathadd(ret, file_getcd(str_null));
	ret->namelist = listarray_new(MAX_NAME, 128);
	r = cfgfile_load(ret, file_timiditycfg, 0);
#if defined(TIMIDITY_CFGFILE)
	if (r != SUCCESS) {
		r = cfgfile_load(ret, TIMIDITY_CFGFILE, 0);
	}
#endif
	if (r != SUCCESS) {
		goto mmcre_err2;
	}
	return(ret);

mmcre_err2:
	_MFREE(ret);

mmcre_err1:
	return(NULL);
}

void midimod_destroy(MIDIMOD hdl) {

	UINT	r;
	TONECFG	bank;

	if (hdl) {
		r = 128;
		do {
			r--;
			inst_bankfree(hdl, r);
		} while(r > 0);
		for (r=2; r<(MIDI_BANKS*2); r++) {
			bank = hdl->tonecfg[r];
			if (bank) {
				_MFREE(bank);
			}
		}
		listarray_destroy(hdl->namelist);
		listarray_destroy(hdl->pathtbl);
		_MFREE(hdl);
	}
}

void midimod_loadprogram(MIDIMOD hdl, UINT num) {

	UINT	bank;

	if (hdl != NULL) {
		bank = (num >> 8) & 0x7f;
		num &= 0x7f;
		if (inst_singleload(hdl, bank << 1, num) != MIDIOUT_SUCCESS) {
			inst_singleload(hdl, 0, num);
		}
	}
}

void midimod_loadrhythm(MIDIMOD hdl, UINT num) {

	UINT	bank;

	if (hdl != NULL) {
		bank = (num >> 8) & 0x7f;
		num &= 0x7f;
		if (inst_singleload(hdl, (bank << 1) + 1, num) != MIDIOUT_SUCCESS) {
			inst_singleload(hdl, 1, num);
		}
	}
}

void midimod_loadgm(MIDIMOD hdl) {

	if (hdl) {
		inst_bankload(hdl, 0);
		inst_bankload(hdl, 1);
	}
}

void midimod_loadall(MIDIMOD hdl) {

	UINT	b;

	if (hdl) {
		for (b=0; b<(MIDI_BANKS*2); b++) {
			inst_bankload(hdl, b);
		}
	}
}

