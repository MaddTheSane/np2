
#ifdef __cplusplus
extern "C" {
#endif

void egcshift(void);

REG8 MEMCALL memegc_rd8(UINT32 addr) GCC_ATTR_REGPARM;
void MEMCALL memegc_wr8(UINT32 addr, REG8 value) GCC_ATTR_REGPARM;
REG16 MEMCALL memegc_rd16(UINT32 addr) GCC_ATTR_REGPARM;
void MEMCALL memegc_wr16(UINT32 addr, REG16 value) GCC_ATTR_REGPARM;

#ifdef __cplusplus
}
#endif

