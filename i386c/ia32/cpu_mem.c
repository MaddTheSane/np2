/*	$Id: cpu_mem.c,v 1.7 2004/01/25 07:53:08 yui Exp $	*/

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

// static BYTE *cpumem = 0;
// static DWORD extmem_size = 0;

#define	cpumem		i386core.e.ext
#define	extmem_size	i386core.e.extsize


/*
 * initialize 1MB-16MB memory
 */

int
init_cpumem(UINT8 usemem)
{
	UINT32	size;

	size = usemem << 20;
	if (size >= (LOWMEM - 0x100000)) {
		size -= (LOWMEM - 0x100000);
	}
	else {
		size = 0;
	}
	if (extmem_size != size) {
		if (cpumem) {
			free(cpumem);
			cpumem = 0;
		}
		if (size) {
			cpumem = (BYTE *)malloc(size);
			if (cpumem == NULL) {
				size = 0;
			}
		}
		extmem_size = size;
	}
	return SUCCESS;
}


/*
 * memory access check
 */
void
cpu_memoryread_check(descriptor_t* sd, DWORD offset, DWORD length, int e)
{
	DWORD uplimit;

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
	}
	sd->flag |= CPU_DESC_FLAG_READABLE;
}

void
cpu_memorywrite_check(descriptor_t* sd, DWORD offset, DWORD length, int e)
{
	DWORD uplimit;

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
	}
	sd->flag |= CPU_DESC_FLAG_WRITABLE;
}

BOOL
cpu_stack_push_check(descriptor_t* sdp, DWORD esp, DWORD length)
{
	DWORD limit;

	if (!CPU_STAT_PM)
		return TRUE;

	if (!sdp->valid || !sdp->p)
		return FALSE;
	if (!sdp->s || sdp->u.seg.c || !sdp->u.seg.wr)
		return FALSE;

	if (!sdp->d) {
		esp &= 0xffff;
		limit = 0xffff;
	} else {
		limit = 0xffffffff;
	}
	if (sdp->u.seg.ec) {
		/* expand-down stack */
		if ((esp == 0)
		 || (esp < length)
		 || (esp - length <= sdp->u.seg.limit)
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
			 || (esp - 1 > sdp->u.seg.limit))
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
	if (!sdp->s || sdp->u.seg.c || !sdp->u.seg.wr)
		return FALSE;

	if (!sdp->d) {
		esp &= 0xffff;
		limit = 0xffff;
	} else {
		limit = 0xffffffff;
	}
	if (sdp->u.seg.ec) {
		/* expand-down stack */
		if ((esp == limit)
		 || ((limit - esp) + 1 < length))
			return FALSE;
	} else {
		/* expand-up stack */
		if ((esp == limit)
		 || (sdp->u.seg.segend == 0)
		 || (esp > sdp->u.seg.limit)
		 || ((sdp->u.seg.limit - esp) + 1 < length))
			return FALSE;
	}
	return TRUE;
}


#define	AS32_NOCHECK
#undef	OVERRUN_CHECK

#if defined(OVERRUN_CHECK)
#define	OVERRUN_EXCEPTION()	EXCEPTION(GP_EXCEPTION, 0)
#else
#define	OVERRUN_EXCEPTION()
#endif

/*
 * code fetch
 */
BYTE MEMCALL
cpu_codefetch(DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (offset <= sd->u.seg.limit) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + offset;
		if (!CPU_STAT_PM)
			return cpu_memoryread(addr);
		return cpu_lcmemoryread(addr);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

WORD MEMCALL
cpu_codefetch_w(DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (offset <= sd->u.seg.limit - 1) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + offset;
		if (!CPU_STAT_PM)
			return cpu_memoryread_w(addr);
		return cpu_lcmemoryread_w(addr);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

DWORD MEMCALL
cpu_codefetch_d(DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (offset <= sd->u.seg.limit - 3) {
		addr = CPU_STAT_SREGBASE(CPU_CS_INDEX) + offset;
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
cpu_vmemoryread(int idx, DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	}
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM)
		return cpu_memoryread(addr);
	return cpu_lmemoryread(addr);

err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

WORD MEMCALL
cpu_vmemoryread_w(int idx, DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - 1 <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit - 1) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	} 
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM)
		return cpu_memoryread_w(addr);
	return cpu_lmemoryread_w(addr);

err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

DWORD MEMCALL
cpu_vmemoryread_d(int idx, DWORD offset)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - 3 <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit - 3) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	}
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM)
		return cpu_memoryread_d(addr);
	return cpu_lmemoryread_d(addr);

