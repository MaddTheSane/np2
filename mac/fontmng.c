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

void *fontmng_create(int size, FDAT_FLAGS type, const char *fontface)
{
	return NULL;
}

void fontmng_destroy(void *hdl)
{
	
}

BRESULT fontmng_getsize(void *hdl, const char *string, POINT_T *pt)
{
	return false;
}

BRESULT fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt)
{
	return false;
}

FNTDAT fontmng_get(void *hdl, const char *string)
{
	return NULL;
}

