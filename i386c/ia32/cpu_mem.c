/*	$Id: cpu_mem.c,v 1.4 2004/01/07 14:49:10 monaka Exp $	*/

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
#include "memory.h"

// あとで…
// extern DWORD cpumem_addrmask;		// -> CPU_STAT_ADRSMASK

BYTE *cpumem = 0;
DWORD extmem_size = 0;
BYTE protectmem_size = 0;

/*
 * initialize 1MB-16MB memory
 */

int
init_cpumem(BYTE usemem)
{
	DWORD size;

	if (usemem > 13)
		usemem = 13;
	size = usemem << 20;

	if (extmem_size != size - (LOWMEM - 0x100000)) {
		if (cpumem) {
			free(cpumem);
			cpumem = 0;
		}
		if (size <= LOWMEM - 0x100000) {
			extmem_size = 0;
			cpumem = 0;
		} else {
			extmem_size = size - (LOWMEM - 0x100000);
			cpumem = (BYTE *)malloc(extmem_size);
			if (cpumem == NULL) {
				protectmem_size = 0;
				return FAILURE;
			}
			memset(cpumem, 0, extmem_size);
		}
	}
	protectmem_size = usemem;
	return SUCCESS;
}


/*
 * memory access check
 */
void
cpu_memoryread_check(descriptor_t* sd, DWORD madr, DWORD length, int e)
{

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			EXCEPTION(e, 0);
		}

		switch (sd->type) {
		case 0:	 case 1:	/* ro */
		case 2:  case 3:	/* rw */
		case 10: case 11:	/* rx */
		case 14: case 15:	/* rxc */
			if ((madr > sd->u.seg.segend - length + 1)
			    || (length - 1 > sd->u.seg.limit)) {
				EXCEPTION(e, 0);
			}
			break;

		case 4:  case 5:	/* ro (expand down) */
		case 6:  case 7:	/* rw (expand down) */
		{
			DWORD uplimit = sd->d ? 0xffffffff : 0x0000ffff;
			if ((madr <= sd->u.seg.segend)
			    || (madr > uplimit)
			    || (uplimit - madr < length - 1)) {
				EXCEPTION(e, 0);
			}
		}
			break;

		default:
			EXCEPTION(e, 0);
			break;
		}
	}
	sd->flag |= CPU_DESC_READABLE;
}

void
cpu_memorywrite_check(descriptor_t* sd, DWORD madr, DWORD length, int e)
{

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			EXCEPTION(e, 0);
		}

		switch (sd->type) {
		case 2: case 3:	/* rw */
			if ((madr > sd->u.seg.segend - length + 1)
			    || (length - 1 > sd->u.seg.limit)) {
				EXCEPTION(e, 0);
			}
			break;

		case 6: case 7:	/* rw (expand down) */
		{
			DWORD uplimit = sd->d ? 0xffffffff : 0x0000ffff;
			if ((madr <= sd->u.seg.segend)
			    || (madr > uplimit)
			    || (uplimit - madr < length - 1)) {
				EXCEPTION(e, 0);
			}
		}
			break;

		default:
			EXCEPTION(e, 0);
			break;
		}
	}
	sd->flag |= CPU_DESC_WRITABLE;
}

BOOL
cpu_stack_push_check(descriptor_t* sdp, DWORD esp, DWORD length)
{

	if (!CPU_STAT_PM)
		return TRUE;

	if (!sdp->valid || !sdp->p)
		return FALSE;
#ifdef _DEBUG
	if (!sdp->s || sdp->u.seg.c || !sdp->u.seg.wr)
		return FALSE;
#endif

	if (!sdp->d)
		esp &= 0xffff;
	if (sdp->u.seg.ec) {
		DWORD limit = (sdp->d) ? 0xffffffff : 0xffff;
		if ((esp == 0)
		 || (esp < length)
		 || (esp - length <= sdp->u.seg.segend)
		 || (esp > limit))
			return FALSE;
	} else {
		/* expand-up stack */
		if (esp == 0) {
			if ((sdp->d && (sdp->u.seg.segend != 0xffffffff))
			 || (!sdp->d && (sdp->u.seg.segend != 0xffff)))
				return FALSE;
		} else {
			if ((esp < length)
			 || (esp - 1 > sdp->u.seg.segend))
				return FALSE;
		}
	}
	return TRUE;
}

