#ifndef	NP2_I386C_MEMORY_H__
#define	NP2_I386C_MEMORY_H__

#ifdef NP2_MEMORY_ASM			// アセンブラ版は 必ずfastcallで
#undef	MEMCALL
#define	MEMCALL	FASTCALL
#endif


// 000000-0fffff メインメモリ
// 100000-10ffef HMA
// 110000-193fff FONT-ROM/RAM
// 1a8000-1bffff VRAM1
// 1c0000-1c7fff ITF-ROM BAK
// 1c8000-1dffff EPSON RAM
// 1e0000-1e7fff VRAM1
// 1f8000-1fffff ITF-ROM

#define	VRAM_STEP	0x100000
#define	VRAM_B		0x0a8000
#define	VRAM_R		0x0b0000
#define	VRAM_G		0x0b8000
#define	VRAM_E		0x0e0000

#define	VRAMADDRMASKEX(a)	((a) & (VRAM_STEP | 0x7fff))

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

#ifdef __cplusplus
extern "C" {
#endif

extern	BYTE	mem[0x200000];

void MEMCALL i286_memorymap(UINT type);
void MEMCALL i286_vram_dispatch(UINT operate);

void MEMCALL i286_memorywrite(UINT32 address, REG8 value);
void MEMCALL i286_memorywrite_w(UINT32 address, REG16 value);
void MEMCALL i286_memorywrite_d(UINT32 address, UINT32 value);
REG8 MEMCALL i286_memoryread(UINT32 address);
REG16 MEMCALL i286_memoryread_w(UINT32 address);
UINT32 MEMCALL i286_memoryread_d(UINT32 address);

#ifdef NP2_MEMORY_ASM
BYTE MEMCALL i286_membyte_read(WORD seg, WORD off);
WORD MEMCALL i286_memword_read(WORD seg, WORD off);
void MEMCALL i286_membyte_write(WORD seg, WORD off, BYTE dat);
void MEMCALL i286_memword_write(WORD seg, WORD off, WORD dat);
#else
#define	i286_membyte_read(a, b) \
	i286_memoryread(((DWORD)(a) << 4) + (WORD)(b))
#define	i286_memword_read(a, b) \
	i286_memoryread_w(((DWORD)(a) << 4) + (WORD)(b))

#define	i286_membyte_write(a, b, c) \
	i286_memorywrite(((DWORD)(a) << 4) + (WORD)(b), (c))
#define	i286_memword_write(a, b, c) \
	i286_memorywrite_w(((DWORD)(a) << 4) + (WORD)(b), (c))
#endif

void MEMCALL i286_memstr_read(UINT seg, UINT off, void *dat, UINT leng);
void MEMCALL i286_memstr_write(UINT seg, UINT off, const void *dat, UINT leng);
void MEMCALL i286_memx_read(UINT32 address, void *dat, UINT leng);
void MEMCALL i286_memx_write(UINT32 address, const void *dat, UINT leng);

#ifdef __cplusplus
}
#endif

#endif	/* !NP2_I386C_MEMORY_H__ */
