//
//  fontmng.c
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#include "compiler.h"
#include "fontmng.h"
#include <CoreText/CoreText.h>

static CFStringRef osakaFont = CFSTR("Osaka−等幅");

struct TagFontManager {
	int				fontsize;
	UINT			fonttype;
	int				fontwidth;
	int				fontheight;

	int			ptsize;
	int			fontalign;
	CTFontRef	font;
};

typedef struct TagFontManager		*FNTMNG;	/*!< Defines handle */


void *fontmng_create(int size, FDAT_FLAGS type, const char *fontface)
{
	if (size < 0) {
		size *= -1;
	}
	if (size < 6) {
		size = 6;
	}
	else if (size > 128) {
		size = 128;
	}

	return NULL;
}

void fontmng_destroy(void *hdl)
{
	
}

BRESULT fontmng_getsize(void *hdl, const char *string, POINT_T *pt)
{
	return FAILURE;
}

BRESULT fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt)
{
	return FAILURE;
}

FNTDAT fontmng_get(void *hdl, const char *string)
{
	return NULL;
}