BOOL
cpu_stack_pop_check(descriptor_t* sdp, DWORD esp, DWORD length)
{
	DWORD limit;

	if (!CPU_STAT_PM)
		return TRUE;

	if (!sdp->valid || !sdp->p)
		return FALSE;
#ifdef _DEBUG
	if (!sdp->s || sdp->u.seg.c || !sdp->u.seg.wr)
		return FALSE;
#endif

	if (!sdp->d) {
		esp &= 0xffff;
		limit = 0xffff;
	} else {
		limit = 0xffffffff;
	}
	if (sdp->u.seg.ec) {
		if ((esp == limit)
		 || ((limit  - esp) + 1 < length))
			return FALSE;
	} else {
		/* expand-up stack */
		if ((esp == limit)
		 || (sdp->u.seg.segend == 0)
		 || (esp > sdp->u.seg.segend)
		 || ((sdp->u.seg.segend - esp) + 1 < length))
			return FALSE;
	}
	return TRUE;
}


/*
 * code fetch
 */
BYTE MEMCALL
cpu_codefetch(DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (!CPU_INST_AS32)
		madr &= 0xffff;
	if (madr <= sd->u.seg.segend) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + madr;
		if (!CPU_STAT_PM)
			return cpu_memoryread(addr);
		return cpu_lcmemoryread(addr);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

WORD MEMCALL
cpu_codefetch_w(DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (!CPU_INST_AS32)
		madr &= 0xffff;
	if (madr <= sd->u.seg.segend - 1) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + madr;
		if (!CPU_STAT_PM)
			return cpu_memoryread_w(addr);
		return cpu_lcmemoryread_w(addr);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

DWORD MEMCALL
cpu_codefetch_d(DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (!CPU_INST_AS32)
		madr &= 0xffff;
	if (madr <= sd->u.seg.segend - 3) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + madr;
		if (!CPU_STAT_PM)
			return cpu_memoryread_d(addr);
		return cpu_lcmemoryread_d(addr);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}


/*
 * virtual address -> linear address
 */
BYTE MEMCALL
cpu_vmemoryread(int idx, DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_READABLE)
		    || (madr <= sd->u.seg.segend)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM)
				return cpu_memoryread(addr);
			return cpu_lmemoryread(addr);
		}
		cpu_memoryread_check(sd, madr, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

WORD MEMCALL
cpu_vmemoryread_w(int idx, DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_READABLE)
		    || (madr <= sd->u.seg.segend - 1)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM)
				return cpu_memoryread_w(addr);
			return cpu_lmemoryread_w(addr);
		}
		cpu_memoryread_check(sd, madr, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

DWORD MEMCALL
cpu_vmemoryread_d(int idx, DWORD madr)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_READABLE)
		    || (madr <= sd->u.seg.segend - 3)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM)
				return cpu_memoryread_d(addr);
			return cpu_lmemoryread_d(addr);
		}
		cpu_memoryread_check(sd, madr, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

/* vaddr memory write */
void MEMCALL
cpu_vmemorywrite(int idx, DWORD madr, BYTE val)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_WRITABLE)
		    || (madr <= sd->u.seg.segend)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM) {
				/* real mode */
				cpu_memorywrite(addr, val);
			} else {
				/* protected mode */
				cpu_lmemorywrite(addr, val);
			}
			return;
		}
		cpu_memorywrite_check(sd, madr, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

void MEMCALL
cpu_vmemorywrite_w(int idx, DWORD madr, WORD val)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_WRITABLE)
		    || (madr <= sd->u.seg.segend - 1)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM) {
				/* real mode */
				cpu_memorywrite_w(addr, val);
			} else {
				/* protected mode */
				cpu_lmemorywrite_w(addr, val);
			}
			return;
		}
		cpu_memorywrite_check(sd, madr, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

void MEMCALL
cpu_vmemorywrite_d(int idx, DWORD madr, DWORD val)
{
	descriptor_t *sd;
	DWORD addr;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!CPU_INST_AS32)
		madr &= 0xffff;
	for (;;) {
		if ((sd->flag & CPU_DESC_WRITABLE)
		    || (madr <= sd->u.seg.segend - 3)) {
			addr = CPU_STAT_SREGBASE(idx) + madr;
			if (!CPU_STAT_PM) {
				/* real mode */
				cpu_memorywrite_d(addr, val);
			} else {
				/* protected mode */
				cpu_lmemorywrite_d(addr, val);
			}
			return;
		}
		cpu_memorywrite_check(sd, madr, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	}
	/*NOTREACHED*/
}

/*
 * physical address memory function
 */
void MEMCALL
cpu_memorywrite_d(DWORD address, DWORD value)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;

	if (adr < LOWMEM - 3) {
		__i286_memorywrite_d(adr, value);
	} else if (adr < LOWMEM) {
		cpu_memorywrite_w(adr, value & 0xffff);
		cpu_memorywrite_w(adr + 2, (value >> 16) & 0xffff);
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size - 3) {
			STOREINTELDWORD(cpumem + adr, value);
		} else if (adr < extmem_size) {
			cpu_memorywrite_w(adr, value & 0xffff);
			cpu_memorywrite_w(adr + 2, (value >> 16) & 0xffff);
		}
	}
}

