/*	$Id: interface.h,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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

#if 0		// このあたりも compiler.hへ
#include "compiler.h"

#include <assert.h>
#include <stdarg.h>

#include "memory.h"
#include "nevent.h"
#include "iobridge.h"

#include "bios.h"
#include "cpuio.h"
#include "dmac.h"

#if defined(_DEBUG)
#define	ASSERT(v)	assert(v)
#define	VERBOSE(v)	ia32_warning v
#else
#define	ASSERT(v)
#define	VERBOSE(v)
#endif
#endif

// --> あとで common.hへ
#ifndef	INLINE
#define	INLINE
#endif

#if 1			// これ SINT64/UINT64 を作ること
typedef	unsigned __int64	QWORD;
typedef signed __int64		SQWORD;
#else
typedef	unsigned long long	QWORD;
typedef signed long long	SQWORD;
#endif

typedef signed char		SBYTE;
typedef signed short		SWORD;
typedef signed int		SDWORD;

#if 0
#define I286_AL		CPU_AL
#define I286_CL		CPU_CL
#define I286_DL		CPU_DL
#define I286_BL		CPU_BL
#define I286_AH		CPU_AH
#define I286_CH		CPU_CH
#define I286_DH		CPU_DH
#define I286_BH		CPU_BH

#define I286_AX		CPU_AX
#define I286_CX		CPU_CX
#define I286_DX		CPU_DX
#define I286_BX		CPU_BX
#define I286_SP		CPU_SP
#define I286_BP		CPU_BP
#define I286_SI		CPU_SI
#define I286_DI		CPU_DI
#define I286_IP		CPU_IP

#define I286_EAX	CPU_EAX
#define I286_ECX	CPU_ECX
#define I286_EDX	CPU_EDX
#define I286_EBX	CPU_EBX
#define I286_ESP	CPU_ESP
#define I286_EBP	CPU_EBP
#define I286_ESI	CPU_ESI
#define I286_EDI	CPU_EDI
#define I286_EIP	CPU_EIP

#define I286_ES		CPU_ES
#define I286_CS		CPU_CS
#define I286_SS		CPU_SS
#define I286_DS		CPU_DS
#define I286_FS		CPU_FS
#define I286_GS		CPU_GS

#define I286_EFLAG	CPU_EFLAG
#define I286_FLAG	CPU_FLAG
#define I286_FLAGL	CPU_FLAGL
#define I286_FLAGH	CPU_FLAGH
#define I286_TRAP	CPU_TRAP
#define I286_INPORT	CPU_INPORT
#define I286_OV		CPU_OV
#define i286s		cpu_stat
#endif

#define CPU_isDI	(!(CPU_FLAG & I_FLAG))
#define CPU_isEI	(CPU_FLAG & I_FLAG)

#define CPU_INITIALIZE()	i386c_initialize()
#define	CPU_RESET()		ia32reset()
#define	CPU_CLEARPREFETCH()
#define	CPU_INTERRUPT(a)	ia32_interrupt(a)
#define	CPU_EXEC()		ia32()
#define	CPU_EXECV30()		ia32()

// #define	CPU_WITHTRAP()		ia32withtrap()
// #define	CPU_WITHDMA()		ia32withdma()
// #define	CPU_STEP()		ia32_step()

void FASTCALL msgbox_str(char *msg);
void FASTCALL msgbox_mem(DWORD no);
void put_cpuinfo(void);

#endif	/* IA32_CPU_INTERFACE_H__ */
