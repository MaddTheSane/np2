/*	$Id: interface.c,v 1.2 2003/12/08 02:09:17 yui Exp $	*/

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
#include "dmap.h"
#include "bios.h"


void
ia32reset(void)
{
//	cpu_init();
#ifdef USE_FPU		// ->ia32_init
//	fpu_init();
#endif

	CPU_SET_SEGREG(CPU_CS_INDEX, 0x1fc0);
	CPU_ADRSMASK = 0xfffff;
}

void
ia32(void)
{
	int rv;

#if defined(WIN32)
	rv = setjmp(exec_1step_jmpbuf);
#else
	rv = sigsetjmp(exec_1step_jmpbuf, 1);
#endif
	switch (rv) {
	case 0:
		break;
	
	default:
		CPU_EIP = CPU_PREV_EIP;
		break;
	}

	do {
		exec_1step();
	} while (CPU_REMCLOCK > 0);
}

void
ia32withtrap(void)
{
	int rv;

#if defined(WIN32)
	rv = setjmp(exec_1step_jmpbuf);
#else
	rv = sigsetjmp(exec_1step_jmpbuf, 1);
#endif
	switch (rv) {
	case 0:
		break;
	
	default:
		CPU_EIP = CPU_PREV_EIP;
		break;
	}


	do {
		exec_1step();
		if (CPU_TRAP) {
			ia32_interrupt(1);
		}
	} while (CPU_REMCLOCK > 0);
}

void
ia32withdma(void)
{
	int rv;

#if defined(WIN32)
	rv = setjmp(exec_1step_jmpbuf);
#else
	rv = sigsetjmp(exec_1step_jmpbuf, 1);
#endif
	switch (rv) {
	case 0:
		break;
	
	default:
		CPU_EIP = CPU_PREV_EIP;
		break;
	}


	do {
		exec_1step();
		dmap_i286();
	} while (CPU_REMCLOCK > 0);
}

void
ia32_step(void)
{
	int rv;

#if defined(WIN32)
	rv = setjmp(exec_1step_jmpbuf);
#else
	rv = sigsetjmp(exec_1step_jmpbuf, 1);
#endif
	switch (rv) {
	case 0:
		break;
	
	default:
		CPU_EIP = CPU_PREV_EIP;
		break;
	}


	do {
		exec_1step();
		if (CPU_TRAP) {
			ia32_interrupt(1);
		}
		dmap_i286();
	} while (CPU_REMCLOCK > 0);
}

void CPUCALL
ia32_interrupt(BYTE vect)
{

	INTERRUPT(vect, 0, 0, 0);
}

/*
 * error function
 */
void
ia32_panic(const char *str, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	strcat(buf, "\n");

	fprintf(stderr, buf);
	__ASSERT(0);
	exit(1);
}

void
ia32_warning(const char *str, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	strcat(buf, "\n");

	fprintf(stderr, buf);
}

void
ia32_printf(const char *str, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);

	fprintf(stderr, buf);
}

/*
 * bios call interface
 */
void
ia32_bioscall(void)
{

	/* XXX */
	if (!CPU_STAT_PM && !CPU_INST_OP32 && !CPU_INST_AS32) {
		DWORD adrs;
		WORD sreg;

		adrs = ((CPU_IP-1) & 0xffff) + CPU_STAT_SREGBASE(CPU_CS_INDEX);
		if ((adrs >= 0xf8000) && (adrs < 0x100000)) {
			biosfunc(adrs);
			sreg = CPU_ES;
			CPU_SET_SEGREG(CPU_ES_INDEX, sreg);
			sreg = CPU_CS;
			CPU_SET_SEGREG(CPU_CS_INDEX, sreg);
			sreg = CPU_SS;
			CPU_SET_SEGREG(CPU_SS_INDEX, sreg);
			sreg = CPU_DS;
			CPU_SET_SEGREG(CPU_DS_INDEX, sreg);
		}
	}
}
