
#ifdef __cplusplus
extern "C" {
#endif

extern	int		fddmtr_biosbusy;					// ver0.26

void fdbiosout(NEVENTITEM item);

void fddmtr_init(void);
void fddmtr_callback(UINT time);
void fddmtr_seek(BYTE drv, BYTE c, UINT size);		// ver0.26

#ifdef __cplusplus
}
#endif

