
enum {
	OPNCH_MAX		= 12,
	OPNA_CLOCK		= 3993600,

	OPN_CHMASK		= 0x8000,
	OPN_STEREO		= 0x8000,
	OPN_MONORAL		= 0x0000
};


#ifdef OPNGENX86

enum {
	FMDIV_BITS		= 8,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 4,

	SIN_BITS		= 11,
	EVC_BITS		= 10,
	ENV_BITS		= 16,
	KF_BITS			= 6,
	FREQ_BITS		= 20,
	ENVTBL_BIT		= 14,
	SINTBL_BIT		= 14
};

#else

enum {
	FMDIV_BITS		= 8,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 4,						// C”Å‚Í–³Ž‹

	SIN_BITS		= 10,
	EVC_BITS		= 10,
	ENV_BITS		= 16,
	KF_BITS			= 6,
	FREQ_BITS		= 20,
	ENVTBL_BIT		= 14,
	SINTBL_BIT		= 15							// env+sin 30bit max
};

#endif

#define	TL_BITS			(FREQ_BITS+2)
#define	OPM_OUTSB		(TL_BITS + 2 - 16)			// OPM output 16bit

#define	SIN_ENT			(1L << SIN_BITS)
#define	EVC_ENT			(1L << EVC_BITS)

#define	EC_ATTACK		0								// ATTACK start
#define	EC_DECAY		(EVC_ENT << ENV_BITS)			// DECAY start
#define	EC_OFF			((2 * EVC_ENT) << ENV_BITS)		// OFF

#define	TL_MAX			(EVC_ENT * 2)

enum {
	OPNSLOT1		= 0,				// slot number
	OPNSLOT2		= 2,
	OPNSLOT3		= 1,
	OPNSLOT4		= 3,

	EM_ATTACK		= 4,
	EM_DECAY1		= 3,
	EM_DECAY2		= 2,
	EM_RELEASE		= 1,
	EM_OFF			= 0
};

typedef struct {
	SINT32		*detune1;			// detune1
	SINT32		totallevel;			// total level
	SINT32		decaylevel;			// decay level
const SINT32	*attack;			// attack raito
const SINT32	*decay1;			// decay1 raito
const SINT32	*decay2;			// decay2 raito
const SINT32	*release;			// release raito
	SINT32 		freq_cnt;			// frequency count
	SINT32		freq_inc;			// frequency step
	SINT32		multiple;			// multiple
	BYTE		keyscale;			// key scale
	BYTE		env_mode;			// envelope mode
	BYTE		envraito;			// envelope raito
	BYTE		ssgeg1;				// SSG-EG

	SINT32		env_cnt;			// envelope count
	SINT32		env_end;			// envelope end count
	SINT32		env_inc;			// envelope step
	SINT32		env_inc_attack;		// envelope attack step
	SINT32		env_inc_decay1;		// envelope decay1 step
	SINT32		env_inc_decay2;		// envelope decay2 step
	SINT32		env_inc_release;	// envelope release step
} OPNSLOT;

typedef struct {
	OPNSLOT	slot[4];
	BYTE	algorithm;			// algorithm
	BYTE	feedback;			// self feedback
	BYTE	padding1[2];
	SINT32	op1fb;				// operator1 feedback
	SINT32	*connect1;			// operator1 connect
	SINT32	*connect3;			// operator3 connect
	SINT32	*connect2;			// operator2 connect
	SINT32	*connect4;			// operator4 connect
	UINT32	keynote[4];			// key note				// ver0.27

	BYTE	keyfunc[4];			// key function
	BYTE	kcode[4];			// key code
	BYTE	pan;				// pan
	BYTE	extop;				// extendopelator-enable
	BYTE	stereo;				// stereo-enable
	BYTE	padding2;
} OPNCH;

typedef struct {
	UINT	playchannels;
	SINT32	feedback2;
	SINT32	feedback3;
	SINT32	feedback4;
	SINT32	outdl;
	SINT32	outdc;
	SINT32	outdr;
	SINT32	calcremain;
	BYTE	keyreg[OPNCH_MAX];
} _OPNGEN, *OPNGEN;

typedef struct {
	SINT32	calc1024;
	SINT32	fmvol;
	UINT	ratebit;
	UINT	vr_en;
	SINT32	vr_l;
	SINT32	vr_r;
} OPNCFG;


#ifdef __cplusplus
extern "C" {
#endif

void opngen_initialize(UINT rate);
void opngen_setvol(UINT vol);
void opngen_setVR(BYTE channel, BYTE value);

void opngen_reset(void);
void opngen_setcfg(BYTE maxch, UINT flag);
void opngen_setextch(UINT chnum, BYTE data);
void opngen_setreg(BYTE chbase, BYTE reg, BYTE value);
void opngen_keyon(UINT chnum, BYTE value);

void SOUNDCALL opngen_getpcm(void *hdl, SINT32 *buf, UINT count);
void SOUNDCALL opngen_getpcmvr(void *hdl, SINT32 *buf, UINT count);

#ifdef __cplusplus
}
#endif

