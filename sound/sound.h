
#ifndef SOUNDCALL
#define	SOUNDCALL
#endif

typedef void (SOUNDCALL * SOUNDCB)(void *hdl, SINT32 *pcm, UINT count);


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT32	opna_rate;
extern	UINT32	dsound_lastclock;


BOOL sound_create(UINT rate, UINT ms);
void sound_destroy(void);

void sound_reset(void);
void sound_streamregist(void *hdl, SOUNDCB cbfn);

void sound_sync(void);

const SINT32 *sound_pcmlock(void);
void sound_pcmunlock(const SINT32 *hdl);

#ifdef __cplusplus
}
#endif

