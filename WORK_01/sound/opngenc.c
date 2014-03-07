#include	"compiler.h"
#include	<math.h>
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"keydisp.h"


#define	OPM_ARRATE		 399128L
#define	OPM_DRRATE		5514396L

#define	EG_STEP	(96.0 / EVC_ENT)					// dB step �c (96.0[dB] / 0x0200)
#define	SC(db)	(SINT32)((db) * ((3.0 / EG_STEP) * (1 << ENV_BITS))) + EC_DECAY
#define	D2(v)	(((double)(6 << KF_BITS) * log((double)(v)) / log(2.0)) + 0.5)
#define	FMASMSHIFT	(32 - 6 - (OPM_OUTSB + 1 + FMDIV_BITS) + FMVOL_SFTBIT)
#define	FREQBASE4096	((double)OPNA_CLOCK / calcrate / 64)


	OPNCFG	opncfg;
//����́A�P���L���b�V���̌������グ�邽�߂ɁAOPNCFG�\���̂̒��ɂ���B
//#ifdef OPNGENX86
//	char	envshift[EVC_ENT];
//	char	sinshift[SIN_ENT];
//#endif


static	SINT32	detunetable[8][32];
static	SINT32	attacktable[94];
static	SINT32	decaytable[94];

static const SINT32	decayleveltable[16] = {
		 			SC( 0),SC( 1),SC( 2),SC( 3),SC( 4),SC( 5),SC( 6),SC( 7),
		 			SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)};
