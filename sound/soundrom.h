
typedef struct {
	char	name[24];
	UINT32	address;
} SOUNDROM;


#ifdef __cplusplus
extern "C" {
#endif

extern	SOUNDROM	soundrom;

void soundrom_reset(void);
void soundrom_load(UINT32 address, const char *primary);
void soundrom_loadex(BYTE sw, const char *primary);

#ifdef __cplusplus
}
#endif

