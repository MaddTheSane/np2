
enum {
	RHYTHM_MAX		= 6
};

typedef struct {
const SINT16	*ptr;
	UINT		remain;
	UINT		volreg;
	UINT		volume;
	UINT		lr;
} RHYTHMCH;

typedef struct {
	RHYTHMCH	r[RHYTHM_MAX];
	UINT		vol;
	UINT		bitmap;
} _RHYTHM, *RHYTHM;

typedef struct {
	SINT16	*data;
	UINT	samples;
} RHYTHMPCM;

typedef struct {
	RHYTHMPCM	pcm[RHYTHM_MAX];
	UINT		vol;
	UINT		voltbl[96];
} RHYTHMCFG;


#ifdef __cplusplus
extern "C" {
#endif

void rhythm_initialize(UINT rate);
void rhythm_deinitialize(void);
void rhythm_setvol(UINT vol);

void rhythm_reset(RHYTHM rhy);
void rhythm_update(RHYTHM rhy);
void rhythm_setreg(RHYTHM rhy, BYTE reg, BYTE val);

void SOUNDCALL rhythm_getpcm(RHYTHM rhy, SINT32 *pcm, UINT count);

#ifdef __cplusplus
}
#endif

