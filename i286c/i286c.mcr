
#define	INHIBIT_WORDP(m)	(1)

#define	__CBW(src)		(UINT16)((char)(src))
#define	__CBD(src)		((char)(src))
#define	WORD2LONG(src)	((short)(src))


#define	SEGMENTPTR(s)	((UINT16 *)(&I286_SEGREG) + (s))

#define REAL_FLAGREG	(UINT16)((I286_FLAG & 0x7ff) | (I286_OV?O_FLAG:0))

#define	STRING_DIR		((I286_FLAG & D_FLAG)?-1:1)
#define	STRING_DIRx2	((I286_FLAG & D_FLAG)?-2:2)

#if !defined(MEMOPTIMIZE)
#define	WORDSZPF(a)		szpflag_w[(a)]
#else
#define	WORDSZPF(a)		((szpcflag[(a) & 0xff] & P_FLAG) | \
									(((a))?0:Z_FLAG) | (((a) >> 8) & S_FLAG))
#endif

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
#define	REG8_B53(op)		_reg8_b53[(op)]
#define	REG8_B20(op)		_reg8_b20[(op)]
#else
#if defined(BYTESEX_LITTLE)
#define	REG8_B53(op)		\
				(((BYTE *)&I286_REG) + (((op) >> 2) & 6) + (((op) >> 5) & 1))
#define	REG8_B20(op)		\
				(((BYTE *)&I286_REG) + (((op) & 3) * 2) + (((op) >> 2) & 1))
#else
#define	REG8_B53(op)		(((BYTE *)&I286_REG) + (((op) >> 2) & 6) +	\
													((((op) >> 5) & 1) ^ 1))
#define	REG8_B20(op)		(((BYTE *)&I286_REG) + (((op) & 3) * 2) +	\
													((((op) >> 2) & 1) ^ 1))
#endif
#endif

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
#define	REG16_B53(op)		_reg16_b53[(op)]
#define	REG16_B20(op)		_reg16_b20[(op)]
#else
#define	REG16_B53(op)		(((UINT16 *)&I286_REG) + (((op) >> 3) & 7))
#define	REG16_B20(op)		(((UINT16 *)&I286_REG) + ((op) & 7))
#endif

#define	SWAPBYTE(p, q) {											\
		BYTE tmp = (p);												\
		(p) = (q);													\
		(q) = tmp;													\
	}


#define	SWAPWORD(p, q) {											\
		UINT16 tmp;													\
		tmp = (p);													\
		(p) = (q);													\
		(q) = tmp;													\
	}


#define	I286IRQCHECKTERM											\
		if (I286_REMCLOCK > 0) {									\
			I286_BASECLOCK -= I286_REMCLOCK;						\
			I286_REMCLOCK = 0;										\
		}


#define	REMOVE_PREFIX												\
		SS_FIX = SS_BASE;											\
		DS_FIX = DS_BASE;


#define	I286_WORKCLOCK(c)	I286_REMCLOCK -= (c)


#define	GET_PCBYTE(b)												\
		(b) = i286_memoryread(CS_BASE + I286_IP);					\
		I286_IP++;


#define	GET_PCBYTES(b)												\
		(b) = __CBW(i286_memoryread(CS_BASE + I286_IP));			\
		I286_IP++;


#define	GET_PCBYTESD(b)												\
		(b) = __CBD(i286_memoryread(CS_BASE + I286_IP));			\
		I286_IP++;


#define	GET_PCWORD(b)												\
		(b) = i286_memoryread_w(CS_BASE + I286_IP);					\
		I286_IP += 2;


#define	PREPART_EA_REG8(b, d_s)										\
		GET_PCBYTE((b))												\
		(d_s) = *(REG8_B53(b));


#define	PREPART_EA_REG8P(b, d_s)									\
		GET_PCBYTE((b))												\
		(d_s) = REG8_B53(b);


#define	PREPART_EA_REG16(b, d_s)									\
		GET_PCBYTE((b))												\
		(d_s) = *(REG16_B53(b));


#define	PREPART_EA_REG16P(b, d_s)									\
		GET_PCBYTE((b))												\
		(d_s) = REG16_B53(b);


#define PREPART_REG8_EA(b, s, d, regclk, memclk)					\
		GET_PCBYTE((b))												\
		if ((b) >= 0xc0) {											\
			I286_WORKCLOCK(regclk);									\
			(s) = *(REG8_B20(b));									\
		}															\
		else {														\
			I286_WORKCLOCK(memclk);									\
			(s) = i286_memoryread(c_calc_ea_dst[(b)]());			\
		}															\
		(d) = REG8_B53(b);


