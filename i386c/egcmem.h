
#ifdef __cplusplus
extern "C" {
#endif

void egcshift(void);

REG8 MEMCALL egc_read(UINT32 addr) GCC_ATTR_REGPARM;
void MEMCALL egc_write(UINT32 addr, REG8 value) GCC_ATTR_REGPARM;
REG16 MEMCALL egc_read_w(UINT32 addr) GCC_ATTR_REGPARM;
void MEMCALL egc_write_w(UINT32 addr, REG16 value) GCC_ATTR_REGPARM;

#ifdef __cplusplus
}
#endif

