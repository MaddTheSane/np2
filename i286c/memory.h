
#ifndef MEMCALL
#define	MEMCALL
#endif


// 000000-0fffff ÉÅÉCÉìÉÅÉÇÉä
// 100000-10ffef HMA
// 110000-193fff FONT-ROM/RAM
// 1a8000-1e7fff VRAM1
// 1f8000-1fffff ITF-ROM

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

#define	VRAM_POS(a)	(a & (VRAM_STEP | 0x7fff))


#ifdef __cplusplus
extern "C" {
#endif

extern	BYTE	mem[0x200000];

void MEMCALL i286_vram_dispatch(UINT operate);

BYTE MEMCALL i286_memoryread(UINT32 address);
UINT16 MEMCALL i286_memoryread_w(UINT32 address);
void MEMCALL i286_memorywrite(UINT32 address, BYTE value);
void MEMCALL i286_memorywrite_w(UINT32 address, UINT16 value);

BYTE MEMCALL i286_membyte_read(UINT seg, UINT off);
UINT16 MEMCALL i286_memword_read(UINT seg, UINT off);
void MEMCALL i286_membyte_write(UINT seg, UINT off, BYTE value);
void MEMCALL i286_memword_write(UINT seg, UINT off, UINT16 value);

void MEMCALL i286_memstr_read(UINT seg, UINT off, void *dat, UINT leng);
void MEMCALL i286_memstr_write(UINT seg, UINT off,
											const void *dat, UINT leng);

void MEMCALL i286_memx_read(UINT32 address, void *dat, UINT leng);
void MEMCALL i286_memx_write(UINT32 address, const void *dat, UINT leng);

#ifdef __cplusplus
}
#endif

