
#ifdef NP2_MEMORY_ASM					// アセンブラ版は 必ずfastcallで
#undef	MEMCALL
#define	MEMCALL	FASTCALL
#endif


// 000000-0fffff メインメモリ
// 100000-10ffef HMA
// 110000-193fff FONT-ROM/RAM
// 1a8000-1e7fff VRAM1
// 1f8000-1fffff ITF-ROM

#define	VRAM_STEP	0x100000
#define	VRAM_B		0x0a8000
#define	VRAM_R		0x0b0000
#define	VRAM_G		0x0b8000
#define	VRAM_E		0x0e0000

#define	VRAM_POS(a)	((a) & (VRAM_STEP | 0x7fff))

#define	VRAM0_B		VRAM_B
#define	VRAM0_R		VRAM_R
#define	VRAM0_G		VRAM_G
#define	VRAM0_E		VRAM_E
#define	VRAM1_B		(VRAM_STEP + VRAM_B)
#define	VRAM1_R		(VRAM_STEP + VRAM_R)
#define	VRAM1_G		(VRAM_STEP + VRAM_G)
#define	VRAM1_E		(VRAM_STEP + VRAM_E)

#define	FONT_ADRS	0x110000
#define	ITF_ADRS	0x1f8000

#if defined(CPUCORE_IA32)
#if 0
#define	LOWMEM		0x100000
#else
#define	LOWMEM		0x10fff0
#endif
#endif	/* CPUCORE_IA32 */


#if defined(BYTESEX_LITTLE)

#define	MEM_SETB(pos, data)		mem[pos] = data
#define	MEM_ANDB(pos, data)		mem[pos] &= data
#define	MEM_XORB(pos, data)		mem[pos] ^= data
#define	MEM_ORB(pos, data)		mem[pos] |= data
#define	MEM_SETW(pos, data)		*(WORD *)(mem + (pos)) = (data)
#define	MEM_SETD(pos, data)		*(DWORD *)(mem + (pos)) = (data)
#define	MEM_GETB(pos)			(mem[pos])
#define	MEM_GETW(pos)			(*(WORD *)(mem + (pos)))
#define	MEM_GETD(pos)			(*(DWORD *)(mem + (pos)))

#elif defined(BYTESEX_BIG)

#define	MEM_SETB(pos, data)		mem[pos] = data
#define	MEM_ANDB(pos, data)		mem[pos] &= data
#define	MEM_XORB(pos, data)		mem[pos] ^= data
#define	MEM_ORB(pos, data)		mem[pos] |= data
#define	MEM_SETW(pos, data)												\
			mem[(pos)] = (BYTE)data;									\
			mem[(pos) + 1] = (BYTE)(data >> 8);
#define	MEM_SETD(pos, data)												\
			mem[(pos)] = (BYTE)data;									\
			mem[(pos) + 1] = (BYTE)(data >> 8);							\
			mem[(pos) + 2] = (BYTE)(data >> 16);						\
			mem[(pos) + 3] = (BYTE)(data >> 24);
#define	MEM_GETB(pos)			(mem[pos])
#define	MEM_GETW(pos)													\
			(((WORD)mem[(pos) + 1] << 8) | mem[(pos)])
#define	MEM_GETD(pos)													\
			(((DWORD)mem[(pos) + 3] << 24) |							\
			 ((DWORD)mem[(pos) + 2] << 16) |							\
			  ((WORD)mem[(pos) + 1] <<  8) | mem[(pos)])
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern	BYTE	mem[0x200000];

void MEMCALL i286_vram_dispatch(UINT operate);

void MEMCALL __i286_memorywrite(UINT32 address, REG8 value);
void MEMCALL __i286_memorywrite_w(UINT32 address, REG16 value);
void MEMCALL __i286_memorywrite_d(UINT32 address, UINT32 value);
REG8 MEMCALL __i286_memoryread(UINT32 address);
REG16 MEMCALL __i286_memoryread_w(UINT32 address);
UINT32 MEMCALL __i286_memoryread_d(UINT32 address);

#ifndef CPUCORE_IA32
#define	i286_memorywrite(a,v)	__i286_memorywrite(a,v)
#define	i286_memorywrite_w(a,v)	__i286_memorywrite_w(a,v)
#define	i286_memorywrite_d(a,v)	__i286_memorywrite_d(a,v)
#define	i286_memoryread(a)		__i286_memoryread(a)
#define	i286_memoryread_w(a)	__i286_memoryread_w(a)
#define	i286_memoryread_d(a)	__i286_memoryread_d(a)
#else	/* CPUCORE_IA32 */
void MEMCALL cpu_memorywrite(DWORD address, BYTE value);
void MEMCALL cpu_memorywrite_w(DWORD address, WORD value);
void MEMCALL cpu_memorywrite_d(DWORD address, DWORD value);
BYTE MEMCALL cpu_memoryread(DWORD address);
WORD MEMCALL cpu_memoryread_w(DWORD address);
DWORD MEMCALL cpu_memoryread_d(DWORD address);
#define	i286_memorywrite(a,v)	cpu_memorywrite(a,v)
#define	i286_memorywrite_w(a,v)	cpu_memorywrite_w(a,v)
#define	i286_memorywrite_d(a,v)	cpu_memorywrite_d(a,v)
#define	i286_memoryread(a)		cpu_memoryread(a)
#define	i286_memoryread_w(a)	cpu_memoryread_w(a)
#define	i286_memoryread_d(a)	cpu_memoryread_d(a)
#endif	/* !CPUCORE_IA32 */

#ifdef NP2_MEMORY_ASM
BYTE MEMCALL i286_membyte_read(WORD seg, WORD off);
WORD MEMCALL i286_memword_read(WORD seg, WORD off);
void MEMCALL i286_membyte_write(WORD seg, WORD off, BYTE dat);
void MEMCALL i286_memword_write(WORD seg, WORD off, WORD dat);
#else
#define	i286_membyte_read(a, b)										\
				i286_memoryread(((DWORD)(a) << 4) + (WORD)(b))
#define	i286_memword_read(a, b)										\
				i286_memoryread_w(((DWORD)(a) << 4) + (WORD)(b))

#define	i286_membyte_write(a, b, c)									\
				i286_memorywrite(((DWORD)(a) << 4) + (WORD)(b), (c))
#define	i286_memword_write(a, b, c)									\
				i286_memorywrite_w(((DWORD)(a) << 4) + (WORD)(b), (c))
#endif

void MEMCALL i286_memstr_read(UINT seg, UINT off, void *dat, UINT leng);
void MEMCALL i286_memstr_write(UINT seg, UINT off,
												const void *dat, UINT leng);
void MEMCALL i286_memx_read(UINT32 address, void *dat, UINT leng);
void MEMCALL i286_memx_write(UINT32 address, const void *dat, UINT leng);

#ifdef __cplusplus
}
#endif


// とりあえず
#define	GETDWORD(a)		(((UINT32)(a)[0]) |				\
						((UINT32)(a)[1] << 8) |			\
						((UINT32)(a)[2] << 16) |		\
						((UINT32)(a)[3] << 24))
#define	GETWORD(a)		(((UINT16)(a)[0]) | ((UINT16)(a)[1] << 8))
#define	SETDWORD(a, b)	*((a)+0) = (BYTE)((b));			\
						*((a)+1) = (BYTE)((b)>>8);		\
						*((a)+2) = (BYTE)((b)>>16);		\
						*((a)+3) = (BYTE)((b)>>24)
#define	SETWORD(a, b)	*((a)+0) = (BYTE)((b));			\
						*((a)+1) = (BYTE)((b)>>8)

