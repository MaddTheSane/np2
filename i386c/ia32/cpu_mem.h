/*	$Id: cpu_mem.h,v 1.5 2004/03/12 13:34:08 monaka Exp $	*/

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

#include "memory.h"
#include "segments.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * memory access check
 */
void cpu_memoryread_check(descriptor_t *sd, UINT32 madr, UINT length, int e);
void cpu_memorywrite_check(descriptor_t *sd, UINT32 madr, UINT length, int e);
void cpu_stack_push_check(UINT16 s, descriptor_t *sd, UINT32 madr, UINT length);
void cpu_stack_pop_check(UINT16 s, descriptor_t *sd, UINT32 madr, UINT length);
#define	STACK_PUSH_CHECK(s, sd, addr, n) cpu_stack_push_check(s, sd, addr, n)
#define	STACK_POP_CHECK(s, sd, addr, n) cpu_stack_pop_check(s, sd, addr, n)

/*
 * virtual address function
 */
void MEMCALL cpu_vmemorywrite(int idx, UINT32 address, UINT8 value);
void MEMCALL cpu_vmemorywrite_w(int idx, UINT32 address, UINT16 value);
void MEMCALL cpu_vmemorywrite_d(int idx, UINT32 address, UINT32 value);
UINT8 MEMCALL cpu_vmemoryread(int idx, UINT32 address);
UINT16 MEMCALL cpu_vmemoryread_w(int idx, UINT32 address);
UINT32 MEMCALL cpu_vmemoryread_d(int idx, UINT32 address);

/*
 * code fetch
 */
UINT8 MEMCALL cpu_codefetch(UINT32 madr);
UINT16 MEMCALL cpu_codefetch_w(UINT32 madr);
UINT32 MEMCALL cpu_codefetch_d(UINT32 madr);

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_CPU_MEM_H__ */
