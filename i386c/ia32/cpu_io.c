/*	$Id: cpu_io.c,v 1.2 2004/02/04 13:24:35 monaka Exp $	*/

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

#include "compiler.h"
#include "cpu.h"
#include "pccore.h"
#include "iocore.h"


static void
check_io(WORD port, int len)
{
	WORD off;
	BYTE bit;
	BYTE map;

	/* そもそも I/O 許可マップが無い場合 */
	if (CPU_STAT_IOLIMIT == 0) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	off = port / 8;
	bit = 1 << (port % 8);
	for (; len > 0; ++off, bit = 0x01) {
		/* I/O 許可マップはビット単位なので */
		if (off * 8 >= CPU_STAT_IOLIMIT) {
			EXCEPTION(GP_EXCEPTION, 0);
		}

		map = cpu_kmemoryread(CPU_STAT_IOADDR + off);
		for (; (len > 0) && (bit != 0x00); bit <<= 1, --len) {
			if (map & bit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
		}
	}
}

BYTE
cpu_in(WORD port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 1);
	}
	return iocore_inp8(port);
}

WORD
cpu_in_w(WORD port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 2);
	}
	return iocore_inp16(port);
}

DWORD
cpu_in_d(WORD port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 4);
	}
	return iocore_inp32(port);
}

void
cpu_out(WORD port, BYTE data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 1);
	}
	iocore_out8(port, data);
}

void
cpu_out_w(WORD port, WORD data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 2);
	}
	iocore_out16(port, data);
}

void
cpu_out_d(WORD port, DWORD data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 4);
	}
	iocore_out32(port, data);
}