#define	PREPART_REG16_EA(b, s, d, regclk, memclk)					\
		GET_PCBYTE(b)												\
		if (b >= 0xc0) {											\
			I286_WORKCLOCK(regclk);									\
			s = *(REG16_B20(b));									\
		}															\
		else {														\
			I286_WORKCLOCK(memclk);									\
			s = i286_memoryread_w(c_calc_ea_dst[b]());				\
		}															\
		d = REG16_B53(b);


#define	ADDBYTE(r, d, s)											\
		(r) = (s) + (d);											\
		I286_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80;					\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		I286_FLAGL |= szpcflag[(r)];


#define	ADDWORD(r, d, s)											\
		(r) = (s) + (d);											\
		I286_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x8000;				\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		if ((r) & 0xffff0000) {										\
			(r) &= 0x0000ffff;										\
			I286_FLAGL |= C_FLAG;									\
		}															\
		I286_FLAGL |= WORDSZPF(r);


// flag no check
#define	ORBYTE(d, s)												\
		(d) |= (s);													\
		I286_OV = 0;												\
		I286_FLAGL = szpcflag[(d)];


#define	ORWORD(d, s)												\
		(d) |= (s);													\
		I286_OV = 0;												\
		I286_FLAGL = WORDSZPF(d);


#define	ADCBYTE(r, d, s) 											\
		(r) = (I286_FLAGL & 1) + (s) + (d);							\
		I286_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80;					\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		I286_FLAGL |= szpcflag[(r)];


#define	ADCWORD(r, d, s) 											\
		(r) = (I286_FLAGL & 1) + (s) + (d);							\
		I286_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x8000;				\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		if ((r) & 0xffff0000) {										\
			(r) &= 0x0000ffff;										\
			I286_FLAGL |= C_FLAG;									\
		}															\
		I286_FLAGL |= WORDSZPF(r);


// flag no check
#define	SBBBYTE(r, d, s) 											\
		(r) = (d) - (s) - (I286_FLAGL & 1);							\
		I286_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80;					\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		I286_FLAGL |= szpcflag[(r) & 0x1ff];

#define	SBBWORD(r, d, s) 											\
		(r) = (d) - (s) - (I286_FLAGL & 1);							\
		I286_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x8000;				\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		if ((r) & 0xffff0000) {										\
			(r) &= 0x0000ffff;										\
			I286_FLAGL |= C_FLAG;									\
		}															\
		I286_FLAGL |= WORDSZPF(r);


// flag no check
#define	ANDBYTE(d, s)												\
		(d) &= (s);													\
		I286_OV = 0;												\
		I286_FLAGL = szpcflag[(d)];


#define	ANDWORD(d, s)												\
		(d) &= s;													\
		I286_OV = 0;												\
		I286_FLAGL = WORDSZPF(d);


// flag no check
#define	BYTE_SUB(r, d, s) 											\
		(r) = (d) - (s);											\
		I286_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80;					\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		I286_FLAGL |= szpcflag[(r) & 0x1ff];

#define	WORD_SUB(r, d, s) 											\
		(r) = (d) - (s);											\
		I286_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x8000;				\
		I286_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG);			\
		if ((r) & 0xffff0000) {										\
			(r) &= 0x0000ffff;										\
			I286_FLAGL |= C_FLAG;									\
		}															\
		I286_FLAGL |= WORDSZPF(r);


// flag no check
#define	BYTE_XOR(d, s)												\
		(d) ^= s;													\
		I286_OV = 0;												\
		I286_FLAGL = szpcflag[(d)];


#define	WORD_XOR(d, s)												\
		(d) ^= (s);													\
		I286_OV = 0;												\
		I286_FLAGL = WORDSZPF(d);


#define	BYTE_NEG(d, s) 												\
		(d) = 0 - (s);												\
		I286_OV = ((d) & (s)) & 0x80;								\
		I286_FLAGL = (BYTE)(((d) ^ (s)) & A_FLAG);					\
		I286_FLAGL |= szpcflag[(d) & 0x1ff];


#define	WORD_NEG(d, s) 												\
		(d) = 0 - (s);												\
		I286_OV = ((d) & (s)) & 0x8000;								\
		I286_FLAGL = (BYTE)(((d) ^ (s)) & A_FLAG);					\
		if ((d) & 0xffff0000) {										\
			(d) &= 0x0000ffff;										\
			I286_FLAGL |= C_FLAG;									\
		}															\
		I286_FLAGL |= WORDSZPF(d);


#define	BYTE_MUL(r, d, s)											\
		I286_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG);			\
		(r) = (BYTE)(d) * (BYTE)(s);								\
		I286_OV = (r) >> 8;											\
		if (I286_OV) {												\
			I286_FLAGL |= C_FLAG;									\
		}


