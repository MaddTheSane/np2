/*	$Id: cpu.c,v 1.2 2003/12/22 18:00:31 monaka Exp $	*/

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

#if defined(IA32_PROFILE_INSTRUCTION)
UINT32	inst_1byte_count[2][256];
UINT32	inst_2byte_count[2][256];
UINT32	ea16_count[24];
UINT32	ea32_count[24];
UINT32	sib0_count[256];
UINT32	sib1_count[256];
UINT32	sib2_count[256];

static const char *ea16_str[24] = {
	"BX + SI", "BX + SI + DISP8", "BX + SI + DISP16",
	"BX + DI", "BX + DI + DISP8", "BX + DI + DISP16",
	"BP + SI", "BP + SI + DISP8", "BP + SI + DISP16",
	"BP + DI", "BP + DI + DISP8", "BP + DI + DISP16",
	"SI",      "SI + DISP8",      "SI + DISP16",
	"DI",      "DI + DISP8",      "DI + DISP16",
	"DISP16",  "BP + DISP8",      "BP + DISP16",
	"BX",      "BX + DISP8",      "BX + DISP16",
};

static const char *ea32_str[24] = {
	"EAX", "ECX", "EDX", "EBX", "SIB", "DISP32", "ESI", "EDI",
	"EAX + DISP8",  "ECX + DISP8",  "EDX + DISP8",  "EBX + DISP8",
	"SIB + DISP8",  "EBP + DISP8",  "ESI + DISP8",  "EDI + DISP8",
	"EAX + DISP32", "ECX + DISP32", "EDX + DISP32", "EBX + DISP32",
	"SIB + DISP32", "EBP + DISP32", "ESI + DISP32", "EDI + DISP32",
};

static const char *sib0_base_str[8] = {
	"EAX", "ECX", "EDX", "EBX", "ESP", "DISP32", "ESI", "EDI",
};

static const char *sib1_base_str[8] = {
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI",
};

static const char *sib_index_str[8] = {
	"EAX", "ECX", "EDX", "EBX", "", "EBP", "ESI", "EDI",
};

static const char *sib_scale_str[4] = {
	"", "2", "4", "8",
};

void
clear_profile_inst(void)
{

	memset(inst_1byte_count, 0, sizeof(inst_1byte_count));
	memset(inst_2byte_count, 0, sizeof(inst_2byte_count));
	memset(ea16_count, 0, sizeof(ea16_count));
	memset(ea32_count, 0, sizeof(ea32_count));
	memset(sib0_count, 0, sizeof(sib0_count));
	memset(sib1_count, 0, sizeof(sib1_count));
	memset(sib2_count, 0, sizeof(sib2_count));
}

void
show_profile_inst(void)
{
	int i;

	printf("instruction (16bit)\n");
	for (i = 0; i < 256; i++) {
		if (inst_1byte_count[0][i] != 0) {
			printf("0x%02x: %d\n", i, inst_1byte_count[0][i]);
		}
	}
	for (i = 0; i < 256; i++) {
		if (inst_2byte_count[0][i] != 0) {
			printf("0x0f%02x: %d\n", i, inst_2byte_count[0][i]);
		}
	}

	printf("instruction (32bit)\n");
	for (i = 0; i < 256; i++) {
		if (inst_1byte_count[1][i] != 0) {
			printf("0x%02x: %d\n", i, inst_1byte_count[1][i]);
		}
	}
	for (i = 0; i < 256; i++) {
		if (inst_2byte_count[1][i] != 0) {
			printf("0x0f%02x: %d\n", i, inst_2byte_count[1][i]);
		}
	}
}

void
show_profile_ea(void)
{
	char buf[80];
	char tmp[80];
	int i;
	int t;

	printf("EA16\n");
	for (i = 0; i < NELEMENTS(ea16_count); i++) {
		if (ea16_count[i] != 0) {
			printf("%s: %d\n", ea16_str[i], ea16_count[i]);
		}
	}
	printf("EA32\n");
	for (i = 0; i < NELEMENTS(ea32_count); i++) {
		if (ea32_count[i] != 0) {
			printf("%s: %d\n", ea32_str[i], ea32_count[i]);
		}
	}
	printf("SIB0\n");
	for (i = 0; i < NELEMENTS(sib0_count); i++) {
		if (sib0_count[i] != 0) {
			sprintf(tmp, "%s", sib0_base_str[i & 7]);
			strcpy(buf, tmp);
			t = (i >> 3) & 7;
			if (t != 4) {
				sprintf(tmp, " + %s", sib_index_str[t]);
				strcat(buf, tmp);
			}
			t = (i >> 6) & 3;
			if (t != 0) {
				sprintf(tmp, " * %s", sib_scale_str[t]);
				strcat(buf, tmp);
			}
			printf("%s: %d\n", buf, sib0_count[i]);
		}
	}
	printf("SIB1\n");
	for (i = 0; i < NELEMENTS(sib1_count); i++) {
		if (sib1_count[i] != 0) {
			sprintf(tmp, "%s", sib1_base_str[i & 7]);
			strcpy(buf, tmp);
			t = (i >> 3) & 7;
			if (t != 4) {
				sprintf(tmp, " + %s", sib_index_str[t]);
				strcat(buf, tmp);
			}
			t = (i >> 6) & 3;
			if (t != 0) {
				sprintf(tmp, " * %s", sib_scale_str[t]);
				strcat(buf, tmp);
			}
			printf("%s + DISP8: %d\n", buf, sib1_count[i]);
		}
	}
	printf("SIB2\n");
	for (i = 0; i < NELEMENTS(sib2_count); i++) {
		if (sib2_count[i] != 0) {
			sprintf(tmp, "%s", sib1_base_str[i & 7]);
			strcpy(buf, tmp);
			t = (i >> 3) & 7;
			if (t != 4) {
				sprintf(tmp, " + %s", sib_index_str[t]);
				strcat(buf, tmp);
			}
			t = (i >> 6) & 3;
			if (t != 0) {
				sprintf(tmp, " * %s", sib_scale_str[t]);
				strcat(buf, tmp);
			}
			printf("%s + DISP32: %d\n", buf, sib2_count[i]);
		}
	}
}
#endif

#define	MAX_PREFIX	8

jmp_buf exec_1step_jmpbuf;

void
exec_1step(void)
{
	BYTE op;
	BYTE prefix;

	CPU_PREV_EIP = CPU_EIP;

	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

	for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
		GET_PCBYTE(op);

		/* prefix */
		if (insttable_info[op] & INST_PREFIX) {
			PROFILE_INC_INST_1BYTE(op);
			(*insttable_1byte[0][op])();
			continue;
		}
		break;
	}
	if (prefix == MAX_PREFIX) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	PROFILE_INC_INST_1BYTE(op);

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
