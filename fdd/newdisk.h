
#ifdef __cplusplus
extern "C" {
#endif

void newdisk_fdd(const char *fname, REG8 type, const char *label);

void newdisk_thd(const char *fname, UINT hddsize);
void newdisk_hdi(const char *fname, UINT hddtype);
void newdisk_vhd(const char *fname, UINT hddsize);
void newdisk_nhd(const char *fname, UINT hddsize);

#ifdef __cplusplus
}
#endif

