
enum {
	SOUND_PCMSEEK		= 0,
	SOUND_PCMSEEK1		= 1,

	SOUND_MAXPCM
};


#ifdef __cplusplus
extern "C" {
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL reverse);

#define	soundmng_pcmplay(a, b)
#define	soundmng_pcmstop(a)


// ---- for windows

BOOL soundmng_initialize(void);
void soundmng_deinitialize(void);

void soundmng_enable(void);
void soundmng_disable(void);

#ifdef __cplusplus
}
#endif