static const UINT32	multipletable[] = {									//32bit�̔z��ɂ��������y�i���e�B�[�������B
			    	1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
static const SINT32 nulltable[] = {
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const UINT32 kftable[16] = {0,0,0,0,0,0,0,1,2,3,3,3,3,3,3,3};	//32bit�̔z��ɂ��������y�i���e�B�[�������B
static const UINT32	dttable[] = {										//32bit�̔z��ɂ��������y�i���e�B�[�������B
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
					2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
					1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
					5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
					2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
					8, 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22};
static const int extendslot[4] = {2, 3, 1, 0};
static const int fmslot[4] = {0, 2, 1, 3};

//�n�[�h�E�F�ALFO�֘A�̃e�[�u��
#if (EVC_BITS >= 7)
#define	LA(n)	(SINT32)((n/0.75)*(1<<(EVC_BITS - 7)))/2
#else
#define	LA(n)	(SINT32)((n/0.75)*(1<<(7 - EVC_BITS)))/2
#endif
#define	LP(n)	(SINT32)
       const double lfo_freq[8]			= {3.98, 5.56, 6.02, 6.37, 6.88, 9.63, 48.1, 72.2};	//�P�� [Hz]
       		 SINT32 lfo_freq_table[8]	= {0,0,0,0,0,0,0,0};
	   const SINT32 lfo_pms_table[8]	= {
				0x00000000/2,		// 0 [cent]
				0x0080D56F/2,		// 3.4 [cent]	table = (1-(2^(n[cent]/1200))) * 2^32
				0x00FE1ED5/2,		// 6.7 [cent]
				0x017BA56C/2,		//10 [cent]
				0x02141EA7/2,		//14 [cent]
				0x02F97DDC/2,		//20 [cent]
				0x05FBD4D5/2,		//40 [cent]
				0x0C1B77B6/2};	//80 [cent]	MUL freq_inc,���̒l �����s���āAedx�̐��l�𑫂��B
       const SINT32 lfo_ams_table[4]	= {LA(0),LA(1.4),LA(5.9),LA(11.8)};	//[dB]

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_initialize(UINT rate) {

	//------------------
	//��Local
	UINT	ratebit;
	int		i;
	int		j;
	double	pom;
	double	freq;
	UINT32	calcrate;

	//------------------
	//��
	if (rate == 44100) {
		ratebit = 0;
	}
	else if (rate == 22050) {
		ratebit = 1;
	}
	else {
		ratebit = 2;
	}
	calcrate = (OPNA_CLOCK / 72) >> ratebit;
	opncfg.calc1024 = FMDIV_ENT * 44100 / (OPNA_CLOCK / 72);

	//------------------
	//��LOG Table
	for (i=0; i<EVC_ENT; i++) {
#ifdef OPNGENX86
		char sft;
		sft = ENVTBL_BIT;
		while(sft < (ENVTBL_BIT + 8)) {
			pom = (double)(1 << sft) / pow(10.0, EG_STEP*(EVC_ENT-i)/20.0);
			opncfg.envtable[i] = (long)pom;
			opncfg.envshift[i] = sft - TL_BITS;
			if (opncfg.envtable[i] >= (1 << (ENVTBL_BIT - 1))) {
				break;
			}
			sft++;
		}
#else
		pom = (double)(1 << ENVTBL_BIT) / pow(10.0, EG_STEP*(EVC_ENT-i)/20.0);
		opncfg.envtable[i] = (long)pom;
#endif
	}

	//------------------
	//��Sin wave Table
	for (i=0; i<SIN_ENT; i++) {
#ifdef OPNGENX86
		char sft;
		sft = SINTBL_BIT;
		while(sft < (SINTBL_BIT + 8)) {
			pom = (double)(1 << sft) * sin(2*PI*i/SIN_ENT);
			opncfg.sintable[i] = (long)pom;
			opncfg.sinshift[i] = sft;
			if (opncfg.sintable[i] >= (1 << (SINTBL_BIT - 1))) {
				break;
			}
			if (opncfg.sintable[i] <= -1 * (1 << (SINTBL_BIT - 1))) {
				break;
			}
			sft++;
		}
#else
		pom = (double)((1 << SINTBL_BIT) - 1) * sin(2*PI*i/SIN_ENT);
		opncfg.sintable[i] = (long)pom;
#endif
	}

	//------------------
	//��Envlop table
	for (i=0; i<EVC_ENT; i++) {				// 0x0000 �` 0x01FF
		pom = pow(((double)(EVC_ENT-1-i)/EVC_ENT), 8) * EVC_ENT;
		opncfg.envcurve[i] = (long)pom;								// 0x0000�`0x03FF : ((1023-i/1024)^8) * 1024
		opncfg.envcurve[EVC_ENT + i] = i;							// 0x0400�`0x07FF : i
	}
	opncfg.envcurve[EVC_ENT*2] = EVC_ENT;							// 0x0800         : 1024

	//------------------
	//��
//	opmbaserate = (1L << FREQ_BITS) / (rate * x / 44100) * 55466;
//	�ł����� x == 55466������c

//	������ FREQ_BITS >= 16������
	if (rate == 44100) {
		opncfg.ratebit = 0 + (FREQ_BITS - 16);
	}
	else if (rate == 22050) {
		opncfg.ratebit = 1 + (FREQ_BITS - 16);
	}
	else {
		opncfg.ratebit = 2 + (FREQ_BITS - 16);
	}

	//�n�[�h�E�F�ALFO��freq�p�e�[�u���̍쐬
	for (i=0; i<8; i++) {
		lfo_freq_table[i] = (SINT32)(((double)SIN_ENT / (((double)OPNA_CLOCK / 72) / lfo_freq[i] )) * (double)(1 << (opncfg.ratebit + 16 - SIN_BITS)));
	}
	//------------------
	//��
	for (i=0; i<4; i++) {
		for (j=0; j<32; j++) {
#if (FREQ_BITS >= 21)
			freq = FREQBASE4096 * dttable[i*32 + j] *
											(1 << (FREQ_BITS-21));
#else
			freq = FREQBASE4096 * dttable[i*32 + j] /
											(1 << (21-FREQ_BITS));
#endif
			detunetable[i][j]   = (long)freq;
			detunetable[i+4][j] = (long)-freq;
		}
	}

	//------------------
	//��
	for (i=0; i<4; i++) {
		attacktable[i] = decaytable[i] = 0;
	}
	for (i=4; i<64; i++) {
		freq = (double)(EVC_ENT << ENV_BITS) * FREQBASE4096;
		if (i < 8) {							// �Y��Ă܂��B
			freq *= 1.0 + (i & 2) * 0.25;
		}
		else if (i < 60) {
			freq *= 1.0 + (i & 3) * 0.25;
		}
		freq *= (double)(1 << ((i >> 2) - 1));
#if 0
		attacktable[i] = (long)((freq + OPM_ARRATE - 1) / OPM_ARRATE);
		decaytable[i] = (long)((freq + OPM_DRRATE - 1) / OPM_DRRATE);
#else
		attacktable[i] = (long)(freq / OPM_ARRATE);
		decaytable[i] = (long)(freq / OPM_DRRATE);
#endif
		if (attacktable[i] >= EC_DECAY) {
			TRACEOUT(("attacktable %d %d %d", i, attacktable[i], EC_DECAY));
		}
		if (decaytable[i] >= EC_DECAY) {
			TRACEOUT(("decaytable %d %d %d", i, decaytable[i], EC_DECAY));
		}
	}
	attacktable[62] = EC_DECAY - 1;
	attacktable[63] = EC_DECAY - 1;
	for (i=64; i<94; i++) {
		attacktable[i] = attacktable[63];
		decaytable[i] = decaytable[63];
	}
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_setvol(UINT vol) {

	opncfg.fmvol = vol * 5 / 4;
#if defined(OPNGENX86)
	opncfg.fmvol <<= FMASMSHIFT;
#endif
}

//==============================================================
//			OPN	Pan
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_setVR(REG8 channel, REG8 value) {

	if ((channel & 3) && (value)) {
		opncfg.vr_en = TRUE;
		opncfg.vr_l = (channel & 1)?value:0;
		opncfg.vr_r = (channel & 2)?value:0;
	}
	else {
		opncfg.vr_en = FALSE;
	}
}


// ----

//==============================================================
//			�G���x���[�v�`��̔��f
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void	Envlop_update(OPNSLOT *slot)
{
	SINT32	iRate;

	switch(slot->env_mode){
		case(EM_ATTACK):	//AR���ɑ���
			iRate = slot->env_inc_attack;
			break;
		case(EM_DECAY1):	//DR���ɑ����H
		case(EM_DECAY2):	//SR���ɑ����H
			if(slot->env_cnt < slot->decaylevel){
				slot->env_mode	= EM_DECAY1;
				slot->env_end	= slot->decaylevel;
				iRate			= slot->env_inc_decay1;
			} else {
				slot->env_mode	= EM_DECAY2;
				slot->env_end	= EC_OFF;
				iRate			= slot->env_inc_decay2;
			}
			break;
		case(EM_RELEASE):	//RR���ɑ���
			iRate = slot->env_inc_release;
			break;
		default:			//EM_OFF
			iRate = 0;
			break;
	}
	slot->env_inc = iRate;		//�����œ��ꂽ�����A�œK�����Ă���Ă���B
}

//==============================================================
//			KeyScale�Ɖ����ɂ��A�G���x���[�v�`��̍X�V
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			Key Scale���ɂ��A�G���x���[�v�`����X�V����B
//==============================================================
static void channleupdate(OPNCH *ch) {

	int		i;
	int		s;
	UINT	evr;
	OPNSLOT	*slot = ch->slot;
	UINT8	kc;

	for (i=0; i<4; i++, slot++) {
		s = ((ch->extop)? extendslot[i] : 0);
		kc = ch->kcode[s];
		slot->freq_inc = (ch->keynote[s] + slot->detune1[kc]) * slot->multiple;
		evr = kc >> slot->keyscale;
		if (slot->envratio != evr) {
			slot->envratio = evr;
			slot->env_inc_attack = slot->attack[evr];
			slot->env_inc_decay1 = slot->decay1[evr];
			slot->env_inc_decay2 = ((slot->ssgeg1)? 0 : slot->decay2[evr] );
			slot->env_inc_release = slot->release[evr];
			Envlop_update(slot);	//�X�V����������A�G���x���[�v���X�V
		}
	}
}

//==============================================================
//			OPN	AL
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_algorithm(OPNCH *ch) {

	SINT32	*outd;			//�o�͐�
	SINT32	*cn1;			
	SINT32	*cn2;			
	SINT32	*cn3;			//��U�A�ϐ��ɓ��ꂽ�ق����A���̗ǂ��@�B��𐶐�����݂����B
	UINT8	outslot;

	if (ch->stereo) {
		switch(ch->pan & 0xc0) {
			case 0x80:
				outd = &opngen.outdl;
				break;

			case 0x40:
				outd = &opngen.outdr;
				break;
			default:
				outd = &opngen.outdc;
				break;
		}
	} else {
		outd = &opngen.outdc;
	}

	switch(ch->algorithm) {
		case 0:		// [1] - [2] - [3] - [4]
			cn1 = &opngen.feedback2;
			cn2 = &opngen.feedback3;
			cn3 = &opngen.feedback4;
			outslot = 0x08;
			break;

		case 1:		// ([1] + [2]) - [3] - [4]
			cn1 = &opngen.feedback3;
			cn2 = &opngen.feedback3;
			cn3 = &opngen.feedback4;
			outslot = 0x08;
			break;

		case 2:		//	([1] + ([2] - [3])) - [4]
			cn1 = &opngen.feedback4;
			cn2 = &opngen.feedback3;
			cn3 = &opngen.feedback4;
			outslot = 0x08;
			break;

		case 3:		//	(([1] - [2]) + (3)] - [4]
			cn1 = &opngen.feedback2;
			cn2 = &opngen.feedback4;
			cn3 = &opngen.feedback4;
			outslot = 0x08;
			break;

		case 4:		//	(([1] - [2]) + ([3] - [4]))
			cn1 = &opngen.feedback2;
			cn2 = outd;
			cn3 = &opngen.feedback4;
			outslot = 0x0a;
			break;

		case 5:		//	[1] - ([2] + [3] + [4])
			cn1 = 0;
			cn2 = outd;
			cn3 = outd;
			outslot = 0x0e;
			break;

		case 6:		// ([1] - [2]) + [3] + [4]
			cn1 = &opngen.feedback2;
			cn2 = outd;
			cn3 = outd;
			outslot = 0x0e;
			break;

		case 7:		//	[1] + [2] + [3] + [4]
		default:
			cn1 = outd;
			cn2 = outd;
			cn3 = outd;
			outslot = 0x0f;
	}

	//�����ŁA�ꊇ�œ����B
	//�ȒP�ȃI�y�R�[�h�𐶐������邷�鎖�ŁA�v���t�F�b�`�̃f�R�[�_�[�ւ̃X���[�v�b�g���グ��B
	ch->connect1 = cn1;
	ch->connect2 = cn2;
	ch->connect3 = cn3;
	ch->connect4 = outd;
	ch->outslot = outslot;
}

//==============================================================
//			OPN	DT & ML
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_dt1_mul(OPNSLOT *slot, REG8 value) {

	slot->multiple = (SINT32)multipletable[value & 0x0f];
	slot->detune1 = detunetable[(value >> 4) & 7];
}

//==============================================================
//			OPN	TL
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_tl(OPNSLOT *slot, REG8 value) {

#if (EVC_BITS >= 7)
	slot->totallevel = ((~value) & 0x007f) << (EVC_BITS - 7);
#else
	slot->totallevel = ((~value) & 0x007f) >> (7 - EVC_BITS);
#endif
}

//==============================================================
//			OPN KS & AL
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_ks_ar(OPNSLOT *slot, REG8 value) {

	slot->keyscale = ((~value) >> 6) & 3;
	value &= 0x1f;
	slot->attack = (value)?(attacktable + (value << 1)):nulltable;
	slot->env_inc_attack = slot->attack[slot->envratio];
	if (slot->env_mode == EM_ATTACK) {
		slot->env_inc = slot->env_inc_attack;
	}
}

//==============================================================
//			OPN	DR
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_d1r(OPNSLOT *slot, REG8 value) {

	slot->amon = (value & 0x80) >> 7;
	value &= 0x1f;
	slot->decay1 = (value)?(decaytable + (value << 1)):nulltable;
	slot->env_inc_decay1 = slot->decay1[slot->envratio];
	if (slot->env_mode == EM_DECAY1) {
		slot->env_inc = slot->env_inc_decay1;
	}
}

//==============================================================
//			OPN	DT & SR
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_dt2_d2r(OPNSLOT *slot, REG8 value) {

	value &= 0x1f;
	slot->decay2 = (value)?(decaytable + (value << 1)):nulltable;
	slot->env_inc_decay2 = ((slot->ssgeg1==1)? 0 : slot->decay2[slot->envratio] );
	if (slot->env_mode == EM_DECAY2) {
		slot->env_inc = slot->env_inc_decay2;
	}
}
//==============================================================
//			OPN	DL & RR
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_d1l_rr(OPNSLOT *slot, REG8 value) {

	slot->decaylevel = decayleveltable[(value >> 4)];
	slot->release = decaytable + ((value & 0x0f) << 2) + 2;
	slot->env_inc_release = slot->release[slot->envratio];
	Envlop_update(slot);	//SL, RR�̕ύX�Ȃ̂ŁA���������g���B
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
static void set_ssgeg(OPNSLOT *slot, REG8 value) {

	value &= 0xf;
	if ((value == 0xb) || (value == 0xd)) {
		slot->ssgeg1 = 1;
		slot->env_inc_decay2 = 0;
	}
	else {
		slot->ssgeg1 = 0;
		slot->env_inc_decay2 = slot->decay2[slot->envratio];
	}
	if (slot->env_mode == EM_DECAY2) {
		slot->env_inc = slot->env_inc_decay2;
	}
}

// ----

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_reset(void) {

	OPNCH	*ch;
	UINT	i;
	OPNSLOT	*slot;
	UINT	j;

	ZeroMemory(&opngen, sizeof(opngen));
	ZeroMemory(opnch, sizeof(opnch));
	opngen.playchannels	= 3;

	ch = opnch;
	for (i=0; i<OPNCH_MAX; i++) {
		ch->keynote[0] = 0;
		slot = ch->slot;
		for (j=0; j<4; j++) {
			slot->env_mode = EM_OFF;
			slot->env_cnt = EC_OFF;
			slot->env_end = EC_OFF + 1;
			slot->env_inc = 0;
			slot->detune1 = detunetable[0];
			slot->attack = nulltable;
			slot->decay1 = nulltable;
			slot->decay2 = nulltable;
			slot->release = decaytable;
			slot++;
		}
		ch++;
	}
	for (i=0x30; i<0xc0; i++) {
		opngen_setreg(0, i, 0xff);
		opngen_setreg(3, i, 0xff);
		opngen_setreg(6, i, 0xff);
		opngen_setreg(9, i, 0xff);
	}
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_setcfg(REG8 maxch, UINT flag) {

	OPNCH	*ch;
	UINT	i;

	opngen.playchannels = maxch;
	ch = opnch;
	if ((flag & OPN_CHMASK) == OPN_STEREO) {
		for (i=0; i<OPNCH_MAX; i++) {
			if (flag & (1 << i)) {
				ch->stereo = TRUE;
				set_algorithm(ch);
			}
			ch++;
		}
	}
	else {
		for (i=0; i<OPNCH_MAX; i++) {
			if (flag & (1 << i)) {
				ch->stereo = FALSE;
				set_algorithm(ch);
			}
			ch++;
		}
	}
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_setextch(UINT chnum, REG8 data) {

	OPNCH	*ch;

	ch = opnch;
	ch[chnum].extop = data;
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_setreg(REG8 chbase, UINT reg, REG8 value) {

	UINT	chpos;
	OPNCH	*ch;
	OPNSLOT	*slot;
	UINT	fn;
	UINT8	blk;
	UINT	i;

	chpos = reg & 3;
	if (chpos == 3) {
		return;
	}
	sound_sync();
	ch = opnch + chbase + chpos;
	if (reg == 0x22) {					// LFO ON & Freq
		if (ch->stereo) {
			opngen.lfo_enable	= (value & 0x08) >> 3;
			opngen.lfo_freq_inc	= lfo_freq_table[value & 0x07];
			opngen.lfo_freq_cnt	= 0;
		}
	} else if (reg < 0xa0) {
		slot = ch->slot + fmslot[(reg >> 2) & 3];
		switch(reg & 0xf0) {
			case 0x30:					// DT1 & MUL
				set_dt1_mul(slot, value);
				channleupdate(ch);
				break;

			case 0x40:					// TL
				set_tl(slot, value);
				break;

			case 0x50:					// KS & AR
				set_ks_ar(slot, value);
				channleupdate(ch);
				break;

			case 0x60:					// AMON & DR
				set_d1r(slot, value);
				break;

			case 0x70:					// DT2 SR
				set_dt2_d2r(slot, value);
				channleupdate(ch);
				break;

			case 0x80:					// SL RR
				set_d1l_rr(slot, value);
				break;

			case 0x90:					// SSG-EG
				set_ssgeg(slot, value);
				channleupdate(ch);
				break;
		}
	} else {
		switch(reg & 0xfc) {
			case 0xa0:					// F-num 1
				blk = ch->keyfunc[0] >> 3;
				fn = ((ch->keyfunc[0] & 7) << 8) + value;
				ch->kcode[0] = (blk << 2) | kftable[fn >> 7];
//				ch->keynote[0] = fn * opmbaserate / (1L << (22-blk));
				ch->keynote[0] = (fn << (opncfg.ratebit + blk)) >> 6;
				channleupdate(ch);		//Key Scale �ɂ��G���x���[�v�`��̍X�V
				break;

			case 0xa4:					// Block & F-num 2
				ch->keyfunc[0] = value & 0x3f;
				break;

			case 0xa8:					// Ch3 F-num 1
				ch = opnch + chbase + 2;
				blk = ch->keyfunc[chpos+1] >> 3;
				fn = ((ch->keyfunc[chpos+1] & 7) << 8) + value;
				ch->kcode[chpos+1] = (blk << 2) | kftable[fn >> 7];
//				ch->keynote[chpos+1] = fn * opmbaserate / (1L << (22-blk));
				ch->keynote[chpos+1] = (fn << (opncfg.ratebit + blk)) >> 6;
				channleupdate(ch);		//Key Scale �ɂ��G���x���[�v�`��̍X�V
				break;

			case 0xac:					// Ch3 Block & F-num 2
				ch = opnch + chbase + 2;
				ch->keyfunc[chpos+1] = value & 0x3f;
				break;

			case 0xb0:					//FB & AL

				//�G���x���[�v������
				slot = ch->slot;
				for (i=0; i<4; i++) {
					//��������������A�A�^�b�N�����蒼���B
					if(slot->env_mode > EM_RELEASE){
						slot->env_mode = EM_ATTACK;
						slot->env_cnt = EC_ATTACK;
						slot->env_end = EC_DECAY;
						slot->env_inc = slot->env_inc_attack;
					//note off����������A�����Ȃ����񂮁B
					} else {
						slot->env_mode = EM_OFF;
						slot->env_cnt = EC_ATTACK;
						slot->env_end = EC_DECAY;
						slot->env_inc = 0;
					}
					slot++;
				}
				ch->algorithm = (UINT8)(value & 7);
				value = (value >> 3) & 7;
				if (value) {
					ch->feedback = 8 - value;
				}
				else {
					ch->feedback = 0;
				}
				set_algorithm(ch);
				break;

			case 0xb4:				//PAN & AMS & PMS
				if (ch->stereo) {
					ch->pms = lfo_pms_table[ value & 0x07];	
					ch->ams = lfo_ams_table[(value & 0x30) >> 4];
					ch->pan = (UINT8)(value & 0xc0);
				}
				set_algorithm(ch);
				break;
		}
	}
}

//==============================================================
//			
//--------------------------------------------------------------
//	������
//			
//	���Ԃ�l
//			
//	�����l
//			
//==============================================================
void opngen_keyon(UINT chnum, REG8 value) {

	OPNCH	*ch;
	OPNSLOT	*slot;
	REG8	bit;
	UINT	i;

	SINT32	iEnv;				//���݂̃G���x���[�v�l

	sound_sync();
	opngen.keyreg[chnum] = value;
	opngen.playing++;
	ch = opnch + chnum;
	ch->playing |= value >> 4;
	slot = ch->slot;
	bit = 0x10;
	for (i=0; i<4; i++) {
		if (value & bit) {							// keyon
			if (slot->env_mode <= EM_RELEASE) {
				slot->freq_cnt = 0;
				if (i == OPNSLOT1) {
					ch->op1fb = 0;
				}
				//�A�^�b�N�́A���݂̃G���x���[�v�̉��ʂ���n�߂�B
				iEnv = opncfg.envcurve[slot->env_cnt >> ENV_BITS];	//���݂̉���
				slot->env_mode = EM_ATTACK;
				slot->env_end = EC_DECAY;
				slot->env_inc = slot->env_inc_attack;
				if(iEnv>=EVC_ENT) {
					slot->env_cnt  = EC_ATTACK;
				} else if(iEnv<=0) {
					slot->env_cnt  = EC_DECAY;
				} else {
					slot->env_cnt = (long)((EVC_ENT-1 -sqrt(EVC_ENT*sqrt(EVC_ENT*sqrt(EVC_ENT*iEnv)))) * (1 << ENV_BITS));
					if((slot->env_cnt)< EC_ATTACK ){
						slot->env_cnt = EC_ATTACK;
					}
				}
			}
		}
		else {										// keyoff
			if (slot->env_mode > EM_RELEASE) {
				slot->env_mode = EM_RELEASE;
				slot->env_end = EC_OFF;
				slot->env_inc = slot->env_inc_release;
				if (!(slot->env_cnt & EC_DECAY)) {
					//�܂��ADecay�ɒB���Ă��Ȃ��ꍇ�B
					slot->env_cnt = (opncfg.envcurve[slot->env_cnt >> ENV_BITS] << ENV_BITS) + EC_DECAY;
				}
			}
		}
		slot++;
		bit <<= 1;
	}
	keydisp_fmkeyon((UINT8)chnum, value);
}

