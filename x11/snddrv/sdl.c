/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(USE_SDLAUDIO) || defined(USE_SDLMIXER)

#include "compiler.h"

#include "soundmng.h"

#include "SDL.h"

static void sdlaudio_callback(void *, unsigned char *, int);


static BOOL
sdlaudio_init(UINT rate, UINT samples)
{
	SDL_AudioSpec fmt;
	int rv;

	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
	fmt.userdata = (void *)(samples * 2 * sizeof(SINT16));

	rv = SDL_Init(SDL_INIT_AUDIO);
	if (rv < 0) {
		fprintf(stderr, "sdlaudio_init: SDL_Init\n");
		return FAILURE;
	}

	audio_fd = SDL_OpenAudio(&fmt, NULL);
	if (audio_fd < 0) {
		fprintf(stderr, "sdlaudio_init: SDL_OpenAudio\n");
		SDL_Quit();
		return FAILURE;
	}

	return SUCCESS;
}

static BOOL
sdlaudio_term(void)
{

	SDL_CloseAudio();
	SDL_Quit();

	return SUCCESS;
}

static void
sdlaudio_play(void)
{

	SDL_PauseAudio(0);
}

static void
sdlaudio_stop(void)
{

	SDL_PauseAudio(1);
}

BOOL
sdlaudio_setup(void)
{

	snddrv.drvinit = sdlaudio_init;
	snddrv.drvterm = sdlaudio_term;
	snddrv.sndplay = sdlaudio_play;
	snddrv.sndstop = sdlaudio_stop;
	snddrv.pcmload = nosound_pcmload;
	snddrv.pcmdestroy = nosound_pcmdestroy;
	snddrv.pcmplay = nosound_pcmplay;
	snddrv.pcmstop = nosound_pcmstop;
	snddrv.pcmvolume = nosound_pcmvolume;

	return SUCCESS;
}

#if defined(USE_SDLMIXER)

#include "SDL_mixer.h"

static BOOL
sdlmixer_init(UINT rate, UINT samples)
{
	int rv;

	rv = SDL_Init(SDL_INIT_AUDIO);
	if (rv < 0) {
		fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());
		goto failure;
	}

	rv = Mix_OpenAudio(rate, AUDIO_S16SYS, 2, samples);
	if (rv < 0) {
		fprintf(stderr, "Mix_OpenAudio(): %s\n", Mix_GetError());
		goto failure1;
	}
	SDL_PauseAudio(1);

	rv = Mix_AllocateChannels(SOUND_MAXPCM);
	if (rv < 0) {
		fprintf(stderr, "Mix_AllocateChannels(): %s\n", Mix_GetError());
		goto failure2;
	}

	Mix_HookMusic(sdlaudio_callback, (void*)(samples * 2 * sizeof(SINT16)));

	return SUCCESS;

failure2:
	Mix_CloseAudio();
failure1:
	SDL_Quit();
failure:
	return FAILURE;
}

static BOOL
sdlmixer_term(void)
{

	Mix_CloseAudio();
	SDL_Quit();

	return SUCCESS;
}

static void
sdlmixer_play(void)
{

	SDL_PauseAudio(0);
}

static void
sdlmixer_stop(void)
{

	SDL_PauseAudio(1);
}

static void *
sdlmixer_pcmload(UINT num, const char *path)
{
	Mix_Chunk *chunk;

	UNUSED(num);

	chunk = Mix_LoadWAV(path);
	return (void *)chunk;
}

static void
sdlmixer_pcmdestroy(void *chanp, UINT num)
{
	Mix_Chunk *chunk = (Mix_Chunk *)chanp;

	Mix_HaltChannel(num);
	Mix_FreeChunk(chunk);
}

static void
sdlmixer_pcmplay(void *chanp, UINT num, BOOL loop)
{
	Mix_Chunk *chunk = (Mix_Chunk *)chanp;

	Mix_PlayChannel(num, chunk, loop ? -1 : 1);
}

static void
sdlmixer_pcmstop(void *chanp, UINT num)
{

	UNUSED(chanp);

	Mix_HaltChannel(num);
}

static void
sdlmixer_pcmvolume(void *chanp, UINT num, int volume)
{

	UNUSED(chanp);

	Mix_Volume(num, (MIX_MAX_VOLUME * volume) / 100);
}

BOOL
sdlmixer_setup(void)
{

	snddrv.drvinit = sdlmixer_init;
	snddrv.drvterm = sdlmixer_term;
	snddrv.sndplay = sdlmixer_play;
	snddrv.sndstop = sdlmixer_stop;
	snddrv.pcmload = sdlmixer_pcmload;
	snddrv.pcmdestroy = sdlmixer_pcmdestroy;
	snddrv.pcmplay = sdlmixer_pcmplay;
	snddrv.pcmstop = sdlmixer_pcmstop;
	snddrv.pcmvolume = sdlmixer_pcmvolume;

	return SUCCESS;
}

#endif	/* USE_SDLMIXER */

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{
	UINT samples = (UINT)userdata;

	UNUSED(len);

	if (sound_event != NULL)
		memset(sound_event, 0, samples);

	SDL_MixAudio(stream, sound_buffer[sound_nextbuf], samples,
	    SDL_MIX_MAXVOLUME);

	sound_nextbuf = (sound_nextbuf + 1) % NSOUNDBUFFER;
	sound_event = sound_buffer[sound_nextbuf];
}

#endif	/* USE_SDLAUDIO || USE_SDLMIXER */
