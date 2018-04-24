//
//  mousemng.h
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#ifndef mousemng_h
#define mousemng_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT8 mousemng_getstat(SINT16 *x, SINT16 *y, int clear);

#ifdef __cplusplus
}
#endif


#endif /* mousemng_h */
