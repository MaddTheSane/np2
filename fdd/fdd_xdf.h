
#ifdef __cplusplus
extern "C" {
#endif

BOOL fddxdf_set(FDDFILE fdd, const char *fname, int ro);
BOOL fddxdf_setfdi(FDDFILE fdd, const char *fname, int ro);
BOOL fddxdf_eject(FDDFILE fdd);

BOOL fddxdf_diskaccess(FDDFILE fdd);
BOOL fddxdf_seek(FDDFILE fdd);
BOOL fddxdf_seeksector(FDDFILE fdd);
BOOL fddxdf_read(FDDFILE fdd);
BOOL fddxdf_write(FDDFILE fdd);
BOOL fddxdf_readid(FDDFILE fdd);

#ifdef __cplusplus
}
#endif

