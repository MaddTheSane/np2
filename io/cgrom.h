
typedef struct {
	UINT	code;
	UINT	lr;
	UINT	line;
} _CGROM, *CGROM;

typedef struct {
	UINT	low;
	UINT	high;
	UINT8	writable;
} _CGWINDOW, *CGWINDOW;


#ifdef __cplusplus
extern "C" {
#endif

void cgrom_reset(void);
void cgrom_bind(void);

#ifdef __cplusplus
}
#endif