void MEMCALL
cpu_memorywrite_w(DWORD address, WORD value)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;

	if (adr < LOWMEM - 1) {
		__i286_memorywrite_w(adr, value);
	} else if (adr < LOWMEM) {
		__i286_memorywrite(adr, value & 0xff);
		cpumem[adr - (LOWMEM - 1)] = (value >> 8) & 0xff;
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size - 1) {
			STOREINTELWORD(cpumem + adr, value);
		} else if (adr == extmem_size - 1) {
			cpumem[adr] = value & 0xff;
		}
	}
}

void MEMCALL
cpu_memorywrite(DWORD address, BYTE value)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;

	if (adr < LOWMEM) {
		__i286_memorywrite(adr, value);
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size) {
			cpumem[adr] = value;
		}
	}
}

DWORD MEMCALL
cpu_memoryread_d(DWORD address)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;
	DWORD val;

	if (adr < LOWMEM - 3) {
		val = __i286_memoryread_d(adr);
	} else if (adr < LOWMEM) {
		val = cpu_memoryread_w(adr);
		val |= (DWORD)cpu_memoryread_w(adr + 2) << 16;
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size - 3) {
			val = LOADINTELDWORD(cpumem + adr);
		} else {
			val = cpu_memoryread_w(adr);
			val |= (DWORD)cpu_memoryread_w(adr + 2) << 16;
		}
	}
	return val;
}

WORD MEMCALL
cpu_memoryread_w(DWORD address)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;
	WORD val;

	if (adr < LOWMEM - 1) {
		val = __i286_memoryread_w(adr);
	} else if (adr < LOWMEM) {
		val = cpu_memoryread(adr);
		val |= (WORD)cpumem[adr - (LOWMEM - 1)] << 8;
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size - 1) {
			val = LOADINTELWORD(cpumem + adr);
		} else if (adr == extmem_size - 1) {
			val = 0xff00 | cpumem[adr];
		} else {
			val = (WORD)-1;
		}
	}
	return val;
}

BYTE MEMCALL
cpu_memoryread(DWORD address)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;
	BYTE val;

	if (adr < LOWMEM) {
		val = __i286_memoryread(adr);
	} else {
		adr -= LOWMEM;
		if (adr < extmem_size) {
			val = cpumem[adr];
		} else {
			val = (BYTE)-1;
		}
	}
	return val;
}
