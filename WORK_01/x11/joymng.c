/*	$Id: joymng.c,v 1.2 2004/07/26 15:53:26 monaka Exp $	*/

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

#if defined(SUPPORT_JOYSTICK)

#include "np2.h"

#include "joymng.h"
#include "joydrv/joydrv.h"

static struct {
	joydrv_handle_t	hdl;

	BYTE		pad1btn[NELEMENTS(np2oscfg.JOY1BTN)];
	REG8		flag;

	joydrv_handle_t	*devlist;
	BOOL		inited;
} joyinfo = {
	NULL,

	{ 0, },
	0xff,

	NULL,
	FALSE,
};

void
joymng_initialize(void)
{
	int i;

	if (!joyinfo.inited) {
		joyinfo.devlist = joydrv_initialize();
		if (joyinfo.devlist == NULL) {
			np2oscfg.JOYPAD1 |= 2;
		}
		joyinfo.inited = TRUE;
	}

	if (joyinfo.hdl) {
		joydrv_close(joyinfo.hdl);
		joyinfo.hdl = NULL;
	}
	if (np2oscfg.JOYPAD1 == 1) {
		joyinfo.hdl = joydrv_open(np2oscfg.JOYDEV[0]);
		if (joyinfo.hdl == NULL) {
			np2oscfg.JOYPAD1 |= 2;
		}
	}

	for (i = 0; i < JOY_NBUTTON; i++) {
		joyinfo.pad1btn[i] = 0xff ^ ((np2oscfg.JOY1BTN[i] & 3) << ((np2oscfg.JOY1BTN[i] & 4) ? 4 : 6));
	}
}

void
joymng_deinitialize(void)
{

	if (joyinfo.hdl) {
		joydrv_close(joyinfo.hdl);
		joyinfo.hdl = NULL;
	}
	if (joyinfo.devlist) {
		_MFREE(joyinfo.devlist);
		joyinfo.devlist = NULL;
	}
	joydrv_terminate();
	joyinfo.inited = FALSE;
	np2oscfg.JOYPAD1 &= 1;
}

const joydrv_handle_t *
joymng_get_devinfo_list(void)
{

	return (const joydrv_handle_t *)joyinfo.devlist;
}

void
joymng_sync(void)
{

	np2oscfg.JOYPAD1 &= 0x7f;
	joyinfo.flag = 0xff;
}

REG8
joymng_getstat(void)
{
	JOYINFO_T ji;
	int i;

	if ((np2oscfg.JOYPAD1 == 1) && joyinfo.hdl) {
		if (joydrv_getstat(joyinfo.hdl, &ji) == SUCCESS) {
			np2oscfg.JOYPAD1 |= 0x80;
			joyinfo.flag = 0xff;

			/* X */
			if (ji.axis[0] > 0x4000) {
				joyinfo.flag &= ~JOY_RIGHT_BIT;
			} else if (ji.axis[0] < -0x4000) {
				joyinfo.flag &= ~JOY_LEFT_BIT;
			}

			/* Y */
			if (ji.axis[1] > 0x4000) {
				joyinfo.flag &= ~JOY_DOWN_BIT;
			} else if (ji.axis[1] < -0x4000) {
				joyinfo.flag &= ~JOY_UP_BIT;
			}

			/* button */
			for (i = 0; i < JOY_NBUTTON; ++i) {
				if (ji.button[i]) {
					joyinfo.flag &= joyinfo.pad1btn[i];
				}
			}
		}
	}
	return joyinfo.flag;
}

REG8
joymng_getstat_with_map(UINT8 *axismap, UINT8 *btnmap)
{
	JOYINFO_T ji;
	int i;
	REG8 joyflag;

	if (joyinfo.hdl) {
		if (joydrv_getstat_with_map(joyinfo.hdl, &ji, axismap, btnmap) == SUCCESS) {
			joyflag = 0xff;

			/* X */
			if (ji.axis[0] > 0x4000) {
				joyflag &= ~JOY_RIGHT_BIT;
			} else if (ji.axis[0] < -0x4000) {
				joyflag &= ~JOY_LEFT_BIT;
			}

			/* Y */
			if (ji.axis[1] > 0x4000) {
				joyflag &= ~JOY_DOWN_BIT;
			} else if (ji.axis[1] < -0x4000) {
				joyflag &= ~JOY_UP_BIT;
			}

			/* button */
			for (i = 0; i < JOY_NBUTTON; ++i) {
				if (ji.button[i]) {
					joyflag &= joyinfo.pad1btn[i];
				}
			}
			return joyflag;
		}
	}
	return 0xff;
}

void
joymng_update(void)
{

	if (joyinfo.hdl) {
		joydrv_update(joyinfo.hdl);
	}
}

int
joymng_update_task(void *p)
{

	UNUSED(p);

	if (joyinfo.hdl) {
		joydrv_update(joyinfo.hdl);
	}

	return TRUE;
}
#endif	/* SUPPORT_JOYSTICK */
