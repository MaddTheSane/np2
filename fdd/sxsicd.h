
#ifdef __cplusplus
extern "C" {
#endif

enum {
	SXSIMEDIA_DATA = 0x10,
	SXSIMEDIA_AUDIO = 0x20
};

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *fname);
UINT sxsicd_gettocinfo(SXSIDEV sxsi, UINT8 *buf);

#ifdef __cplusplus
}
#endif

