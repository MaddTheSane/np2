/*	$Id: cpu.c,v 1.11 2004/02/13 14:52:35 monaka Exp $	*/

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

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"

#include "inst_table.h"


sigjmp_buf exec_1step_jmpbuf;

void
exec_1step(void)
{
	int prefix;
	BYTE op;

	CPU_PREV_EIP = CPU_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

	for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
		GET_PCBYTE(op);

		/* prefix */
		if (insttable_info[op] & INST_PREFIX) {
			(*insttable_1byte[0][op])();
			continue;
		}
		break;
	}
	if (prefix == MAX_PREFIX) {
		EXCEPTION(UD_EXCEPTION, 0);
	}

	/* normal / rep, but not use */
	if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		return;
	}

	/* rep */
	CPU_WORKCLOCK(5);
	if (!CPU_INST_AS32) {
		if (CPU_CX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX);
			} else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX && (CPU_FLAGL & Z_FLAG));
			} else {
				/* repne */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX && !(CPU_FLAGL & Z_FLAG));
			}
		}
	} else {
		if (CPU_ECX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX);
			} else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX && (CPU_FLAGL & Z_FLAG));
			} else {
				/* repne */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX && !(CPU_FLAGL & Z_FLAG));
			}
		}
	}
}
