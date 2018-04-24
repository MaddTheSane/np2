//
//  np2.m
//  Neko Project 2
//
//  Created by C.W. Betts on 4/23/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "np2.h"

NP2OSCFG	np2oscfg = {0, 0, 0, 0, 0};
static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;
