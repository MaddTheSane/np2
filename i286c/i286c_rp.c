#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"i286c.mcr"


// ---------------------------------------------------------------------- ins

I286EXT i286c_rep_insb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			BYTE dat = iocore_inp8(I286_DX);
			i286_memorywrite(I286_DI + ES_BASE, dat);
			I286_DI += stp;
			I286_CLOCK(4)
		} while (--I286_CX);
	}
}

I286EXT i286c_rep_insw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			WORD dat = iocore_inp16(I286_DX);
			i286_memorywrite_w(I286_DI + ES_BASE, dat);
			I286_DI += stp;
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

// ---------------------------------------------------------------------- outs

I286EXT i286c_rep_outsb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			BYTE dat = i286_memoryread(I286_SI + DS_FIX);
			I286_SI += stp;
			iocore_out8(I286_DX, dat);
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

I286EXT i286c_rep_outsw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			WORD dat = i286_memoryread_w(I286_SI + DS_FIX);
			I286_SI += stp;
			iocore_out16(I286_DX, dat);
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

// ---------------------------------------------------------------------- movs

I286EXT i286c_rep_movsb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			BYTE dat = i286_memoryread(I286_SI + DS_FIX);
			i286_memorywrite(I286_DI + ES_BASE, dat);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

I286EXT i286c_rep_movsw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			WORD dat = i286_memoryread_w(I286_SI + DS_FIX);
			i286_memorywrite_w(I286_DI + ES_BASE, dat);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

// ---------------------------------------------------------------------- lods

I286EXT i286c_rep_lodsb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			I286_AL = i286_memoryread(I286_SI + DS_FIX);
			I286_SI += stp;
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

I286EXT i286c_rep_lodsw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			I286_AX = i286_memoryread_w(I286_SI + DS_FIX);
			I286_SI += stp;
			I286_CLOCK(4)
		} while(--I286_CX);
	}
}

// ---------------------------------------------------------------------- stos

I286EXT i286c_rep_stosb(void) {

	I286_CLOCK(4)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			i286_memorywrite(I286_DI + ES_BASE, I286_AL);
			I286_DI += stp;
			I286_CLOCK(3)
		} while(--I286_CX);
	}
}

I286EXT i286c_rep_stosw(void) {

	I286_CLOCK(4)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			i286_memorywrite_w(I286_DI + ES_BASE, I286_AX);
			I286_DI += stp;
			I286_CLOCK(3)
		} while(--I286_CX);
	}
}

// ---------------------------------------------------------------------- cmps

I286EXT i286c_repe_cmpsb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			DWORD res;
			DWORD dst = i286_memoryread(I286_SI + DS_FIX);
			DWORD src = i286_memoryread(I286_DI + ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(9)
			BYTE_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_cmpsb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		do {
			DWORD res;
			DWORD dst = i286_memoryread(I286_SI + DS_FIX);
			DWORD src = i286_memoryread(I286_DI + ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(9)
			BYTE_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

I286EXT i286c_repe_cmpsw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			DWORD res;
			DWORD dst = i286_memoryread_w(I286_SI + DS_FIX);
			DWORD src = i286_memoryread_w(I286_DI + ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(9)
			WORD_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_cmpsw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		do {
			DWORD res;
			DWORD dst = i286_memoryread_w(I286_SI + DS_FIX);
			DWORD src = i286_memoryread_w(I286_DI + ES_BASE);
			I286_SI += stp;
			I286_DI += stp;
			I286_CLOCK(9)
			WORD_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

// ---------------------------------------------------------------------- scas

I286EXT i286c_repe_scasb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		DWORD dst = I286_AL;
		do {
			DWORD res;
			DWORD src = i286_memoryread(I286_DI + ES_BASE);
			I286_DI += stp;
			I286_CLOCK(8)
			BYTE_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_scasb(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIR;
		DWORD dst = I286_AL;
		do {
			DWORD res;
			DWORD src = i286_memoryread(I286_DI + ES_BASE);
			I286_DI += stp;
			I286_CLOCK(8)
			BYTE_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

I286EXT i286c_repe_scasw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		DWORD dst = I286_AX;
		do {
			DWORD res;
			DWORD src = i286_memoryread_w(I286_DI + ES_BASE);
			I286_DI += stp;
			I286_CLOCK(8)
			WORD_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (I286_FLAGL & Z_FLAG));
	}
}

I286EXT i286c_repne_scasw(void) {

	I286_CLOCK(5)
	if (I286_CX) {
		WORD stp = STRING_DIRx2;
		DWORD dst = I286_AX;
		do {
			DWORD res;
			DWORD src = i286_memoryread_w(I286_DI + ES_BASE);
			I286_DI += stp;
			I286_CLOCK(8)
			WORD_SUB(res, dst, src)
			I286_CX--;
		} while((I286_CX) && (!(I286_FLAGL & Z_FLAG)));
	}
}

