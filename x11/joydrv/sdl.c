/*	$Id: sdl.c,v 1.1 2004/07/26 15:53:27 monaka Exp $	*/

/*-
 * Copyright (c) 2004 NONAKA Kimihiro <aw9k-nnk@asahi-net.or.jp>,
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "compiler.h"

#if defined(USE_SDL_JOYSTICK)

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_joystick.h"

#include "np2.h"
#include "joymng.h"
#include "joydrv/joydrv.h"

typedef struct {
	joydrv_handle_common	common;

	SDL_Joystick		*hdl;
} joydrv_sdl_hdl;


joydrv_handle_t *
joydrv_sdl_initialize(void)
{
	char str[32];
	joydrv_sdl_hdl *shdl;
	joydrv_handle_t *devlist = NULL;
	size_t allocsize;
	int ndrv = 0;
	int rv;
	int i, n;

	rv = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	if (rv < 0) {
		return NULL;
	}

	ndrv = SDL_NumJoysticks();
	if (ndrv <= 0) {
		goto sdl_err;
	}

	allocsize = sizeof(joydrv_handle_t) * (ndrv + 1);
	devlist = _MALLOC(allocsize, "joy device list");
	if (devlist == NULL) {
		goto sdl_err;
	}
	memset(devlist, 0, allocsize);

	for (n = 0, i = 0; i < ndrv; ++i) {
		sprintf(str, "%d", i);
		devlist[n] = joydrv_sdl_open(str);
		if (devlist[n] == NULL) {
			continue;
		}
		shdl = (joydrv_sdl_hdl *)devlist[n++];
		SDL_JoystickClose(shdl->hdl);
		shdl->hdl = NULL;
	}

	return devlist;

sdl_err:
	if (devlist) {
		for (i = 0; i < ndrv; ++i) {
			if (devlist[i]) {
				joydrv_sdl_close(devlist[i]);
			}
		}
		_MFREE(devlist);
	}
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	return NULL;
}

void
joydrv_sdl_terminate(void)
{

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

joydrv_handle_t
joydrv_sdl_open(const char *dev)
{
	joydrv_handle_t hdl;
	joydrv_sdl_hdl *shdl = NULL;
	SDL_Joystick *joy = NULL;
	char *endptr;
	long lval;
	int drv;
	int ndrv;
	int naxis;
	int nbutton;
	int i;

	if (dev == NULL) {
		goto sdl_err;
	}
	errno = 0;
	lval = strtol(dev, &endptr, 10);
	if (dev[0] == '\0' || *endptr != '\0') {
		goto sdl_err;
	}
	if (errno == ERANGE && (lval == LONG_MAX || lval == LONG_MIN)) {
		goto sdl_err;
	}
	if (lval < 0 || lval > INT_MAX) {
		goto sdl_err;
	}
	drv = (int)lval;

	ndrv = SDL_NumJoysticks();
	if (ndrv <= 0 || drv >= ndrv) {
		goto sdl_err;
	}

	joy = SDL_JoystickOpen(drv);
	if (joy == NULL) {
		goto sdl_err;
	}

	naxis = SDL_JoystickNumAxes(joy);
	if (naxis < 2 || naxis >= 255) {
		goto sdl_err;
	}
	nbutton = SDL_JoystickNumButtons(joy);
	if (nbutton < 2 || nbutton >= 255) {
		goto sdl_err;
	}

	shdl = _MALLOC(sizeof(joydrv_sdl_hdl), "SDL joystick handle");
	if (shdl == NULL) {
		goto sdl_err;
	}
	memset(shdl, 0, sizeof(joydrv_sdl_hdl));
	hdl = &shdl->common;

	shdl->hdl = joy;
	hdl->devindex = drv;
	hdl->naxis = naxis;
	hdl->nbutton = nbutton;
	for (i = 0; i < JOY_NAXIS; ++i) {
		if (np2oscfg.JOYAXISMAP[0][i] < naxis) {
			hdl->axis[i] = np2oscfg.JOYAXISMAP[0][i];
		} else {
			hdl->axis[i] = JOY_AXIS_INVALID;
		}
	}
	for (i = 0; i < JOY_NBUTTON; ++i) {
		if (np2oscfg.JOYBTNMAP[0][i] < nbutton) {
			hdl->button[i] = np2oscfg.JOYBTNMAP[0][i];
		} else {
			hdl->button[i] = JOY_BUTTON_INVALID;
		}
	}
	hdl->devname = strdup(SDL_JoystickName(drv));

	return hdl;

sdl_err:
	if (shdl) {
		if (shdl->common.devname) {
			free(shdl->common.devname);
		}
		_MFREE(shdl);
	}
	if (joy) {
		SDL_JoystickClose(joy);
	}
	return NULL;
}

void
joydrv_sdl_close(joydrv_handle_t hdl)
{
	joydrv_sdl_hdl *shdl = (joydrv_sdl_hdl *)hdl;
	SDL_Joystick *joy = shdl->hdl;

	if (joy) {
		SDL_JoystickClose(joy);
	}
	if (hdl->devname) {
		free(hdl->devname);
	}
	_MFREE(shdl);
}

void
joydrv_sdl_update(joydrv_handle_t hdl)
{

	(void)hdl;

	SDL_JoystickUpdate();
}

BOOL
joydrv_sdl_getstat(joydrv_handle_t hdl, JOYINFO_T *ji)
{
	joydrv_sdl_hdl *shdl = (joydrv_sdl_hdl *)hdl;
	SDL_Joystick *joy = shdl->hdl;
	int i;

	SDL_JoystickUpdate();

	for (i = 0; i < JOY_NAXIS; ++i) {
		ji->axis[i] = (hdl->axis[i] == JOY_AXIS_INVALID) ? 0 :
		    SDL_JoystickGetAxis(joy, hdl->axis[i]);
	}
	for (i = 0; i < JOY_NBUTTON; ++i) {
		ji->button[i] = (hdl->button[i] == JOY_BUTTON_INVALID) ? 0 :
		    SDL_JoystickGetButton(joy, hdl->button[i]);
	}
	return SUCCESS;
}

BOOL
joydrv_sdl_getstat_with_map(joydrv_handle_t hdl, JOYINFO_T *ji, UINT8 *axismap, UINT8 *btnmap)
{
	joydrv_sdl_hdl *shdl = (joydrv_sdl_hdl *)hdl;
	SDL_Joystick *joy = shdl->hdl;
	int i;

	SDL_JoystickUpdate();

	for (i = 0; i < JOY_NAXIS; ++i) {
		ji->axis[i] = (axismap[i] == JOY_AXIS_INVALID) ? 0 :
		    SDL_JoystickGetAxis(joy, axismap[i]);
	}
	for (i = 0; i < JOY_NBUTTON; ++i) {
		ji->button[i] = (btnmap[i] == JOY_BUTTON_INVALID) ? 0 :
		    SDL_JoystickGetButton(joy, btnmap[i]);
	}
	return SUCCESS;
}
#endif	/* USE_SDL_JOYSTICK */
