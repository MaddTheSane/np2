
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

typedef void (*GDCSUBFN)(UINT32 csrw, const GDCVECT *vect,
														REG16 pat, REG8 ope);


#ifdef __cplusplus
extern "C" {
#endif

void gdcslavewait(NEVENTITEM item);

void gdcsub_init(void);
void gdcsub_null(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_line(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_box(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_circle(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);
void gdcsub_txt(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope);

void gdcsub_text(UINT32 csrw, const GDCVECT *vect, const BYTE *pat, REG8 ope);
void gdcsub_write(void);

#ifdef __cplusplus
}
#endif

