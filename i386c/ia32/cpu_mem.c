/*	$Id: cpu_mem.c,v 1.14 2004/03/12 13:34:08 monaka Exp $	*/

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


/*
 * memory access check
 */
void
cpu_memoryread_check(descriptor_t *sd, UINT32 offset, UINT length, int e)
{
	UINT32 uplimit;

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			VERBOSE(("cpu_memoryread_check: invalid"));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			VERBOSE(("cpu_memoryread_check: not present"));
			EXCEPTION(e, 0);
		}
	}

	switch (sd->type) {
	case 0:	 case 1:	/* ro */
	case 2:  case 3:	/* rw */
	case 10: case 11:	/* rx */
	case 14: case 15:	/* rxc */
		if (offset > sd->u.seg.limit - length + 1) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) > sd->u.seg.limit(%08x) - length(%08x) + 1", offset, sd->u.seg.limit, length));
			EXCEPTION(e, 0);
		}
		if (length - 1 > sd->u.seg.limit) {
			VERBOSE(("cpu_memoryread_check: length(%08x) - 1 > sd->u.seg.limit(%08x)", length, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		break;

	case 4:  case 5:	/* ro (expand down) */
	case 6:  case 7:	/* rw (expand down) */
		uplimit = sd->d ? 0xffffffff : 0x0000ffff;
		if (offset <= sd->u.seg.limit) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) <= sd->u.seg.limit(%08x)", offset, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		if (offset > uplimit) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) > uplimit(%08x)", offset, uplimit));
			EXCEPTION(e, 0);
		}
		if (uplimit - offset < length - 1) {
			VERBOSE(("cpu_memoryread_check: uplimit(%08x) - offset(%08x) < length(%08x) - 1", uplimit, offset, length));
			EXCEPTION(e, 0);
		}
		break;

	default:
		VERBOSE(("cpu_memoryread_check: invalid type (type = %d)", sd->type));
		EXCEPTION(e, 0);
		break;
	}
	sd->flag |= CPU_DESC_FLAG_READABLE;
}

void
cpu_memorywrite_check(descriptor_t *sd, UINT32 offset, UINT length, int e)
{
	UINT32 uplimit;

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			VERBOSE(("cpu_memorywrite_check: invalid"));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			VERBOSE(("cpu_memorywrite_check: not present"));
			EXCEPTION(e, 0);
		}

		if (!sd->s) {
			VERBOSE(("cpu_memorywrite_check: system segment"));
			EXCEPTION(e, 0);
		}
	}

	switch (sd->type) {
	case 2: case 3:	/* rw */
		if (offset > sd->u.seg.limit - length + 1) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) > sd->u.seg.limit(%08x) - length(%08x) + 1", offset, sd->u.seg.limit, length));
			EXCEPTION(e, 0);
		}
		if (length - 1 > sd->u.seg.limit) {
			VERBOSE(("cpu_memorywrite_check: length(%08x) - 1 > sd->u.seg.limit(%08x)", length, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		break;

	case 6: case 7:	/* rw (expand down) */
		uplimit = sd->d ? 0xffffffff : 0x0000ffff;
		if (offset <= sd->u.seg.limit) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) <= sd->u.seg.limit(%08x)", offset, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		if (offset > uplimit) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) > uplimit(%08x)", offset, uplimit));
			EXCEPTION(e, 0);
		}
		if (uplimit - offset < length - 1) {
			VERBOSE(("cpu_memorywrite_check: uplimit(%08x) - offset(%08x) < length(%08x) - 1", uplimit, offset, length));
			EXCEPTION(e, 0);
		}
		break;

	default:
		VERBOSE(("cpu_memorywrite_check: invalid type (type = %d)", sd->type));
		EXCEPTION(e, 0);
		break;
	}
	sd->flag |= CPU_DESC_FLAG_WRITABLE;
}

