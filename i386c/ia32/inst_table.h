/*	$Id: inst_table.h,v 1.3 2004/01/14 16:14:49 monaka Exp $	*/

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

#ifndef	IA32_CPU_INST_TABLE_H__
#define	IA32_CPU_INST_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* info of instruction */
extern	BYTE insttable_info[256];

/* table of instruction */
extern	void (*insttable_1byte[2][256])(void);
extern	void (*insttable_2byte[2][256])(void);


/*
 * for group
 */

/* group 1 */
extern	void (*insttable_G1EbIb[])(BYTE *, DWORD);
extern	void (*insttable_G1EwIx[])(WORD *, DWORD);
extern	void (*insttable_G1EdIx[])(DWORD *, DWORD);
extern	void (*insttable_G1EbIb_ext[])(DWORD, DWORD);
extern	void (*insttable_G1EwIx_ext[])(DWORD, DWORD);
extern	void (*insttable_G1EdIx_ext[])(DWORD, DWORD);

/* group 2 */
extern	void (*insttable_G2Eb[])(BYTE *);
extern	void (*insttable_G2Ew[])(WORD *);
extern	void (*insttable_G2Ed[])(DWORD *);
extern	void (*insttable_G2EbCL[])(BYTE *, BYTE);
extern	void (*insttable_G2EwCL[])(WORD *, BYTE);
extern	void (*insttable_G2EdCL[])(DWORD *, BYTE);
extern	void (*insttable_G2Eb_ext[])(DWORD);
extern	void (*insttable_G2Ew_ext[])(DWORD);
extern	void (*insttable_G2Ed_ext[])(DWORD);
extern	void (*insttable_G2EbCL_ext[])(DWORD, BYTE);
extern	void (*insttable_G2EwCL_ext[])(DWORD, BYTE);
extern	void (*insttable_G2EdCL_ext[])(DWORD, BYTE);

/* group 3 */
extern	void (*insttable_G3Eb[])(DWORD);
extern	void (*insttable_G3Ew[])(DWORD);
extern	void (*insttable_G3Ed[])(DWORD);

/* group 4 */
extern	void (*insttable_G4[])(DWORD);

/* group 5 */
extern	void (*insttable_G5Ew[])(DWORD);
extern	void (*insttable_G5Ed[])(DWORD);

/* group 6 */
extern	void (*insttable_G6[])(DWORD);

/* group 7 */
extern	void (*insttable_G7[])(DWORD);

/* group 8 */
extern	void (*insttable_G8EwIb[])(DWORD);
extern	void (*insttable_G8EdIb[])(DWORD);

/* group 9 */
extern	void (*insttable_G9[])(DWORD);

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_INST_TABLE_H__ */
