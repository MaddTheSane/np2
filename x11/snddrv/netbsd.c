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

#if defined(USE_NETBSDAUDIO)

#include "compiler.h"

#include "np2.h"

#include "soundmng.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#include <sys/audioio.h>

#ifndef	AUDIODEV
#define	AUDIODEV	"/dev/audio"
#endif

static BOOL
netbsdaudio_init(UINT rate, UINT samples)
{
	audio_info_t a_info;
	u_int blocksize;
	u_int oldhiwat, newhiwat;
	char* audiodev;
	
	UNUSED(samples);

	if (audio_fd >= 0)
		return SUCCESS;

	if (np2oscfg.audiodev[0] == '\0')
		audiodev = AUDIODEV;
	else
		audiodev = np2oscfg.audiodev;
	audio_fd = open(audiodev, O_WRONLY);
	if (audio_fd < 0) {
		perror("audio_init: open");
		return FAILURE;
	}

	ioctl(audio_fd, AUDIO_FLUSH, 0);

	AUDIO_INITINFO(&a_info);
	if (ioctl(audio_fd, AUDIO_GETINFO, &a_info) < 0)
		return FAILURE;
	blocksize = a_info.blocksize;
	oldhiwat = a_info.hiwat;

	AUDIO_INITINFO(&a_info);
	a_info.play.encoding = AUDIO_ENCODING_SLINEAR;
	a_info.play.channels = 2;
	a_info.play.precision = 16;
	a_info.play.sample_rate = rate;

	newhiwat = (rate * 4 * 450) / 1000 / blocksize;
	if (newhiwat < oldhiwat && newhiwat > 1) {
		a_info.hiwat = newhiwat;
		a_info.lowat = (a_info.hiwat * 3) / 4;
	}

	if (ioctl(audio_fd, AUDIO_SETINFO, &a_info) < 0) {
		fprintf(stderr, "audio_init: AUDIO_SETINFO\n");
		close(audio_fd);
		return FAILURE;
	}

	return SUCCESS;
}

static BOOL
netbsdaudio_term(void)
{

	ioctl(audio_fd, AUDIO_DRAIN);
	close(audio_fd);

	return SUCCESS;
}

BOOL
netbsdaudio_setup(void)
{

	snddrv.drvinit = netbsdaudio_init;
	snddrv.drvterm = netbsdaudio_term;
	snddrv.sndplay = snddrv_play;
	snddrv.sndstop = snddrv_stop;
	snddrv.pcmload = nosound_pcmload;
	snddrv.pcmdestroy = nosound_pcmdestroy;
	snddrv.pcmplay = nosound_pcmplay;
	snddrv.pcmstop = nosound_pcmstop;
	snddrv.pcmvolume = nosound_pcmvolume;

	return SUCCESS;
}

#endif	/* USE_NETBSDAUDIO */
