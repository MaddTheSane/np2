//
//  taskmng.c
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#include "compiler.h"
#include "taskmng.h"
#include "vramhdl.h"
#include "menubase.h"

BOOL	task_avail;


void sighandler(int signo) {
	
	(void)signo;
	task_avail = FALSE;
}


void taskmng_initialize(void) {
	
	task_avail = TRUE;
}

void taskmng_exit(void) {
	
	task_avail = FALSE;
}

void taskmng_rol(void) {
	//TODO: NSEvent!
}

BOOL taskmng_sleep(UINT32 tick) {
	
	UINT32	base;
	
	base = GETTICK();
	while((task_avail) && ((GETTICK() - base) < tick)) {
		taskmng_rol();
		usleep(1);
	}
	return(task_avail);
}

