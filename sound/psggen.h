
enum {
	PSGFREQPADBIT		= 12,
	PSGADDEDBIT			= 3
};

enum {
	PSGENV_INC			= 15,
	PSGENV_ONESHOT		= 16,
	PSGENV_LASTON		= 32,
	PSGENV_ONECYCLE		= 64
};

typedef struct {
	SINT32	freq;
	SINT32	count;
	BYTE	*pvol;
	UINT16	puchi;
	UINT16	padding;
} PSGTONE;

typedef struct {
	SINT32	freq;
	SINT32	count;
	UINT	base;
} PSGNOISE;

typedef struct {
	BYTE	tune[3][2];		// 0
	BYTE	noise;			// 6
	BYTE	mixer;			// 7
	BYTE	vol0;			// 8
	BYTE	vol1;			// 9
	BYTE	vol2;			// a
	BYTE	envtime[2];		// b
	BYTE	env;			// d
	BYTE	io1;
	BYTE	io2;
} PSGREG;

typedef struct {
	PSGTONE		tone[3];
	PSGNOISE	noise;
	PSGREG		reg;
	UINT16		envcnt;
	UINT16		envmax;
	BYTE		envmode;
	BYTE		envvol;
	char		envvolcnt;
	BYTE		mixer;
	UINT		puchicount;
} _PSGGEN, *PSGGEN;

typedef struct {
	SINT32	volume[16];
	SINT32	voltbl[16];
	UINT	rate;
	UINT32	base;
	UINT16	puchidec;
} PSGGENCFG;


#ifdef __cplusplus
extern "C" {
#endif

void psggen_initialize(UINT rate);
void psggen_setvol(UINT vol);

void psggen_reset(PSGGEN psg);
void psggen_setreg(PSGGEN psg, BYTE reg, BYTE val);
BYTE psggen_getreg(PSGGEN psg, BYTE reg);

void SOUNDCALL psggen_getpcm(PSGGEN psg, SINT32 *pcm, UINT count);

#ifdef __cplusplus
}
#endif

