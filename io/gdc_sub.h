
enum {
	GDCOPE_REPLACE		= 0,
	GDCOPE_COMPLEMENT	= 1,
	GDCOPE_CLEAR		= 2,
	GDCOPE_SET			= 3
};

typedef struct {
	BYTE	ope;
	BYTE	DC[2];
	BYTE	D[2];
	BYTE	D2[2];
	BYTE	D1[2];
	BYTE	DM[2];
} GDCVECT;

extern const UINT32 gdcplaneseg[4];

typedef void (*GDCSUBFN)(UINT32 csrw, const GDCVECT *vect,
														REG16 pat, REG8 ope);


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
extern const UINT8 gdcbitreverse[0x100];
#define	GDCPATREVERSE(d)		gdcbitreverse[(d) & 0xff]
#else
REG8 gdcbitreverse(REG8 data);
#define	GDCPATREVERSE(d)		gdcbitreverse((REG8)(d))
#endif

void gdcslavewait(NEVENTITEM item);

void gdcsub_initialize(void);
void gdcsub_setslavewait(UINT32 clock);

void gdcsub_vect0(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_vectl(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_vectt(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_vectc(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_vectr(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);

void gdcsub_text(UINT32 csrw, const GDCVECT *vect, const BYTE *pat, REG8 ope);
void gdcsub_write(void);

#ifdef __cplusplus
}
#endif

