
#ifdef __cplusplus
extern "C" {
#endif

BRESULT	fdd_set_nfd(FDDFILE fdd, FDDFUNC fdd_fn, const OEMCHAR *fname, int ro);

BRESULT fdd_seeksector_nfd(FDDFILE fdd);	//	�ǉ�(kaiE)
BRESULT	fdd_read_nfd(FDDFILE fdd);
BRESULT	fdd_write_nfd(FDDFILE fdd);
BRESULT fdd_readid_nfd(FDDFILE fdd);

BRESULT fdd_seeksector_nfd1(FDDFILE fdd);	//	�ǉ�(kaiD)
BRESULT	fdd_read_nfd1(FDDFILE fdd);			//	�ǉ�(kaiD)
BRESULT	fdd_write_nfd1(FDDFILE fdd);		//	�ǉ�(kaiD)
BRESULT fdd_readid_nfd1(FDDFILE fdd);		//	�ǉ�(kaiD)

#ifdef __cplusplus
}
#endif

