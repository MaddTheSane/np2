

#ifdef __cplusplus
extern "C" {
#endif

void cs4231io_reset(void);
void cs4231io_bind(void);

void IOOUTCALL cs4231io_w8(UINT port, REG8 value);
REG8 IOINPCALL cs4231io_r8(UINT port);

#ifdef __cplusplus
}
#endif

