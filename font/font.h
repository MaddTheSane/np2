
#define	FONTMEMORYBIND				// 520KB���炢�������폜(����


#ifdef __cplusplus
extern "C" {
#endif

#ifdef FONTMEMORYBIND
#define	fontrom		(mem + FONT_ADRS)
#else
extern	UINT8	__font[0x84000];
#define	fontrom		(__font)
#endif

void font_initialize(void);
UINT8 font_load(const char *filename, BOOL force);

#ifdef __cplusplus
}
#endif

