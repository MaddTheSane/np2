
typedef struct {
	UINT	maxmem;
	BYTE	*pageptr[4];
	UINT	target;
	UINT32	page[4];
} _EXTMEM, *EXTMEM;


#ifdef __cplusplus
extern "C" {
#endif

void extmemmng_clear(void);
BOOL extmemmng_realloc(UINT megabytes);

BOOL extmem_init(BYTE usemem);
void extmem_reset(void);
void extmem_bind(void);

#ifdef __cplusplus
}
#endif

