/*
 * Copyright (c) 2003 NONAKA Kimihiro
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

#include "compiler.h"

#include "np2.h"

#include "toolkit.h"

#include "sysmng.h"

#if defined(USE_GTK) && defined(USE_QT)

gui_toolkit_t* toolkitp;

static struct {
	gui_toolkit_t*	toolkit;
} toolkit[] = {
	{ &gtk_toolkit, },
	{ &qt_toolkit, },
};

void
toolkit_initialize(void)
{
	int i;

	if (NELEMENTS(toolkit) > 0) {
		for (i = 0; i < NELEMENTS(toolkit); i++) {
			gui_toolkit_t* p = toolkit[i].toolkit;
			if (strcasecmp(p->get_toolkit(), np2oscfg.toolkit) == 0)
				break;
		}
		if (i < NELEMENTS(toolkit)) {
			toolkitp = toolkit[i].toolkit;
			return;
		}
		sysmng_update(SYS_UPDATEOSCFG);
		milstr_ncpy(np2oscfg.toolkit, "gtk", sizeof(np2oscfg.toolkit));
	}
	toolkitp = &gtk_toolkit;
}

#endif	/* USE_GTK && USE_QT */
