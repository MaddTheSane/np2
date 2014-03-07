
enum {
	OPNCH_MAX		= 12,
	OPNA_CLOCK		= 55466 * 72,

	OPN_CHMASK		= 0x8000,
	OPN_STEREO		= 0x8000,
	OPN_MONORAL		= 0x0000
};


#if defined(OPNGENX86)

enum {
	FMDIV_BITS		= 8,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 4
};

#define SIN_BITS		11
#define EVC_BITS		10
#define ENV_BITS		16
#define KF_BITS			6
#define FREQ_BITS		20
#define ENVTBL_BIT		14
#define SINTBL_BIT		14

#elif defined(OPNGENARM)

enum {
	FMDIV_BITS		= 8,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 4
};

#define SIN_BITS		8
#define	EVC_BITS		7
#define	ENV_BITS		16
#define	KF_BITS			6
#define	FREQ_BITS		20
#define	ENVTBL_BIT		14
#define	SINTBL_BIT		14							// env+sin 30bit max

#else

enum {
	FMDIV_BITS		= 8,
	FMDIV_ENT		= (1 << FMDIV_BITS),
	FMVOL_SFTBIT	= 4
};

#define	SIN_BITS		10
#define	EVC_BITS		10
#define	ENV_BITS		16
#define	KF_BITS			6
#define	FREQ_BITS		20
#define	ENVTBL_BIT		14
#define	SINTBL_BIT		15

#endif

#define	TL_BITS			(FREQ_BITS+2)
#define	OPM_OUTSB		(TL_BITS + 2 - 16)			// OPM output 16bit

#define	SIN_ENT			(1L << SIN_BITS)
#define	EVC_ENT			(1L << EVC_BITS)		// 1 << 10 [bit] = 0x0200

#define	EC_ATTACK		0								// ATTACK start = 0x0000-0000
#define	EC_DECAY		(EVC_ENT << ENV_BITS)			// DECAY start  = 0x0200-0000
#define	EC_OFF			((2 * EVC_ENT) << ENV_BITS)		// OFF          = 0x0400-0000

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
const SINT32	*attack;			// attack ratio
const SINT32	*decay1;			// decay1 ratio
const SINT32	*decay2;			// decay2 ratio
const SINT32	*release;			// release ratio
	SINT32 		freq_cnt;			// frequency count
	SINT32		freq_inc;			// frequency step
	SINT32		multiple;			// multiple
	UINT8		keyscale;			// key scale
	UINT8		env_mode;			// envelope mode
	UINT8		envratio;			// envelope ratio
	UINT8		ssgeg1;				// SSG-EG

	SINT32		env_cnt;			// envelope count
	SINT32		env_end;			// envelope end count
	SINT32		env_inc;			// envelope step
	SINT32		env_inc_attack;		// envelope attack step
	SINT32		env_inc_decay1;		// envelope decay1 step
	SINT32		env_inc_decay2;		// envelope decay2 step
	SINT32		env_inc_release;	// envelope release step
	UINT		amon;				// AMON
} OPNSLOT;

typedef struct {
	OPNSLOT	slot[4];
	UINT8	algorithm;			// algorithm
	UINT8	feedback;			// self feedback
	UINT8	playing;
	UINT8	outslot;
	SINT32	op1fb;				// operator1 feedback
	SINT32	*connect1;			// operator1 connect
	SINT32	*connect3;			// operator3 connect
	SINT32	*connect2;			// operator2 connect
	SINT32	*connect4;			// operator4 connect
	UINT32	keynote[4];			// key note				// ver0.27

	UINT8	keyfunc[4];			// key function
	UINT8	kcode[4];			// key code
	UINT8	pan;				// pan
	UINT8	extop;				// extendopelator-enable
	UINT8	stereo;				// stereo-enable
	UINT8	padding2;			//
	UINT32	pms;				//PMS
	UINT32	ams;				//AMS
} OPNCH;

typedef struct {
	UINT	playchannels;
	UINT	playing;
	SINT32	feedback2;
	SINT32	feedback3;
	SINT32	feedback4;
	SINT32	outdl;
	SINT32	outdc;
	SINT32	outdr;
	SINT32	calcremain;
	SINT32	lfo_freq_cnt;			// frequency count
	SINT32	lfo_freq_inc;			// frequency step
	UINT	lfo_enable;
	UINT8	keyreg[OPNCH_MAX];
} _OPNGEN, *OPNGEN;

typedef struct {
	SINT32	calc1024;
	SINT32	fmvol;
	UINT	ratebit;
	UINT	vr_en;
	SINT32	vr_l;
	SINT32	vr_r;

	SINT32	envcurve[EVC_ENT*2 + 1];
	SINT32	sintable[SIN_ENT];
	SINT32	envtable[EVC_ENT];
#ifdef OPNGENX86
	char	sinshift[SIN_ENT];
	char	envshift[EVC_ENT];
#endif
} OPNCFG;


#ifdef __cplusplus
extern "C" {
#endif

void opngen_initialize(UINT rate);
void opngen_setvol(UINT vol);
void opngen_setVR(REG8 channel, REG8 value);

void opngen_reset(void);
void opngen_setcfg(REG8 maxch, UINT flag);
void opngen_setextch(UINT chnum, REG8 data);
void opngen_setreg(REG8 chbase, UINT reg, REG8 value);
void opngen_keyon(UINT chnum, REG8 value);

void SOUNDCALL opngen_getpcm(void *hdl, SINT32 *buf, UINT count);
void SOUNDCALL opngen_getpcmvr(void *hdl, SINT32 *buf, UINT count);

#ifdef __cplusplus
}
#endif

