/*	$Id: shift_rotate.h,v 1.1 2003/12/08 00:55:32 yui Exp $	*/

/*
 * Copyright (c) 2003 NONAKA Kimihiro
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

#ifndef	IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__
#define	IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SAR
 */
void SAR_Eb(BYTE *);
void SAR_Ew(WORD *);
void SAR_Ed(DWORD *);
void SAR_Eb_ext(DWORD);
void SAR_Ew_ext(DWORD);
void SAR_Ed_ext(DWORD);
void SAR_EbCL(BYTE *, BYTE);
void SAR_EbCL_ext(DWORD, BYTE);
void SAR_EwCL(WORD *, BYTE);
void SAR_EwCL_ext(DWORD, BYTE);
void SAR_EdCL(DWORD *, BYTE);
void SAR_EdCL_ext(DWORD, BYTE);

/*
 * SHR
 */
void SHR_Eb(BYTE *);
void SHR_Ew(WORD *);
void SHR_Ed(DWORD *);
void SHR_Eb_ext(DWORD);
void SHR_Ew_ext(DWORD);
void SHR_Ed_ext(DWORD);
void SHR_EbCL(BYTE *, BYTE);
void SHR_EbCL_ext(DWORD, BYTE);
void SHR_EwCL(WORD *, BYTE);
void SHR_EwCL_ext(DWORD, BYTE);
void SHR_EdCL(DWORD *, BYTE);
void SHR_EdCL_ext(DWORD, BYTE);

/*
 * SHL
 */
void SHL_Eb(BYTE *);
void SHL_Ew(WORD *);
void SHL_Ed(DWORD *);
void SHL_Eb_ext(DWORD);
void SHL_Ew_ext(DWORD);
void SHL_Ed_ext(DWORD);
void SHL_EbCL(BYTE *, BYTE);
void SHL_EbCL_ext(DWORD, BYTE);
void SHL_EwCL(WORD *, BYTE);
void SHL_EwCL_ext(DWORD, BYTE);
void SHL_EdCL(DWORD *, BYTE);
void SHL_EdCL_ext(DWORD, BYTE);

/*
 * SHRD
 */
void SHRD_EwGwIb(void);
void SHRD_EdGdIb(void);
void SHRD_EwGwCL(void);
void SHRD_EdGdCL(void);

/*
 * SHLD
 */
void SHLD_EwGwIb(void);
void SHLD_EdGdIb(void);
void SHLD_EwGwCL(void);
void SHLD_EdGdCL(void);

/*
 * ROR
 */
void ROR_Eb(BYTE *);
void ROR_Ew(WORD *);
void ROR_Ed(DWORD *);
void ROR_Eb_ext(DWORD);
void ROR_Ew_ext(DWORD);
void ROR_Ed_ext(DWORD);
void ROR_EbCL(BYTE *, BYTE);
void ROR_EbCL_ext(DWORD, BYTE);
void ROR_EwCL(WORD *, BYTE);
void ROR_EwCL_ext(DWORD, BYTE);
void ROR_EdCL(DWORD *, BYTE);
void ROR_EdCL_ext(DWORD, BYTE);

/*
 * ROL
 */
void ROL_Eb(BYTE *);
void ROL_Ew(WORD *);
void ROL_Ed(DWORD *);
void ROL_Eb_ext(DWORD);
void ROL_Ew_ext(DWORD);
void ROL_Ed_ext(DWORD);
void ROL_EbCL(BYTE *, BYTE);
void ROL_EbCL_ext(DWORD, BYTE);
void ROL_EwCL(WORD *, BYTE);
void ROL_EwCL_ext(DWORD, BYTE);
void ROL_EdCL(DWORD *, BYTE);
void ROL_EdCL_ext(DWORD, BYTE);

/*
 * RCR
 */
void RCR_Eb(BYTE *);
void RCR_Ew(WORD *);
void RCR_Ed(DWORD *);
void RCR_Eb_ext(DWORD);
void RCR_Ew_ext(DWORD);
void RCR_Ed_ext(DWORD);
void RCR_EbCL(BYTE *, BYTE);
void RCR_EbCL_ext(DWORD, BYTE);
void RCR_EwCL(WORD *, BYTE);
void RCR_EwCL_ext(DWORD, BYTE);
void RCR_EdCL(DWORD *, BYTE);
void RCR_EdCL_ext(DWORD, BYTE);

/*
 * RCL
 */
void RCL_Eb(BYTE *);
void RCL_Ew(WORD *);
void RCL_Ed(DWORD *);
void RCL_Eb_ext(DWORD);
void RCL_Ew_ext(DWORD);
void RCL_Ed_ext(DWORD);
void RCL_EbCL(BYTE *, BYTE);
void RCL_EbCL_ext(DWORD, BYTE);
void RCL_EwCL(WORD *, BYTE);
void RCL_EwCL_ext(DWORD, BYTE);
void RCL_EdCL(DWORD *, BYTE);
void RCL_EdCL_ext(DWORD, BYTE);

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__ */
