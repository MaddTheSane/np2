
#ifdef __cplusplus
extern "C" {
#endif

void egcshift(void);

BYTE MEMCALL egc_read(UINT32 addr);
void MEMCALL egc_write(UINT32 addr, BYTE value);
UINT16 MEMCALL egc_read_w(UINT32 addr);
void MEMCALL egc_write_w(UINT32 addr, UINT16 value);

#ifdef __cplusplus
}
#endif

