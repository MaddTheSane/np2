/*	$Id: cpu_mem.h,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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

#ifndef	IA32_CPU_CPU_MEM_H__
#define	IA32_CPU_CPU_MEM_H__

#include "segments.h"

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE *cpumem;
extern BYTE protectmem_size;
extern DWORD extmem_size;

int init_cpumem(BYTE usemem);

/*
 * memory access check
 */
void cpu_memoryread_check(descriptor_t* sd, DWORD madr, DWORD length, int e);
void cpu_memorywrite_check(descriptor_t* sd, DWORD madr, DWORD length, int e);
BOOL cpu_stack_push_check(descriptor_t* sd, DWORD madr, DWORD length);
BOOL cpu_stack_pop_check(descriptor_t* sd, DWORD madr, DWORD length);
#define	CHECK_STACK_PUSH(sdp, addr, n)	cpu_stack_push_check(sdp, addr, n)
#define	CHECK_STACK_POP(sdp, addr, n)	cpu_stack_pop_check(sdp, addr, n)

/*
 * physcal address function
 */
void MEMCALL cpu_memorywrite(DWORD address, BYTE value);
void MEMCALL cpu_memorywrite_w(DWORD address, WORD value);
void MEMCALL cpu_memorywrite_d(DWORD address, DWORD value);
BYTE MEMCALL cpu_memoryread(DWORD address);
WORD MEMCALL cpu_memoryread_w(DWORD address);
DWORD MEMCALL cpu_memoryread_d(DWORD address);

/*
 * virtual address function
 */
void MEMCALL cpu_vmemorywrite(int idx, DWORD address, BYTE value);
void MEMCALL cpu_vmemorywrite_w(int idx, DWORD address, WORD value);
void MEMCALL cpu_vmemorywrite_d(int idx, DWORD address, DWORD value);
BYTE MEMCALL cpu_vmemoryread(int idx, DWORD address);
WORD MEMCALL cpu_vmemoryread_w(int idx, DWORD address);
DWORD MEMCALL cpu_vmemoryread_d(int idx, DWORD address);

/*
 * code fetch
 */
BYTE MEMCALL cpu_codefetch(DWORD madr);
WORD MEMCALL cpu_codefetch_w(DWORD madr);
DWORD MEMCALL cpu_codefetch_d(DWORD madr);

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_CPU_MEM_H__ */
