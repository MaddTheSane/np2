
typedef struct {
	UINT	maxmem;
	BYTE	*pageptr[4];								// ver0.28
	UINT	target;
	UINT32	page[4];
} _EXTMEM, *EXTMEM;


#ifdef __cplusplus
extern "C" {
#endif

extern	BYTE	*extmemmng_ptr;
extern	UINT32	extmemmng_size;

void extmemmng_clear(void);
BOOL extmemmng_realloc(UINT megabytes);

BOOL extmem_init(BYTE usemem);
void extmem_reset(void);
void extmem_bind(void);

#ifdef __cplusplus
}
#endif

