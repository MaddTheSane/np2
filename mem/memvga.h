
#if defined(SUPPORT_PC9821)

#ifdef __cplusplus
extern "C" {
#endif

REG8 MEMCALL memvgaf_rd8(UINT32 address) GCC_ATTR_REGPARM;
void MEMCALL memvgaf_wr8(UINT32 address, REG8 value) GCC_ATTR_REGPARM;
REG16 MEMCALL memvgaf_rd16(UINT32 address) GCC_ATTR_REGPARM;
void MEMCALL memvgaf_wr16(UINT32 address, REG16 value) GCC_ATTR_REGPARM;

REG8 MEMCALL memvga0_rd8(UINT32 address);
void MEMCALL memvga0_wr8(UINT32 address, REG8 value);
REG16 MEMCALL memvga0_rd16(UINT32 address);
void MEMCALL memvga0_wr16(UINT32 address, REG16 value);

REG8 MEMCALL memvgaio_rd8(UINT32 address);
void MEMCALL memvgaio_wr8(UINT32 address, REG8 value);
REG16 MEMCALL memvgaio_rd16(UINT32 address);
void MEMCALL memvgaio_wr16(UINT32 address, REG16 value);

#ifdef __cplusplus
}
#endif

#endif