void
cpu_stack_push_check(UINT16 s, descriptor_t *sd, UINT32 esp, UINT length)
{
	UINT32 limit;

	if (CPU_STAT_PM) {
		if (!sd->valid || !sd->p) {
			VERBOSE(("cpu_stack_push_check: valid = %d, present = %d", sd->valid, sd->p));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}
		if (!sd->s || sd->u.seg.c || !sd->u.seg.wr) {
			VERBOSE(("cpu_stack_push_check: s = %d, c = %d, wr", sd->s, sd->u.seg.c, sd->u.seg.wr));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}

		if (!sd->d) {
			limit = 0xffff;
		} else {
			limit = 0xffffffff;
		}
		if (sd->u.seg.ec) {
			/* expand-down stack */
			if ((esp == 0)
			 || (esp < length)
			 || (esp - length <= sd->u.seg.limit)
			 || (esp > limit)) {
				VERBOSE(("cpu_stack_push_check: expand-down, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		} else {
			/* expand-up stack */
			if (esp == 0) {
				if ((sd->d && (sd->u.seg.segend != 0xffffffff))
				 || (!sd->d && (sd->u.seg.segend != 0xffff))) {
					VERBOSE(("cpu_stack_push_check: expand-up, esp = %08x, length = %08x", esp, length));
					VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
					VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
					EXCEPTION(SS_EXCEPTION, s & 0xfffc);
				}
			} else {
				if ((esp < length)
				 || (esp - 1 > sd->u.seg.limit)) {
					VERBOSE(("cpu_stack_push_check: expand-up, esp = %08x, length = %08x", esp, length));
					VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
					VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
					EXCEPTION(SS_EXCEPTION, s & 0xfffc);
				}
			}
		}
	}
}

void
cpu_stack_pop_check(UINT16 s, descriptor_t *sd, UINT32 esp, UINT length)
{
	UINT32 limit;

	if (CPU_STAT_PM) {
		if (!sd->valid || !sd->p) {
			VERBOSE(("cpu_stack_pop_check: valid = %d, present = %d", sd->valid, sd->p));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}
		if (!sd->s || sd->u.seg.c || !sd->u.seg.wr) {
			VERBOSE(("cpu_stack_pop_check: s = %d, c = %d, wr", sd->s, sd->u.seg.c, sd->u.seg.wr));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}

		if (!sd->d) {
			limit = 0xffff;
		} else {
			limit = 0xffffffff;
		}
		if (sd->u.seg.ec) {
			/* expand-down stack */
			if ((esp == limit)
			 || ((limit - esp) + 1 < length)) {
				VERBOSE(("cpu_stack_pop_check: expand-up, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_pop_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_pop_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		} else {
			/* expand-up stack */
			if ((esp == limit)
			 || (sd->u.seg.segend == 0)
			 || (esp > sd->u.seg.limit)
			 || ((sd->u.seg.limit - esp) + 1 < length)) {
				VERBOSE(("cpu_stack_pop_check: expand-up, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_pop_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_pop_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		}
	}
}


#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
/*
 * code prefetch
 */
#define	CPU_PREFETCHQ_MASK	(CPU_PREFETCH_QUEUE_LENGTH - 1)

INLINE static MEMCALL void
cpu_prefetch(UINT32 address)
{
	UINT offset = address & CPU_PREFETCHQ_MASK;
	UINT length = CPU_PREFETCH_QUEUE_LENGTH - offset;

	cpu_memory_access_la_region(address, length, CPU_PAGE_READ_CODE, CPU_STAT_USER_MODE, CPU_PREFETCHQ + offset);
	CPU_PREFETCHQ_REMAIN = (SINT8)length;
}

INLINE static MEMCALL UINT8
cpu_prefetchq(UINT32 address)
{
	UINT8 v;

	CPU_PREFETCHQ_REMAIN--;
	v = CPU_PREFETCHQ[address & CPU_PREFETCHQ_MASK];
	return v;
}

INLINE static MEMCALL UINT16
cpu_prefetchq_w(UINT32 address)
{
	BYTE *p;
	UINT16 v;

	CPU_PREFETCHQ_REMAIN -= 2;
	p = CPU_PREFETCHQ + (address & CPU_PREFETCHQ_MASK);
	v = LOADINTELWORD(p);
	return v;
}

INLINE static MEMCALL UINT32
cpu_prefetchq_3(UINT32 address)
{
	BYTE *p;
	UINT32 v;

	CPU_PREFETCHQ_REMAIN -= 3;
	p = CPU_PREFETCHQ + (address & CPU_PREFETCHQ_MASK);
	v = LOADINTELWORD(p);
	v += ((UINT32)p[2]) << 16;
	return v;
}

INLINE static MEMCALL UINT32
cpu_prefetchq_d(UINT32 address)
{
	BYTE *p;
	UINT32 v;

	CPU_PREFETCHQ_REMAIN -= 4;
	p = CPU_PREFETCHQ + (address & CPU_PREFETCHQ_MASK);
	v = LOADINTELDWORD(p);
	return v;
}
#endif	/* IA32_SUPPORT_PREFETCH_QUEUE */

#if defined(IA32_SUPPORT_DEBUG_REGISTER)
INLINE static void
check_memory_break_point(UINT32 address, UINT length, UINT rw)
{
	int i;

	if (CPU_STAT_BP && !(CPU_EFLAG & RF_FLAG)) {
		for (i = 0; i < CPU_DEBUG_REG_INDEX_NUM; i++) {
			if ((CPU_STAT_BP & (1 << i))
			 && (CPU_DR7_GET_RW(i) & rw)

			 && ((address <= CPU_DR(i) && address + length > CPU_DR(i))
			  || (address > CPU_DR(i) && address < CPU_DR(i) + CPU_DR7_GET_LEN(i)))) {
				CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_B(i);
			}
		}
	}
}
#else
#define	check_memory_break_point(address, length, rw)
#endif

/*
 * code fetch
 */
UINT8 MEMCALL
cpu_codefetch(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit) {
		addr = sd->u.seg.segbase + offset;
#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
		if (CPU_PREFETCHQ_REMAIN <= 0) {
			cpu_prefetch(addr);
		}
		return cpu_prefetchq(addr);
#else	/* !IA32_SUPPORT_PREFETCH_QUEUE */
		if (!CPU_STAT_PM)
			return cpu_memoryread(addr);
		return cpu_lcmemoryread(addr);
#endif	/* IA32_SUPPORT_PREFETCH_QUEUE */
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

UINT16 MEMCALL
cpu_codefetch_w(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
	UINT16 v;
#endif

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit - 1) {
		addr = sd->u.seg.segbase + offset;
#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
		if (CPU_PREFETCHQ_REMAIN <= 0) {
			cpu_prefetch(addr);
		}
		if (CPU_PREFETCHQ_REMAIN >= 2) {
			return cpu_prefetchq_w(addr);
		}

		v = cpu_prefetchq(addr);
		addr++;
		cpu_prefetch(addr);
		v += (UINT16)cpu_prefetchq(addr) << 8;
		return v;
#else	/* !IA32_SUPPORT_PREFETCH_QUEUE */
		if (!CPU_STAT_PM)
			return cpu_memoryread_w(addr);
		return cpu_lcmemoryread_w(addr);
#endif	/* IA32_SUPPORT_PREFETCH_QUEUE */
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

UINT32 MEMCALL
cpu_codefetch_d(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
	UINT32 v;
#endif

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit - 3) {
		addr = sd->u.seg.segbase + offset;
#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
		if (CPU_PREFETCHQ_REMAIN <= 0) {
			cpu_prefetch(addr);
		}
		if (CPU_PREFETCHQ_REMAIN >= 4) {
			return cpu_prefetchq_d(addr);
		} else {
			switch (CPU_PREFETCHQ_REMAIN) {
			case 1:
				v = cpu_prefetchq(addr);
				addr++;
				cpu_prefetch(addr);
				v += (UINT32)cpu_prefetchq_3(addr) << 8;
				break;

			case 2:
				v = cpu_prefetchq_w(addr);
				addr += 2;
				cpu_prefetch(addr);
				v += (UINT32)cpu_prefetchq_w(addr) << 16;
				break;

			case 3:
				v = cpu_prefetchq_3(addr);
				addr += 3;
				cpu_prefetch(addr);
				v += (UINT32)cpu_prefetchq(addr) << 24;
				break;
			}
			return v;
		}
#else	/* !IA32_SUPPORT_PREFETCH_QUEUE */
		if (!CPU_STAT_PM)
			return cpu_memoryread_d(addr);
		return cpu_lcmemoryread_d(addr);
#endif	/* IA32_SUPPORT_PREFETCH_QUEUE */
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}


/*
 * virtual address -> linear address
 */
UINT8 MEMCALL
cpu_vmemoryread(int idx, UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit)
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 1, CPU_DR7_RW_RO);
	if (!CPU_STAT_PM)
		return cpu_memoryread(addr);
	return cpu_lmemoryread(addr, CPU_STAT_USER_MODE);

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemoryread: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

UINT16 MEMCALL
cpu_vmemoryread_w(int idx, UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - 1 <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - 1)
				goto range_failure;
			break;
		}
	} 
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 2, CPU_DR7_RW_RO);
	if (!CPU_STAT_PM)
		return cpu_memoryread_w(addr);
	return cpu_lmemoryread_w(addr, CPU_STAT_USER_MODE);

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemoryread_w: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

UINT32 MEMCALL
cpu_vmemoryread_d(int idx, UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - 3 <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - 3)
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 4, CPU_DR7_RW_RO);
	if (!CPU_STAT_PM)
		return cpu_memoryread_d(addr);
	return cpu_lmemoryread_d(addr, CPU_STAT_USER_MODE);

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemoryread_d: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

/* vaddr memory write */
void MEMCALL
cpu_vmemorywrite(int idx, UINT32 offset, UINT8 val)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit)
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 1, CPU_DR7_RW_RW);
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite(addr, val, CPU_STAT_USER_MODE);
	}
	return;

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemorywrite: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
}

void MEMCALL
cpu_vmemorywrite_w(int idx, UINT32 offset, UINT16 val)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - 1 <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - 1)
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 2, CPU_DR7_RW_RW);
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite_w(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite_w(addr, val, CPU_STAT_USER_MODE);
	}
	return;

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemorywrite_w: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
}

void MEMCALL
cpu_vmemorywrite_d(int idx, UINT32 offset, UINT32 val)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - 3 <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - 3)
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 4, CPU_DR7_RW_RW);
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite_d(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite_d(addr, val, CPU_STAT_USER_MODE);
	}
	return;

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemorywrite_d: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
}
