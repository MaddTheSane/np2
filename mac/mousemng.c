//
//  mousemng.c
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#include	"compiler.h"
#include	"mousemng.h"


UINT8 mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {
	
	*x = 0;
	*y = 0;
	(void)clear;
	return(0xa0);
}