err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

/* vaddr memory write */
void MEMCALL
cpu_vmemorywrite(int idx, DWORD offset, BYTE val)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 1,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	}
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite(addr, val);
	}
	return;

err:
	EXCEPTION(exc, 0);
}

void MEMCALL
cpu_vmemorywrite_w(int idx, DWORD offset, WORD val)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 2,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - 1 <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit - 1) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	}
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite_w(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite_w(addr, val);
	}
	return;

err:
	EXCEPTION(exc, 0);
}

void MEMCALL
cpu_vmemorywrite_d(int idx, DWORD offset, DWORD val)
{
	descriptor_t *sd;
	DWORD addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

#ifndef	AS32_NOCHECK
	if (!CPU_INST_AS32)
		offset &= 0xffff;
#endif
	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 4,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - 3 <= sd->u.seg.limit) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;

		default:
			if (offset > sd->u.seg.limit - 3) {
				if (idx == CPU_SS_INDEX)
					exc = SS_EXCEPTION;
				else
					exc = GP_EXCEPTION;
				goto err;
			}
			break;
		}
	}
	addr = CPU_STAT_SREGBASE(idx) + offset;
	if (!CPU_STAT_PM) {
		/* real mode */
		cpu_memorywrite_d(addr, val);
	} else {
		/* protected mode */
		cpu_lmemorywrite_d(addr, val);
	}
	return;

err:
	EXCEPTION(exc, 0);
}

/*
 * physical address memory function
 */
void MEMCALL
cpu_memorywrite_d(DWORD address, DWORD value)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;
	DWORD diff;
	DWORD off;

	if (adr < LOWMEM - 3) {
		__i286_memorywrite_d(adr, value);
	} else if (adr < LOWMEM) {
		diff = LOWMEM - adr;

		switch (diff) {
		default:
			ia32_panic("cpu_memorywrite_d: diff(%d)", diff);
			break;

		case 3:
			__i286_memorywrite_w(adr, value & 0xffff);
			value >>= 16;
			__i286_memorywrite(adr + 2, value & 0xff);
			value >>= 8;
			break;

		case 2:
			__i286_memorywrite_w(adr, value & 0xffff);
			value >>= 16;
			break;

		case 1:
			__i286_memorywrite(adr, value & 0xff);
			value >>= 8;
			break;
		}

		if (extmem_size > 0) {
			off = 0;

			switch (4 - diff) {
			case 3:
				cpumem[off++] = value & 0xff;
				value >>= 8;
				if (off >= extmem_size) {
					OVERRUN_EXCEPTION();
					break;
				}
				/*FALLTHROUGH*/
			case 2:
				cpumem[off++] = value & 0xff;
				if (off >= extmem_size) {
					OVERRUN_EXCEPTION();
					break;
				}
				value >>= 8;
				/*FALLTHROUGH*/
			case 1:
				cpumem[off] = value & 0xff;
				break;
			}
		} else {
			OVERRUN_EXCEPTION();
		}
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size - 3) {
			STOREINTELDWORD(cpumem + adr, value);
		} else if (adr < extmem_size) {
			diff = extmem_size - adr;
			value >>= (8 * (4 - diff));

			switch (diff) {
			default:
				ia32_panic("cpu_memorywrite_d: diff(%d)", diff);
				break;

			case 3:
				cpumem[extmem_size - 3] = value & 0xff;
				value >>= 8;
				/*FALLTHROUGH*/
			case 2:
				cpumem[extmem_size - 2] = value & 0xff;
				value >>= 8;
				/*FALLTHROUGH*/
			case 1:
				cpumem[extmem_size - 1] = value & 0xff;
				break;
			}
			OVERRUN_EXCEPTION();
		} else {
			OVERRUN_EXCEPTION();
		}
	} else {
		OVERRUN_EXCEPTION();
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
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size - 1) {
			STOREINTELWORD(cpumem + adr, value);
		} else if (adr == extmem_size - 1) {
			cpumem[adr] = value & 0xff;
			OVERRUN_EXCEPTION();
		} else {
			OVERRUN_EXCEPTION();
		}
	} else {
		OVERRUN_EXCEPTION();
	}
}

