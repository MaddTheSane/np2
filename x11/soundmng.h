/*	$Id: soundmng.h,v 1.5 2008/04/02 13:04:00 monaka Exp $	*/

#ifndef	NP2_X11_SOUNGMNG_H__
#define	NP2_X11_SOUNGMNG_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SOUND_PCMSEEK,
	SOUND_PCMSEEK1,
	SOUND_MAXPCM
};

enum {
	SNDDRV_NODRV,
	SNDDRV_SDL,
	SNDDRV_DRVMAX
};

BYTE snddrv_drv2num(const char *);
const char *snddrv_num2drv(BYTE);

#if !defined(NOSOUND)
UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL reverse);

BOOL soundmng_pcmplay(UINT num, BOOL loop);
void soundmng_pcmstop(UINT num);

/* ---- for X11 */

BOOL soundmng_initialize(void);
void soundmng_deinitialize(void);

BOOL soundmng_pcmload(UINT num, const char *filename);
void soundmng_pcmvolume(UINT num, int volume);

#else	/* NOSOUND */

#define soundmng_create(rate, ms)	0, (void)rate, (void)ms
#define	soundmng_destroy()
#define	soundmng_reset()
#define	soundmng_play()
#define	soundmng_stop()
#define	soundmng_sync()
#define	soundmng_setreverse(reverse)

#define	soundmng_pcmplay(num, loop)
#define	soundmng_pcmstop(num)

/* ---- for X11 */

#define	soundmng_initialize()		np2cfg.SOUND_SW = 0, FAILURE
#define	soundmng_deinitialize()

#define	soundmng_pcmload(num, filename)	FAILURE
#define	soundmng_pcmvolume(num, volume)

#endif	/* !NOSOUND */

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_SOUNGMNG_H__ */
