
#ifdef __cplusplus
extern "C" {
#endif

extern	int		diskdrv_delay[4];
extern	OEMCHAR	diskdrv_fname[4][MAX_PATH];

void diskdrv_sethdd(REG8 drv, const OEMCHAR *fname);

void diskdrv_readyfdd(REG8 drv, const OEMCHAR *fname, int readonly);
void diskdrv_setfdd(REG8 drv, const OEMCHAR *fname, int readonly);
void diskdrv_callback(void);

#ifdef __cplusplus
}
#endif

