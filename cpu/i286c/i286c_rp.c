#include	"compiler.h"
#include	"cpucore.h"
#include	"i286c.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"i286c.mcr"

/* ins */

I286EXT i286c_rep_insb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		do
		{
			REG8 dat = iocore_inp8(I286_DX);
			i286_memorywrite(I286_DI + I286_ES_BASE, dat);
			I286_DI += stp;
			I286_WORKCLOCK(4);
		} while (--I286_CX);
	}
}

I286EXT i286c_rep_insw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		do
		{
			REG16 dat = iocore_inp16(I286_DX);
			i286_memorywrite_w(I286_DI + I286_ES_BASE, dat);
			I286_DI += stp;
			I286_WORKCLOCK(4);
		} while (--I286_CX);
	}
}

/* outs */

I286EXT i286c_rep_outsb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		do
		{
			REG8 dat = i286_memoryread(I286_SI + I286_DS_FIX);
			I286_SI += stp;
			iocore_out8(I286_DX, (UINT8)dat);
			I286_WORKCLOCK(4);
		} while (--I286_CX);
	}
}

I286EXT i286c_rep_outsw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		do
		{
			REG16 dat = i286_memoryread_w(I286_SI + I286_DS_FIX);
			I286_SI += stp;
			iocore_out16(I286_DX, (UINT16)dat);
			I286_WORKCLOCK(4);
		} while (--I286_CX);
	}
}

/* movs */

