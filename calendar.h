
typedef struct {
	_SYSTIME	dt;
	_SYSTIME	realc;
	UINT		steps;
	UINT		realchg;
} _CALENDAR, *CALENDAR;


#ifdef __cplusplus
extern "C" {
#endif

extern	_CALENDAR	cal;

void calendar_init(void);
void calendar_inc(void);
void calendar_set(const BYTE *bcd);
void calendar_get(BYTE *bcd);
void calendar_getreal(BYTE *bcd);
void calendar_getvir(BYTE *bcd);

#ifdef __cplusplus
}
#endif

