
#ifndef MEMCALL
#define	MEMCALL
#endif


// 000000-0fffff ���C��������
// 100000-10ffef HMA
// 110000-193fff FONT-ROM/RAM
// 1a8000-1bffff VRAM1
// 1c0000-1c7fff ITF-ROM BAK
// 1c8000-1dffff EPSON RAM
// 1e0000-1e7fff VRAM1
// 1f8000-1fffff ITF-ROM

#define	USE_HIMEM		0x110000

enum {
	VRAM_STEP	= 0x100000,
	VRAM_B		= 0x0a8000,
	VRAM_R		= 0x0b0000,
	VRAM_G		= 0x0b8000,
	VRAM_E		= 0x0e0000,

	VRAM0_B		= VRAM_B,
	VRAM0_R		= VRAM_R,
	VRAM0_G		= VRAM_G,
	VRAM0_E		= VRAM_E,
	VRAM1_B		= (VRAM_STEP + VRAM_B),
	VRAM1_R		= (VRAM_STEP + VRAM_R),
	VRAM1_G		= (VRAM_STEP + VRAM_G),
	VRAM1_E		= (VRAM_STEP + VRAM_E),

	FONT_ADRS	= 0x110000,
	ITF_ADRS	= 0x1f8000
};

#define	VRAMADDRMASKEX(a)	((a) & (VRAM_STEP | 0x7fff))


#ifdef __cplusplus
extern "C" {
#endif

void MEMCALL i286_memorymap(UINT type);
void MEMCALL i286_vram_dispatch(UINT operate);

REG8 MEMCALL i286_memoryread(UINT32 address);
REG16 MEMCALL i286_memoryread_w(UINT32 address);
void MEMCALL i286_memorywrite(UINT32 address, REG8 value);
void MEMCALL i286_memorywrite_w(UINT32 address, REG16 value);

REG8 MEMCALL i286_membyte_read(UINT seg, UINT off);
REG16 MEMCALL i286_memword_read(UINT seg, UINT off);
void MEMCALL i286_membyte_write(UINT seg, UINT off, REG8 value);
void MEMCALL i286_memword_write(UINT seg, UINT off, REG16 value);

void MEMCALL i286_memstr_read(UINT seg, UINT off, void *dat, UINT leng);
void MEMCALL i286_memstr_write(UINT seg, UINT off,
												const void *dat, UINT leng);

void MEMCALL i286_memx_read(UINT32 address, void *dat, UINT leng);
void MEMCALL i286_memx_write(UINT32 address, const void *dat, UINT leng);


// ---- Physical Space (DMA)

#define	MEMP_READ8(addr)					\
			i286_memoryread((addr))
#define	MEMP_WRITE8(addr, dat)				\
			i286_memorywrite((addr), (dat))


// ---- Logical Space (BIOS)

#define	MEML_READ8(seg, off)				\
			i286_membyte_read((seg), (off))
#define	MEML_READ16(seg, off)				\
			i286_memwrite_read((seg), (off))
#define	MEML_WRITE8(seg, off, dat)			\
			i286_membyte_write((seg), (off), (dat));
#define	MEML_WRITE16(seg, off, dat)			\
			i286_memword_write((seg), (off), (dat));
#define MEML_READSTR(seg, off, dat, leng)	\
			i286_memstr_read((seg), (off), (dat), (leng))
#define MEML_WRITESTR(seg, off, dat, leng)	\
			i286_memstr_write((seg), (off), (dat), (leng))
#define MEML_READ(addr, dat, leng)			\
			i286_memx_read((addr), (dat), (leng))
#define MEML_WRITE(addr, dat, leng)			\
			i286_memx_write((addr), (dat), (leng))


#ifdef __cplusplus
}
#endif