#if 1
I286EXT i286c_rep_movsb(I286CORE *cpu)
{
	UINT16	r_cx;
	int		stp;
	UINT16	r_si;
	UINT16	r_di;

	I286_WORKCLOCK(5);
	r_cx = I286_CX;
	if (r_cx)
	{
		stp = STRING_DIR;
		r_si = I286_SI;
		r_di = I286_DI;
		while (TRUE /*CONSTCOND*/)
		{
			REG8 dat = i286_memoryread(I286_DS_FIX + r_si);
			i286_memorywrite(I286_ES_BASE + r_di, dat);
			r_si += stp;
			r_di += stp;
			I286_WORKCLOCK(4);
			r_cx--;
			if (!r_cx)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
		I286_CX = r_cx;
		I286_SI = r_si;
		I286_DI = r_di;
	}
}

I286EXT i286c_rep_movsw(I286CORE *cpu)
{
	UINT16	r_cx;
	int		stp;
	UINT16	r_si;
	UINT16	r_di;

	I286_WORKCLOCK(5);
	r_cx = I286_CX;
	if (r_cx)
	{
		stp = STRING_DIRx2;
		r_si = I286_SI;
		r_di = I286_DI;
		while (TRUE /*CONSTCOND*/)
		{
			REG16 dat = i286_memoryread_w(I286_DS_FIX + r_si);
			i286_memorywrite_w(I286_ES_BASE + r_di, dat);
			r_si += stp;
			r_di += stp;
			I286_WORKCLOCK(4);
			r_cx--;
			if (!r_cx)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
		I286_CX = r_cx;
		I286_SI = r_si;
		I286_DI = r_di;
	}
}
#else
I286EXT i286c_rep_movsb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		while (TRUE /*CONSTCOND*/)
		{
			REG8 dat = i286_memoryread(I286_SI + I286_DS_FIX);
			i286_memorywrite(I286_DI + I286_ES_BASE, dat);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(4);
			I286_CX--;
			if (!I286_CX)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}

I286EXT i286c_rep_movsw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		while (TRUE /*CONSTCOND*/)
		{
			REG16 dat = i286_memoryread_w(I286_SI + I286_DS_FIX);
			i286_memorywrite_w(I286_DI + I286_ES_BASE, dat);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(4);
			I286_CX--;
			if (!I286_CX)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}
#endif

/* lods */

I286EXT i286c_rep_lodsb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		while (TRUE /*CONSTCOND*/)
		{
			I286_AL = i286_memoryread(I286_SI + I286_DS_FIX);
			I286_SI += stp;
			I286_WORKCLOCK(4);
			I286_CX--;
			if (!I286_CX)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}

I286EXT i286c_rep_lodsw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		while (TRUE /*CONSTCOND*/)
		{
			I286_AX = i286_memoryread_w(I286_SI + I286_DS_FIX);
			I286_SI += stp;
			I286_WORKCLOCK(4);
		 	I286_CX--;
		 	if (!I286_CX)
		 	{
		 		break;
		 	}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}

/* stos */

I286EXT i286c_rep_stosb(I286CORE *cpu)
{
	I286_WORKCLOCK(4);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		while (TRUE /*CONSTCOND*/)
		{
			i286_memorywrite(I286_DI + I286_ES_BASE, I286_AL);
			I286_DI += stp;
			I286_WORKCLOCK(3);
			I286_CX--;
			if (!I286_CX)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}

I286EXT i286c_rep_stosw(I286CORE *cpu)
{
	I286_WORKCLOCK(4);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		while (TRUE /*CONSTCOND*/)
		{
			i286_memorywrite_w(I286_DI + I286_ES_BASE, I286_AX);
			I286_DI += stp;
			I286_WORKCLOCK(3);
			I286_CX--;
			if (!I286_CX)
			{
				break;
			}
			if (I286_REMCLOCK <= 0)
			{
				I286_IP -= I286_PREFIX + 1;
				break;
			}
		}
	}
}

/* cmps */

I286EXT i286c_repe_cmpsb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		do
		{
			UINT res;
			UINT dst = i286_memoryread(I286_SI + I286_DS_FIX);
			UINT src = i286_memoryread(I286_DI + I286_ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(9);
			SUBBYTE(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_cmpsb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		do
		{
			UINT res;
			UINT dst = i286_memoryread(I286_SI + I286_DS_FIX);
			UINT src = i286_memoryread(I286_DI + I286_ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(9);
			SUBBYTE(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

I286EXT i286c_repe_cmpsw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		do
		{
			UINT32 res;
			UINT32 dst = i286_memoryread_w(I286_SI + I286_DS_FIX);
			UINT32 src = i286_memoryread_w(I286_DI + I286_ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(9);
			SUBWORD(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_cmpsw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		do
		{
			UINT32 res;
			UINT32 dst = i286_memoryread_w(I286_SI + I286_DS_FIX);
			UINT32 src = i286_memoryread_w(I286_DI + I286_ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_WORKCLOCK(9);
			SUBWORD(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

/*  scas */

I286EXT i286c_repe_scasb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		UINT dst = I286_AL;
		do
		{
			UINT res;
			UINT src = i286_memoryread(I286_DI + I286_ES_BASE);
			I286_DI += stp;
			I286_WORKCLOCK(8);
			SUBBYTE(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_scasb(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIR;
		UINT dst = I286_AL;
		do
		{
			UINT res;
			UINT src = i286_memoryread(I286_DI + I286_ES_BASE);
			I286_DI += stp;
			I286_WORKCLOCK(8);
			SUBBYTE(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

I286EXT i286c_repe_scasw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		UINT32 dst = I286_AX;
		do
		{
			UINT32 res;
			UINT32 src = i286_memoryread_w(I286_DI + I286_ES_BASE);
			I286_DI += stp;
			I286_WORKCLOCK(8);
			SUBWORD(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_scasw(I286CORE *cpu)
{
	I286_WORKCLOCK(5);
	if (I286_CX)
	{
		int stp = STRING_DIRx2;
		UINT32 dst = I286_AX;
		do
		{
			UINT32 res;
			UINT32 src = i286_memoryread_w(I286_DI + I286_ES_BASE);
			I286_DI += stp;
			I286_WORKCLOCK(8);
			SUBWORD(res, dst, src)
			I286_CX--;
		} while ((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

