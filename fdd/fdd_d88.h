
#ifdef __cplusplus
extern "C" {
#endif

BOOL fddd88_set(FDDFILE fdd, const char *fname, int ro);
BOOL fddd88_eject(FDDFILE fdd);

BOOL fdd_diskaccess_d88(void);
BOOL fdd_seek_d88(void);
BOOL fdd_seeksector_d88(void);
BOOL fdd_read_d88(void);
BOOL fdd_write_d88(void);
BOOL fdd_diagread_d88(void);
BOOL fdd_readid_d88(void);
BOOL fdd_writeid_d88(void);

BOOL fdd_formatinit_d88(void);
BOOL fdd_formating_d88(const UINT8 *ID);
BOOL fdd_isformating_d88(void);

#ifdef __cplusplus
}
#endif

