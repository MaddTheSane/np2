//
//  soundmng.h
//  Neko Project 2
//
//  Created by C.W. Betts on 6/24/16.
//  Copyright © 2016 C.W. Betts. All rights reserved.
//

#ifndef soundmng_h
#define soundmng_h


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
#define soundmng_reset()
void soundmng_play(void);
void soundmng_stop(void);
#define soundmng_sync()
#define soundmng_setreverse(r)
	
#define	soundmng_pcmplay(a, b)
#define	soundmng_pcmstop(a)


#ifdef __cplusplus
}
#endif

#endif /* soundmng_h */
