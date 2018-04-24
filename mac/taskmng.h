//
//  taskmng.h
//  Neko Project 2
//
//  Created by C.W. Betts on 4/22/18.
//  Copyright © 2018 C.W. Betts. All rights reserved.
//

#ifndef taskmng_h
#define taskmng_h

#include <stdio.h>

extern	BOOL	task_avail;

void taskmng_initialize(void);
void taskmng_exit(void);
void taskmng_rol(void);
#define	taskmng_isavail()		(task_avail)
BOOL taskmng_sleep(UINT32 tick);

#endif /* taskmng_h */