#define	WORD_MUL(r, d, s)											\
		I286_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG);			\
		(r) = (UINT16)(d) * (UINT16)(s);							\
		I286_OV = (r) >> 16;										\
		if (I286_OV) {												\
			I286_FLAGL |= C_FLAG;									\
		}


#define	BYTE_IMUL(r, d, s)											\
		I286_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG);			\
		(r) = (char)(d) * (char)(s);								\
		I286_OV = ((r) + 0x80) & 0xffffff00;						\
		if (I286_OV) {												\
			I286_FLAGL |= C_FLAG;									\
		}


#define	WORD_IMUL(r, d, s)											\
		I286_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG);			\
		(r) = (short)(d) * (short)(s);								\
		I286_OV = ((r) + 0x8000) & 0xffff0000;						\
		if (I286_OV) {												\
			I286_FLAGL |= C_FLAG;									\
		}


// flag no check
#define	BYTE_INC(s) {												\
		BYTE	b;													\
		b = (s);													\
		b++;														\
		I286_OV = b & (b ^ (s)) & 0x80;								\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)((b ^ (s)) & A_FLAG);					\
		I286_FLAGL |= szpcflag[b];									\
		(s) = b;													\
	}


#define	WORD_INC(s) {												\
		UINT16 b;													\
		b = (s);													\
		b++;														\
		I286_OV = b & (b ^ (s)) & 0x8000;							\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)((b ^ (s)) & A_FLAG);					\
		I286_FLAGL |= WORDSZPF(b);									\
		(s) = b;													\
	}


// flag no check
#define	BYTE_DEC(s) {												\
		BYTE b = (s);												\
		b--;														\
		I286_OV = (s) & (b ^ (s)) & 0x80;							\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)((b ^ (s)) & A_FLAG);					\
		I286_FLAGL |= szpcflag[b];									\
		(s) = b;													\
	}


#define	WORD_DEC(s) {												\
		UINT16 b = (s);												\
		b--;														\
		I286_OV = (s) & (b ^ (s)) & 0x8000;							\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)((b ^ (s)) & A_FLAG);					\
		I286_FLAGL |= WORDSZPF(b);									\
		(s) = b;													\
	}


// flag no check
#define	INCWORD(w, clock) {											\
		UINT16 bak;													\
		bak = (w);													\
		(w)++;														\
		I286_OV = (w) & ((w) ^ bak) & 0x8000;						\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)(((w) ^ bak) & A_FLAG);					\
		I286_FLAGL |= WORDSZPF(w);									\
		I286_WORKCLOCK(clock);										\
	}


#define	DECWORD(w, clock) {											\
		UINT16 bak;													\
		bak = (w);													\
		w--;														\
		I286_OV = bak & (w ^ bak) & 0x8000;							\
		I286_FLAGL &= C_FLAG;										\
		I286_FLAGL |= (BYTE)((w ^ bak) & A_FLAG);					\
		I286_FLAGL |= WORDSZPF(w);									\
		I286_WORKCLOCK(clock);										\
	}


#define	REGPUSH(reg, clock)	{										\
		I286_WORKCLOCK(clock);										\
		I286_SP -= 2;												\
		i286_memorywrite_w(I286_SP + SS_BASE, reg);					\
	}


#define	REGPUSH0(reg)												\
		I286_SP -= 2;												\
		i286_memorywrite_w(I286_SP + SS_BASE, reg);


#define	SP_PUSH(reg, clock)	{										\
		UINT16 sp = reg;											\
		I286_SP -= 2;												\
		i286_memorywrite_w(I286_SP + SS_BASE, sp);					\
		I286_WORKCLOCK(clock);										\
	}


#define	REGPOP(reg, clock) {										\
		I286_WORKCLOCK(clock);										\
		reg = i286_memoryread_w(I286_SP + SS_BASE);					\
		I286_SP += 2;												\
	}

#define	SP_POP(reg, clock) {										\
		I286_WORKCLOCK(clock);										\
		reg = i286_memoryread_w(I286_SP + SS_BASE);					\
	}


#define	REGPOP0(reg) 												\
		reg = i286_memoryread_w(I286_SP + SS_BASE);					\
		I286_SP += 2;



#define	JMPSHORT(clock) {											\
		I286_WORKCLOCK(clock);										\
		I286_IP += __CBW(i286_memoryread(CS_BASE + I286_IP));		\
		I286_IP++;													\
	}


#define	JMPNOP(clock) {												\
		I286_WORKCLOCK(clock);										\
		I286_IP++;													\
	}


#define	MOVIMM8(reg) {												\
		I286_WORKCLOCK(2);											\
		GET_PCBYTE(reg)												\
	}


#define	MOVIMM16(reg) {												\
		I286_WORKCLOCK(2);											\
		GET_PCWORD(reg)												\
	}


#define	INT_NUM(a, b)		i286_intnum((a), (UINT16)(b))

