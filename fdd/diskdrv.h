
#ifdef __cplusplus
extern "C" {
#endif

extern	int		diskdrv_delay[4];
extern	char	diskdrv_fname[4][MAX_PATH];

void diskdrv_sethdd(REG8 drv, const char *fname);

void diskdrv_setfdd(REG8 drv, const char *fname, int readonly);
void diskdrv_callback(void);

#ifdef __cplusplus
}
#endif

