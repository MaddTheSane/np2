/*	$Id: resolve.h,v 1.3 2004/02/05 16:43:44 monaka Exp $	*/

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

#ifndef	IA32_CPU_RESOLVE_H__
#define	IA32_CPU_RESOLVE_H__

#ifdef __cplusplus
extern "C" {
#endif

void resolve_init(void);

#if defined(DEBUG) || !defined(IA32_INLINE_CALC_EA)
DWORD calc_ea_dst(DWORD);
#else
DWORD (*calc_ea_dst_tbl[0x100])(void);
DWORD (*calc_ea32_dst_tbl[0x100])(void);

INLINE static DWORD
calc_ea_dst(DWORD op)
{

	__ASSERT(op < 0x100);

	if (!CPU_INST_AS32)
		return ((*calc_ea_dst_tbl[op])() & 0xffff);
	return (*calc_ea32_dst_tbl[op])();
}
#endif

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_RESOLVE_H__ */
