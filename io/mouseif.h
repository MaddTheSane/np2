
typedef struct {
	UINT32	lastc;
	UINT	rapidclock;
	SINT16	x;
	SINT16	y;
	SINT16	rx;
	SINT16	ry;
	SINT16	sx;
	SINT16	sy;
	SINT16	latch_x;
	SINT16	latch_y;
	BYTE	portc;
	BYTE	timing;
	BYTE	mode;
	BYTE	rapid;
	BYTE	b;
} _MOUSEIF, *MOUSEIF;


#ifdef __cplusplus
extern "C" {
#endif

void mouseif_reset(void);
void mouseif_bind(void);
void mouseif_sync(void);
void mouseint(NEVENTITEM item);

#ifdef __cplusplus
}
#endif

