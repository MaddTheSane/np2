
#define	FONTMEMORYBIND				// 520KB���炢�������폜(����


#ifdef __cplusplus
extern "C" {
#endif

#ifdef FONTMEMORYBIND
#define	font	(mem + FONT_ADRS)
#else
extern	BYTE	__font[0x84000];
#define	font	(__font)
#endif

void font_init(void);
BYTE font_load(const char *filename, BOOL force);

#ifdef __cplusplus
}
#endif

