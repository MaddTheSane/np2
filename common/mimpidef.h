
enum {
	MIMPI_LA		= 0,
	MIMPI_PCM,
	MIMPI_GS,
	MIMPI_RHYTHM
};

typedef struct {
	BYTE	ch[16];
	BYTE	map[3][128];
	BYTE	bank[3][128];
} MIMPIDEF;


#ifdef __cplusplus
extern "C" {
#endif

BOOL mimpidef_load(MIMPIDEF *def, const char *filename);

#ifdef __cplusplus
}
#endif

