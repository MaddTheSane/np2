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

#if defined(USE_ESDAUDIO)

#include "compiler.h"

#include "soundmng.h"

#include <esd.h>


static BOOL
esdaudio_init(UINT rate, UINT samples)
{

	UNUSED(samples);

	if (audio_fd >= 0)
		return SUCCESS;

	audio_fd = esd_play_stream_fallback(
	    ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_PLAY,
	    rate, NULL, "xnp2");
	if (audio_fd < 0) {
		fprintf(stderr, "esdaudio_init: ESD open\n");
		return FAILURE;
	}
	return SUCCESS;
}

static BOOL
esdaudio_term(void)
{

	esd_close(audio_fd);

	return SUCCESS;
}

BOOL
esdaudio_setup(void)
{

	snddrv.drvinit = esdaudio_init;
	snddrv.drvterm = esdaudio_term;
	snddrv.sndplay = snddrv_play;
	snddrv.sndstop = snddrv_stop;
	snddrv.pcmload = nosound_pcmload;
	snddrv.pcmdestroy = nosound_pcmdestroy;
	snddrv.pcmplay = nosound_pcmplay;
	snddrv.pcmstop = nosound_pcmstop;
	snddrv.pcmvolume = nosound_pcmvolume;

	return SUCCESS;
}

#endif	/* USE_ESDAUDIO */