void MEMCALL
cpu_memorywrite(DWORD address, BYTE value)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;

	if (adr < LOWMEM) {
		__i286_memorywrite(adr, value);
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size) {
			cpumem[adr] = value;
		} else {
			OVERRUN_EXCEPTION();
		}
	} else {
		OVERRUN_EXCEPTION();
	}
}

DWORD MEMCALL
cpu_memoryread_d(DWORD address)
{
	DWORD adr = address & CPU_STAT_ADRSMASK;
	DWORD val;
	DWORD diff;

	if (adr < LOWMEM - 3) {
		val = __i286_memoryread_d(adr);
	} else if (adr < LOWMEM) {
		diff = LOWMEM - adr;

		switch (diff) {
		default:
			ia32_panic("cpu_memoryread_d: diff(%d)", diff);
			val = 0;	/* compiler happy */
			break;

		case 3:
			val = __i286_memoryread_w(adr);
			val |= __i286_memoryread(adr + 2);
			if (extmem_size > 0) {
				val |= cpumem[0] << 24;
			} else {
				val |= 0xff000000;
				OVERRUN_EXCEPTION();
			}
			break;

		case 2:
			val = __i286_memoryread_w(adr);
			if (extmem_size > 1) {
				val |= (DWORD)LOADINTELWORD(cpumem) << 16;
			} else {
				val |= 0xffff0000;
				OVERRUN_EXCEPTION();
			}
			break;

		case 1:
			val = __i286_memoryread(adr);
			if (extmem_size > 2) {
				val |= (DWORD)LOADINTELWORD(cpumem) << 8;
				val |= (DWORD)cpumem[2] << 24;
			} else {
				val |= 0xffffff00;
				OVERRUN_EXCEPTION();
			}
			break;
		}
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size - 3) {
			val = LOADINTELDWORD(cpumem + adr);
		} else if (adr < extmem_size) {
			diff = extmem_size - adr;
			val = 0;

			switch (diff) {
			default:
				ia32_panic("cpu_memoryread_d: diff(%d)", diff);
				break;

			case 3:
				val |= cpumem[extmem_size - 3];
				/*FALLTHROUGH*/
			case 2:
				val |= (cpumem[extmem_size - 2] << (8 * (diff - 2)));
				/*FALLTHROUGH*/
			case 1:
				val |= (cpumem[extmem_size - 1] << (8 * (diff - 1)));
				break;
			}
			val |= ((DWORD)-1) << (8 * diff);
			OVERRUN_EXCEPTION();
		} else {
			val = (DWORD)-1;
			OVERRUN_EXCEPTION();
		}
	} else {
		val = (DWORD)-1;
		OVERRUN_EXCEPTION();
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
		val = __i286_memoryread(adr);
		val |= (WORD)cpumem[0] << 8;
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size - 1) {
			val = LOADINTELWORD(cpumem + adr);
		} else if (adr == extmem_size - 1) {
			val = 0xff00 | cpumem[adr];
			OVERRUN_EXCEPTION();
		} else {
			val = (WORD)-1;
			OVERRUN_EXCEPTION();
		}
	} else {
		val = (WORD)-1;
		OVERRUN_EXCEPTION();
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
	} else if (extmem_size > 0) {
		adr -= LOWMEM;
		if (adr < extmem_size) {
			val = cpumem[adr];
		} else {
			val = (BYTE)-1;
			OVERRUN_EXCEPTION();
		}
	} else {
		val = (BYTE)-1;
		OVERRUN_EXCEPTION();
	}
	return val;
}
