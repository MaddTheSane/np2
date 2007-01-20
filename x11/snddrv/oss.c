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

#include "compiler.h"

#if defined(USE_OSSAUDIO)

#include "np2.h"

#include "soundmng.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#if defined(__FreeBSD__)
#include <machine/soundcard.h>
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#include <soundcard.h>
#elif defined(__linux__)
#include <sys/soundcard.h>
#endif

#ifndef	AUDIODEV
#define	AUDIODEV	"/dev/dsp"
#endif


static BOOL
ossaudio_init(UINT rate, UINT samples)
{
	int sample_rate, is_stereo, encoding, fragment;
	char* audiodev;

        if (audio_fd >= 0)
                return SUCCESS;

	if (np2oscfg.audiodev[0] == '\0')
		audiodev = AUDIODEV;
	else
		audiodev = np2oscfg.audiodev;
	audio_fd = open(audiodev, O_WRONLY);
	if (audio_fd < 0) {
		perror("ossaudio_init: open");
		return FAILURE;
	}

	ioctl(audio_fd, SNDCTL_DSP_RESET, 0);

#if defined(BYTESEX_BIG)
	encoding = AFMT_S16_BE;
#else
	encoding = AFMT_S16_LE;
#endif
	if (ioctl(audio_fd, SNDCTL_DSP_SAMPLESIZE, &encoding) < 0) {
		fprintf(stderr, "audio_init: ioctl(SNDCTL_DSP_SAMPLESIZE)\n");
		goto failure;
	}

	is_stereo = 1;
	if (ioctl(audio_fd, SNDCTL_DSP_STEREO, &is_stereo) < 0) {
		fprintf(stderr, "audio_init: ioctl(SNDCTL_DSP_STEREO)\n");
		goto failure;
	}

	sample_rate = rate;
	if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &sample_rate) < 0) {
		fprintf(stderr, "audio_init: ioctl(SNDCTL_DSP_SPEED)\n");
		goto failure;
	}

#if defined(SNDCTL_DSP_SETFRAGMENT)
	fragment = calc_fragment(samples * 4);
	fragment |= 0x00020000;
	if (ioctl(audio_fd, SNDCTL_DSP_GETBLKSIZE, &fragment) < 0) {
		fprintf(stderr, "audio_init: ioctl(SNDCTL_DSP_GETBLKSIZE)\n");
		goto failure;
	}
#endif

	return SUCCESS;

failure:
	close(audio_fd);
	audio_fd = -1;
	return FAILURE;
}

static BOOL
ossaudio_term(void)
{

	ioctl(audio_fd, SNDCTL_DSP_SYNC, 0);
	close(audio_fd);

	return SUCCESS;
}

int
ossaudio_setup(void)
{

	snddrv.drvinit = ossaudio_init;
	snddrv.drvterm = ossaudio_term;
	snddrv.sndplay = snddrv_play;
	snddrv.sndstop = snddrv_stop;
	snddrv.pcmload = nosound_pcmload;
	snddrv.pcmdestroy = nosound_pcmdestroy;
	snddrv.pcmplay = nosound_pcmplay;
	snddrv.pcmstop = nosound_pcmstop;
	snddrv.pcmvolume = nosound_pcmvolume;

	return SUCCESS;
}

#endif	/* USE_OSSAUDIO */
