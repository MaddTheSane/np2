#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"v30patch.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"dmap.h"
#include	"i286c.mcr"


	I286REG		i286reg;

const BYTE iflags[256] = {					// Z_FLAG, S_FLAG, P_FLAG
			0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
			0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
			0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84};


// ----

	BYTE	_szpcflag8[0x200];

#if !defined(MEMOPTIMIZE)
	BYTE	_szpflag16[0x10000];
#endif

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
	BYTE	*_reg8_b53[256];
	BYTE	*_reg8_b20[256];
#endif
#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
	UINT16	*_reg16_b53[256];
	UINT16	*_reg16_b20[256];
#endif

void i286_initialize(void) {

	UINT	i;
	UINT	bit;
	BYTE	f;

	for (i=0; i<0x100; i++) {
		f = P_FLAG;
		for (bit=0x80; bit; bit>>=1) {
			if (i & bit) {
				f ^= P_FLAG;
			}
		}
		if (!(i & 0xff)) {
			f |= Z_FLAG;
		}
		if (i & 0x80) {
			f |= S_FLAG;
		}
		_szpcflag8[i+0x000] = f;
		_szpcflag8[i+0x100] = f | C_FLAG;
	}

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
	for (i=0; i<0x100; i++) {
		int pos;
#if defined(BYTESEX_LITTLE)
		pos = ((i & 0x20)?1:0);
#else
		pos = ((i & 0x20)?0:1);
#endif
		pos += ((i >> 3) & 3) * 2;
		_reg8_b53[i] = ((BYTE *)&I286_REG) + pos;
#if defined(BYTESEX_LITTLE)
		pos = ((i & 0x4)?1:0);
#else
		pos = ((i & 0x4)?0:1);
#endif
		pos += (i & 3) * 2;
		_reg8_b20[i] = ((BYTE *)&I286_REG) + pos;
#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
		_reg16_b53[i] = ((UINT16 *)&I286_REG) + ((i >> 3) & 7);
		_reg16_b20[i] = ((UINT16 *)&I286_REG) + (i & 7);
#endif
	}
#endif

#if !defined(MEMOPTIMIZE)
	for (i=0; i<0x10000; i++) {
		f = P_FLAG;
		for (bit=0x80; bit; bit>>=1) {
			if (i & bit) {
				f ^= P_FLAG;
			}
		}
		if (!i) {
			f |= Z_FLAG;
		}
		if (i & 0x8000) {
			f |= S_FLAG;
		}
		_szpflag16[i] = f;
	}
#endif
#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
	i286cea_initialize();
#endif
	v30init();
}

void i286_reset(void) {

	i286_initialize();
	ZeroMemory(&i286reg, sizeof(i286reg));
	I286_CS = 0x1fc0;
	CS_BASE = 0x1fc00;
}

void i286_resetprefetch(void) {
}

void CPUCALL i286_intnum(UINT vect, UINT IP) {

const BYTE	*ptr;

	REGPUSH0(REAL_FLAGREG)
	REGPUSH0(I286_CS)
	REGPUSH0((UINT16)IP)

	I286_FLAG &= ~(T_FLAG | I_FLAG);
	I286_TRAP = 0;

	ptr = I286_MEM + (vect * 4);
	I286_IP = LOADINTELWORD(ptr+0);				// real mode!
	I286_CS = LOADINTELWORD(ptr+2);				// real mode!
	CS_BASE = I286_CS << 4;
	I286_WORKCLOCK(20);
}

void CPUCALL i286_interrupt(BYTE vect) {

	UINT	op;
const BYTE	*ptr;

	op = i286_memoryread(I286_IP + CS_BASE);
	if (op == 0xf4) {							// hlt
		I286_IP++;
	}
	REGPUSH0(REAL_FLAGREG)						// ‚±‚±V30‚Ε’Òελ‚ª‡‚ν‚Θ‚Ά
	REGPUSH0(I286_CS)
	REGPUSH0(I286_IP)

	I286_FLAG &= ~(T_FLAG | I_FLAG);
	I286_TRAP = 0;

	ptr = I286_MEM + (vect * 4);
	I286_IP = LOADINTELWORD(ptr+0);				// real mode!
	I286_CS = LOADINTELWORD(ptr+2);				// real mode!
	CS_BASE = I286_CS << 4;
	I286_WORKCLOCK(20);
}

void i286(void) {

	UINT	opcode;

	if (I286_TRAP) {
		do {
			GET_PCBYTE(opcode);
			i286op[opcode]();
			if (I286_TRAP) {
				i286_interrupt(1);
			}
			dmap_i286();
		} while(I286_REMCLOCK > 0);
	}
	else if (dmac.working) {
		do {
			GET_PCBYTE(opcode);
			i286op[opcode]();
			dmap_i286();
		} while(I286_REMCLOCK > 0);
	}
	else {
		do {
			GET_PCBYTE(opcode);
			i286op[opcode]();
		} while(I286_REMCLOCK > 0);
	}
}

void i286_step(void) {

	UINT	opcode;

	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= ~(O_FLAG);

	GET_PCBYTE(opcode);
	i286op[opcode]();

	I286_FLAG &= ~(O_FLAG);
	if (I286_OV) {
		I286_FLAG |= (O_FLAG);
	}
	dmap_i286();
}


// ---- test

#if defined(I286C_TEST)
BYTE BYTESZPF(UINT r) {

	if (r & (~0xff)) {
		TRACEOUT(("BYTESZPF bound error: %x", r));
	}
	return(_szpcflag8[r & 0xff]);
}

BYTE BYTESZPCF(UINT r) {

	if (r & (~0x1ff)) {
		TRACEOUT(("BYTESZPCF bound error: %x", r));
	}
	return(_szpcflag8[r & 0x1ff]);
}

BYTE WORDSZPF(UINT32 r) {

	BYTE	f1;
	BYTE	f2;

	if (r & (~0xffff)) {
		TRACEOUT(("WORDSZPF bound error: %x", r));
	}
	f1 = _szpflag16[r & 0xffff];
	f2 = _szpcflag8[r & 0xff] & P_FLAG;
	f2 += (r)?0:Z_FLAG;
	f2 += (r >> 8) & S_FLAG;
	if (f1 != f2) {
		TRACEOUT(("word flag error: %.2x %.2x", f1, f2));
	}
	return(f1);
}

BYTE WORDSZPCF(UINT32 r) {

	BYTE	f1;
	BYTE	f2;

	if ((r & 0xffff0000) && (!(r & 0x00010000))) {
		TRACEOUT(("WORDSZPCF bound error: %x", r));
	}
	f1 = (r >> 16) & 1;
	f1 += _szpflag16[LOW16(r)];

	f2 = _szpcflag8[r & 0xff] & P_FLAG;
	f2 += (LOW16(r))?0:Z_FLAG;
	f2 += (r >> 8) & S_FLAG;
	f2 += (r >> 16) & 1;

	if (f1 != f2) {
		TRACEOUT(("word flag error: %.2x %.2x", f1, f2));
	}
	return(f1);
}
#endif

