/*	$Id: skbdwin.h,v 1.1 2004/06/16 12:50:49 monaka Exp $	*/

#ifndef	NP2_X11_SKBDWIN_H__
#define	NP2_X11_SKBDWIN_H__

#if defined(SUPPORT_SOFTKBD)

#include "softkbd.h"

typedef struct {
	int	posx;
	int	posy;
	BYTE	type;
} SKBDCFG;

extern SKBDCFG skbdcfg;

BOOL skbdwin_initialize(void);
void skbdwin_deinitialize(void);
void skbdwin_create(void);
void skbdwin_destroy(void);
void skbdwin_process(void);
void skbdwin_readini(void);
void skbdwin_writeini(void);

#else	/* !SUPPORT_SOFTKBD */

#define	skbdwin_initialize()
#define	skbdwin_create()
#define	skbdwin_destroy()
#define	skbdwin_draw()
#define	skbdwin_readini()
#define	skbdwin_writeini()

#endif	/* SUPPORT_SOFTKBD */

#endif	/* NP2_X11_SKBDWIN_H__ */
