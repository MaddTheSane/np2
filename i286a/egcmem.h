
#ifdef __cplusplus
extern "C" {
#endif

void egcshift(void);

REG8 MEMCALL egc_read(UINT32 addr);
void MEMCALL egc_write(UINT32 addr, REG8 value);
REG16 MEMCALL egc_read_w(UINT32 addr);
void MEMCALL egc_write_w(UINT32 addr, REG16 value);

#ifdef __cplusplus
}
#endif

