/*	$Id: interface.h,v 1.5 2004/01/25 05:41:29 yui Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	IA32_CPU_INTERFACE_H__
#define	IA32_CPU_INTERFACE_H__

typedef signed char		SBYTE;
typedef signed short		SWORD;
typedef signed int		SDWORD;

typedef	UINT64			QWORD;
typedef	SINT64			SQWORD;

#define CPU_isDI	(!(CPU_FLAG & I_FLAG))
#define CPU_isEI	(CPU_FLAG & I_FLAG)

#define CPU_INITIALIZE()	i386c_initialize()
#define	CPU_DEINITIALIZE()
#define	CPU_RESET()		ia32reset()
#define	CPU_CLEARPREFETCH()
#define	CPU_INTERRUPT(vect)	ia32_interrupt(vect)
#define	CPU_EXEC()		ia32()
#define	CPU_EXECV30()		ia32()
#define	CPU_SHUT()		ia32shut()
#define	CPU_SETEXTSIZE(size)	init_cpumem(size)
// #define CPU_SETEMM(frame, addr)

void i386c_initialize(void);

void FASTCALL msgbox_str(char *msg);
void FASTCALL msgbox_mem(DWORD no);
void put_cpuinfo(void);

#endif	/* IA32_CPU_INTERFACE_H__ */
