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

#ifndef	NP2_X11_TOOLKIT_H__
#define	NP2_X11_TOOLKIT_H__

#if !defined(USE_GTK) && !defined(USE_QT)
#error undefined both USE_GTK and USE_QT!!!
#endif

typedef struct {
	const char*	(*get_toolkit)(void);
	BOOL		(*arginit)(int* argc, char*** argv);
	void		(*widget_create)(void);
	void		(*widget_show)(void);
	void		(*widget_mainloop)(void);
	void		(*widget_quit)(void);
	void		(*set_window_title)(const char* str);
} gui_toolkit_t;

#if defined(USE_GTK) && defined(USE_QT)

extern gui_toolkit_t* toolkitp;

void toolkit_initialize(void);
#define	toolkit_arginit(argcp, argvp)	(*toolkitp->arginit)(argcp, argvp)
#define	toolkit_widget_create()		(*widget_create)()
#define	toolkit_widget_show()		(*widget_show)()
#define	toolkit_widget_mainloop()	(*widget_mainloop)()
#define	toolkit_widget_quit()		(*widget_quit)()
#define	toolkit_set_window_title(s)	(*toolkitp->set_window_title)(s)

#elif defined(USE_GTK)

#include "gtk/gtktoolkit.h"

#define	toolkit_initialize()
#define	toolkit_arginit(argcp, argvp)	gui_gtk_arginit(argcp, argvp)
#define	toolkit_widget_create()		gui_gtk_widget_create()
#define	toolkit_widget_show()		gui_gtk_widget_show()
#define	toolkit_widget_mainloop()	gui_gtk_widget_mainloop()
#define	toolkit_widget_quit()		gui_gtk_widget_quit()
#define	toolkit_set_window_title(s)	gui_gtk_set_window_title(s)

#elif defined(USE_QT)

#define	toolkit_initialize()
#define	toolkit_arginit(argcp, argvp)
#define	toolkit_widget_create()	
#define	toolkit_widget_show()	
#define	toolkit_widget_mainloop()
#define	toolkit_widget_quit()
#define	toolkit_set_window_title(s)

#endif

#endif	/* NP2_X11_TOOLKIT_H__ */
