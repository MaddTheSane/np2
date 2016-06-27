//
//  ticks.c
//  Neko Project 2
//
//  Created by C.W. Betts on 6/24/16.
//  Copyright © 2016 C.W. Betts. All rights reserved.
//

#include "ticks.h"
#include <mach/mach_time.h>
#include <CoreServices/CoreServices.h>

uint64_t getcurrenttime(void) {  // in nanoseconds from system boot
	uint64_t absolute = mach_absolute_time();
	Nanoseconds nano = AbsoluteToNanoseconds(*(AbsoluteTime *)&absolute);
	return *(uint64_t *)&nano;
}
