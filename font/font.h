
#define	FONTMEMORYBIND				// 520KB‚­‚ç‚¢ƒƒ‚ƒŠíœ(‚§‚¡


#ifdef __cplusplus
extern "C" {
#endif

#ifdef FONTMEMORYBIND
#define	fontrom		(mem + FONT_ADRS)
#else
extern	BYTE	__font[0x84000];
#define	fontrom		(__font)
#endif

void font_init(void);
BYTE font_load(const char *filename, BOOL force);

#ifdef __cplusplus
}
#endif

