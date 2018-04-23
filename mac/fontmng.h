//
//  fontmng.h
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#ifndef fontmng_h
#define fontmng_h

#include <stdio.h>
#include <CoreFoundation/CFBase.h>

typedef CF_OPTIONS(UINT, FDAT_FLAGS) {
	FDAT_BOLD			= 0x01,
	FDAT_PROPORTIONAL	= 0x02,
	FDAT_ALIAS			= 0x04,
	FDAT_ANSI			= 0x08
};

enum {
	FDAT_DEPTH			= 255,
	FDAT_DEPTHBIT		= 8
};

typedef struct {
	int		width;
	int		height;
	int		pitch;
} _FNTDAT, *FNTDAT;

#ifdef __cplusplus
extern "C" {
#endif

void *fontmng_create(int size, FDAT_FLAGS type, const char *fontface);
void fontmng_destroy(void *hdl);

BRESULT fontmng_getsize(void *hdl, const char *string, POINT_T *pt);
BRESULT fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt);
FNTDAT fontmng_get(void *hdl, const char *string);

#ifdef __cplusplus
}
#endif


#endif /* fontmng_h */
