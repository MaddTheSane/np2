
// PC-9821 PCIƒuƒŠƒbƒW

#if defined(SUPPORT_PC9821)

#ifdef __cplusplus
extern "C" {
#endif

void IOOUTCALL pcidev_w32(UINT port, UINT32 value);
UINT32 IOOUTCALL pcidev_r32(UINT port);

void pcidev_reset(void);
void pcidev_bind(void);

#ifdef __cplusplus
}
#endif

#endif

