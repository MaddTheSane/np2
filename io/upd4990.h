
#define UPD4990_REGLEN	8

typedef struct {
	BYTE	last;
	BYTE	cmd;
	BYTE	serial;
	BYTE	parallel;
	BYTE	reg[UPD4990_REGLEN];
	UINT	pos;
	BYTE	cdat;
	BYTE	regsft;
} _UPD4990, *UPD4990;


#ifdef __cplusplus
extern "C" {
#endif

void uPD4990_reset(void);
void uPD4990_bind(void);

#ifdef __cplusplus
}
#endif

