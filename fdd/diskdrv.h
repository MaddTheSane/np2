
#ifdef __cplusplus
extern "C" {
#endif

// extern	int		diskdrv_delay[4];
// extern	OEMCHAR	diskdrv_fname[4][MAX_PATH];

void diskdrv_sethdd(REG8 drv, const OEMCHAR *fname);

void diskdrv_readyfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly);
void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly);
void diskdrv_callback(void);

#define diskdrv_readyfdd(d, f, r)	diskdrv_readyfddex(d, f, FTYPE_NONE, r)
#define diskdrv_setfdd(d, f, r)		diskdrv_setfddex(d, f, FTYPE_NONE, r)

#ifdef __cplusplus
}
#endif

