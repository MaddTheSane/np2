#ifndef	NP2_I386C_MEMORY_H__
#define	NP2_I386C_MEMORY_H__

#ifdef NP2_MEMORY_ASM			// アセンブラ版は 必ずfastcallで
#undef	MEMCALL
#define	MEMCALL	FASTCALL
#endif

#ifndef GCC_ATTR_REGPARM
#define	GCC_ATTR_REGPARM
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

void MEMCALL i286_memorywrite(UINT32 address, REG8 value) GCC_ATTR_REGPARM;
void MEMCALL i286_memorywrite_w(UINT32 address, REG16 value) GCC_ATTR_REGPARM;
void MEMCALL i286_memorywrite_d(UINT32 address, UINT32 value) GCC_ATTR_REGPARM;
REG8 MEMCALL i286_memoryread(UINT32 address) GCC_ATTR_REGPARM;
REG16 MEMCALL i286_memoryread_w(UINT32 address) GCC_ATTR_REGPARM;
UINT32 MEMCALL i286_memoryread_d(UINT32 address) GCC_ATTR_REGPARM;

#ifdef NP2_MEMORY_ASM
REG8 MEMCALL i286_membyte_read(UINT seg, UINT off) GCC_ATTR_REGPARM;
REG16 MEMCALL i286_memword_read(UINT seg, UINT off) GCC_ATTR_REGPARM;
void MEMCALL i286_membyte_write(UINT seg, UINT off, REG8 dat) GCC_ATTR_REGPARM;
void MEMCALL i286_memword_write(UINT seg, UINT off, REG16 dat) GCC_ATTR_REGPARM;
#else
#define	i286_membyte_read(a, b) \
	i286_memoryread(((UINT32)(a) << 4) + (UINT16)(b))
#define	i286_memword_read(a, b) \
	i286_memoryread_w(((UINT32)(a) << 4) + (UINT16)(b))

#define	i286_membyte_write(a, b, c) \
	i286_memorywrite(((UINT32)(a) << 4) + (UINT16)(b), (c))
#define	i286_memword_write(a, b, c) \
	i286_memorywrite_w(((UINT32)(a) << 4) + (UINT16)(b), (c))
#endif

void MEMCALL memp_read(UINT32 address, void *dat, UINT leng) GCC_ATTR_REGPARM;
void MEMCALL memp_write(UINT32 address, const void *dat, UINT leng) GCC_ATTR_REGPARM;

REG8 MEMCALL meml_read8(UINT seg, UINT off) GCC_ATTR_REGPARM;
REG16 MEMCALL meml_read16(UINT seg, UINT off) GCC_ATTR_REGPARM;
void MEMCALL meml_write8(UINT seg, UINT off, REG8 dat) GCC_ATTR_REGPARM;
void MEMCALL meml_write16(UINT seg, UINT off, REG16 dat) GCC_ATTR_REGPARM;
void MEMCALL meml_readstr(UINT seg, UINT off, void *dat, UINT leng) GCC_ATTR_REGPARM;
void MEMCALL meml_writestr(UINT seg, UINT off, const void *dat, UINT leng) GCC_ATTR_REGPARM;
void MEMCALL meml_read(UINT32 address, void *dat, UINT leng) GCC_ATTR_REGPARM;
void MEMCALL meml_write(UINT32 address, const void *dat, UINT leng) GCC_ATTR_REGPARM;



// ---- Physical Space (DMA)

#define	MEMP_READ8(addr)					\
			i286_memoryread((addr))
#define	MEMP_WRITE8(addr, dat)				\
			i286_memorywrite((addr), (dat))

#define MEMP_READ(addr, dat, leng)			\
			memp_read((addr), (dat), (leng))
#define MEMP_WRITE(addr, dat, leng)			\
			memp_write((addr), (dat), (leng))


// ---- Logical Space (BIOS)

#define	MEML_READ8(seg, off)				\
			meml_read8((seg), (off))
#define	MEML_READ16(seg, off)				\
			meml_read16((seg), (off))
#define	MEML_WRITE8(seg, off, dat)			\
			meml_write8((seg), (off), (dat))
#define	MEML_WRITE16(seg, off, dat)			\
			meml_write16((seg), (off), (dat))
#define MEML_READSTR(seg, off, dat, leng)	\
			meml_readstr((seg), (off), (dat), (leng))
#define MEML_WRITESTR(seg, off, dat, leng)	\
			meml_writestr((seg), (off), (dat), (leng))
#define MEML_READ(addr, dat, leng)			\
			meml_read((addr), (dat), (leng))
#define MEML_WRITE(addr, dat, leng)			\
			meml_write((addr), (dat), (leng))

#ifdef __cplusplus
}
#endif

#endif	/* !NP2_I386C_MEMORY_H__ */
