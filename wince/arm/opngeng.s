
; OPNGENARM‚ð’è‹`‚·‚é‚© SIN_BITS,EVC_BITS‚ÌÝ’è‚ð‡‚¹‚é‚±‚ÆB

FMDIV_BITS		equ		8
FMDIV_ENT		equ		(1 << FMDIV_BITS)
FMVOL_SFTBIT	equ		4

SIN_BITS		equ		8
EVC_BITS		equ		7
ENV_BITS		equ		16
KF_BITS			equ		6
FREQ_BITS		equ		20
ENVTBL_BIT		equ		14
SINTBL_BIT		equ		14

TL_BITS			equ		(FREQ_BITS+2)
OPM_OUTSB		equ		(TL_BITS + 2 - 16)

SIN_ENT			equ		(1 << SIN_BITS)
EVC_ENT			equ		(1 << EVC_BITS)

EC_ATTACK		equ		0
EC_DECAY		equ		(EVC_ENT << ENV_BITS)
EC_OFF			equ		((2 * EVC_ENT) << ENV_BITS)

EM_ATTACK		equ		4
EM_DECAY1		equ		3
EM_DECAY2		equ		2
EM_RELEASE		equ		1
EM_OFF			equ		0



; s_detune1			equ		0
S1_TOTALLEVEL		equ		4
S1_DECAYLEVEL		equ		8
; s_attack			equ		12
; s_decay1			equ		16
; s_decay2			equ		20
; s_release			equ		24
S1_FREQ_CNT			equ		28
S1_FREQ_INC			equ		32
; s_multiple		equ		36
; s_keyscale		equ		40
S1_ENV_MODE			equ		41
; s_envraito		equ		42
; s_ssgeg1			equ		43
S1_ENV_CNT			equ		44
S1_ENV_END			equ		48
S1_ENV_INC			equ		52
; s_env_inc_attack	equ		56
S1_ENVINCDECAY1		equ		60
S1_ENVINCDECAY2		equ		64
; s_env_inc_release	equ		68
S_SIZE				equ		72

S2_ENV_INC			equ		(S_SIZE * 1 + S1_ENV_INC)
S2_ENV_CNT			equ		(S_SIZE * 1 + S1_ENV_CNT)
S2_ENV_END			equ		(S_SIZE * 1 + S1_ENV_END)
S2_TOTALLEVEL		equ		(S_SIZE * 1 + S1_TOTALLEVEL)
S2_FREQ_INC			equ		(S_SIZE * 1 + S1_FREQ_INC)
S2_FREQ_CNT			equ		(S_SIZE * 1 + S1_FREQ_CNT)
S2_ENV_MODE			equ		(S_SIZE * 1 + S1_ENV_MODE)
S2_DECAYLEVEL		equ		(S_SIZE * 1 + S1_DECAYLEVEL)
S2_ENVINCDECAY1		equ		(S_SIZE * 1 + S1_ENVINCDECAY1)
S2_ENVINCDECAY2		equ		(S_SIZE * 1 + S1_ENVINCDECAY2)

S3_ENV_INC			equ		(S_SIZE * 2 + S1_ENV_INC)
S3_ENV_CNT			equ		(S_SIZE * 2 + S1_ENV_CNT)
S3_ENV_END			equ		(S_SIZE * 2 + S1_ENV_END)
S3_TOTALLEVEL		equ		(S_SIZE * 2 + S1_TOTALLEVEL)
S3_FREQ_INC			equ		(S_SIZE * 2 + S1_FREQ_INC)
S3_FREQ_CNT			equ		(S_SIZE * 2 + S1_FREQ_CNT)
S3_ENV_MODE			equ		(S_SIZE * 2 + S1_ENV_MODE)
S3_DECAYLEVEL		equ		(S_SIZE * 2 + S1_DECAYLEVEL)
S3_ENVINCDECAY1		equ		(S_SIZE * 2 + S1_ENVINCDECAY1)
S3_ENVINCDECAY2		equ		(S_SIZE * 2 + S1_ENVINCDECAY2)

S4_ENV_INC			equ		(S_SIZE * 3 + S1_ENV_INC)
S4_ENV_CNT			equ		(S_SIZE * 3 + S1_ENV_CNT)
S4_ENV_END			equ		(S_SIZE * 3 + S1_ENV_END)
S4_TOTALLEVEL		equ		(S_SIZE * 3 + S1_TOTALLEVEL)
S4_FREQ_INC			equ		(S_SIZE * 3 + S1_FREQ_INC)
S4_FREQ_CNT			equ		(S_SIZE * 3 + S1_FREQ_CNT)
S4_ENV_MODE			equ		(S_SIZE * 3 + S1_ENV_MODE)
S4_DECAYLEVEL		equ		(S_SIZE * 3 + S1_DECAYLEVEL)
S4_ENVINCDECAY1		equ		(S_SIZE * 3 + S1_ENVINCDECAY1)
S4_ENVINCDECAY2		equ		(S_SIZE * 3 + S1_ENVINCDECAY2)

; C_algorithm		equ		(S_SIZE * 4 + 0)
C_FEEDBACK			equ		(S_SIZE * 4 + 1)
C_PLAYING			equ		(S_SIZE * 4 + 2)
C_OUTSLOT			equ		(S_SIZE * 4 + 3)
C_OP1FB				equ		(S_SIZE * 4 + 4)
C_CONNECT1			equ		(S_SIZE * 4 + 8)
C_CONNECT3			equ		(S_SIZE * 4 + 12)
C_CONNECT2			equ		(S_SIZE * 4 + 16)
C_CONNECT4			equ		(S_SIZE * 4 + 20)
; C_keynote			equ		(S_SIZE * 4 + 24)
; C_keyfunc			equ		(S_SIZE * 4 + 40)
; C_kcode			equ		(S_SIZE * 4 + 44)
; C_pan				equ		(S_SIZE * 4 + 48)
; C_extop			equ		(S_SIZE * 4 + 49)
; C_stereo			equ		(S_SIZE * 4 + 50)
; C_padding2		equ		(S_SIZE * 4 + 51)
C_SIZE				equ		(S_SIZE * 4 + 52)

G_PLAYCHANNELS		equ		0
G_PLAYING			equ		4
G_FEEDBACK2			equ		8
G_FEEDBACK3			equ		12
G_FEEDBACK4			equ		16
G_OUTDL				equ		20
G_OUTDC				equ		24
G_OUTDR				equ		28
G_CALCREMAIN		equ		32
; G_keyreg			equ		36

T_ORG				equ		24
T_CALC1024			equ		(0 - T_ORG)
T_FMVOL				equ		(4 - T_ORG)
T_ratebit			equ		(8 - T_ORG)
T_vr_en				equ		(12 - T_ORG)
T_vr_l				equ		(16 - T_ORG)
T_vr_r				equ		(20 - T_ORG)
T_sintable			equ		(24 - T_ORG)
T_envtable			equ		(24 - T_ORG + SIN_ENT * 4)
T_envcurve			equ		(24 - T_ORG + SIN_ENT * 4 + EVC_ENT * 4)

	IMPORT	opnch
	IMPORT	opngen
	IMPORT	opncfg

	EXPORT	opngen_getpcm
	EXPORT	opngen_getpcmvr

	AREA	OPNGENA, CODE, READONLY

; r0	Temporary Register
; r1	Offset
; r2	Counter
; r3	Temporary Register
; r4	Temporary Register
; r5	channel counter
; r6	OPNCH
; r7	OPNCH base
; r8	L
; r9	R
; r10	opngen Fix
; r11	opncfg Fix
; r12	Temporary Register


opngen_getpcm
opngen_getpcmvr
				cmp		r2, #0
				moveq	pc, lr
				ldr		r12, dcd_opngen
				ldr		r3, [r12, #G_PLAYING]
				cmp		r3, #0
				moveq	pc, lr

				stmdb	sp!, {r4 - r11, lr}
				ldr		r7, dcd_opnch
				mov		r10, r12
				ldr		r11, dcd_opncfg
				ldr		lr, [r10, #G_CALCREMAIN]
				ldr		r3, [r10, #G_OUTDL]
				ldr		r4, [r10, #G_OUTDR]
getpcm_lp		rsb		r0, lr, #0
				mul		r8, r0, r3
				mul		r9, r0, r4
				add		lr, lr, #FMDIV_ENT
mksmp_lp		mov		r12, #0
				str		r12, [r10, #G_OUTDL]
				str		r12, [r10, #G_OUTDC]
				str		r12, [r10, #G_OUTDR]
				ldr		r5, [r10, #G_PLAYCHANNELS]
				sub		r5, r12, r5, lsl #8
				mov		r6, r7
slotcalc_lp		ldrb	r0, [r6, #C_PLAYING]
				ldrb	r12, [r6, #C_OUTSLOT]
				tst		r0, r12
				beq		slot5calc
				add		r5, r5, #1

				mov		r12, #0
				str		r12, [r10, #G_FEEDBACK2]
				str		r12, [r10, #G_FEEDBACK3]
				str		r12, [r10, #G_FEEDBACK4]
slot1calc		ldr		r3, [r6, #S1_ENV_INC]		; calc env
				ldr		r12, [r6, #S1_ENV_CNT]
				add		r3, r3, r12
				ldr		r12, [r6, #S1_ENV_END]
				cmp		r3, r12
				bcs		slot1update
s1calcenv		str		r3, [r6, #S1_ENV_CNT]
				mov		r3, r3, lsr #ENV_BITS
				subs	r12, r3, #EVC_ENT
				addcc	r12, r11, #T_envcurve		; r12 = opntbl.envcurve
				ldrcc	r12, [r12, r3, lsl #2]
				ldr		r0, [r6, #S1_TOTALLEVEL]
				subs	r0, r0, r12
				ldr		r3, [r6, #S1_FREQ_INC]		; freq
				ldr		r12, [r6, #S1_FREQ_CNT]
				add		r3, r3, r12
				str		r3, [r6, #S1_FREQ_CNT]
				bls		slot2calc
				ldrb	r4, [r6, #C_FEEDBACK]
				cmp		r4, #0
				ldrne	r12, [r6, #C_OP1FB]
				addne	r3, r3, r12 asr r4			; back!
	if 1
				mov		r3, r3, lsl #(32 - FREQ_BITS)
	else
				mov		r3, r3, lsr #(FREQ_BITS - SIN_BITS)
				mov		r3, r3, lsl #(32 - SIN_BITS)
	endif
				mov		r3, r3, lsr #(32 - SIN_BITS)
				add		r4, r11, #T_sintable		; r1 = opntbl.sintable
				ldr		r3, [r4, r3, lsl #2]
				add		r4, r11, #T_envtable		; r1 = opntbl.envtable
				ldr		r4, [r4, r0, lsl #2]
				mul		r3, r4, r3
				mov		r3, r3, asr #(ENVTBL_BIT + SINTBL_BIT - TL_BITS)
				strne	r3, [r6, #C_OP1FB]
				addne	r3, r3, r12
				subne	r3, r3, r3 asr #31			; adjust....
				movne	r3, r3, asr #1
				ldr		r12, [r6, #C_CONNECT1]
				cmp		r12, #0
				streq	r3, [r10, #G_FEEDBACK2]
				streq	r3, [r10, #G_FEEDBACK3]
				streq	r3, [r10, #G_FEEDBACK4]
				ldrne	r0, [r12]
				addne	r0, r0, r3
				strne	r0, [r12]
slot2calc		ldr		r3, [r6, #S2_ENV_INC]		; calc env
				ldr		r12, [r6, #S2_ENV_CNT]
				add		r3, r3, r12
				ldr		r12, [r6, #S2_ENV_END]
				cmp		r3, r12
				bcs		slot2update
s2calcenv		str		r3, [r6, #S2_ENV_CNT]
				mov		r3, r3, lsr #ENV_BITS
				subs	r12, r3, #EVC_ENT
				addcc	r12, r11, #T_envcurve		; r12 = opntbl.envcurve
				ldrcc	r12, [r12, r3, lsl #2]
				ldr		r0, [r6, #S2_TOTALLEVEL]
				subs	r0, r0, r12
				ldr		r3, [r6, #S2_FREQ_INC]		; freq
				ldr		r12, [r6, #S2_FREQ_CNT]
				add		r3, r3, r12
				str		r3, [r6, #S2_FREQ_CNT]
				bls		slot3calc
				ldr		r12, [r10, #G_FEEDBACK2]
				add		r3, r3, r12
	if 1
				mov		r3, r3, lsl #(32 - FREQ_BITS)
	else
				mov		r3, r3, lsr #(FREQ_BITS - SIN_BITS)
				mov		r3, r3, lsl #(32 - SIN_BITS)
	endif
				mov		r3, r3, lsr #(32 - SIN_BITS)
				add		r12, r11, #T_sintable		; r12 = opntbl.sintable
				ldr		r3, [r12, r3, lsl #2]
				add		r12, r11, #T_envtable		; r12 = opntbl.envtable
				ldr		r12, [r12, r0, lsl #2]
				mul		r3, r12, r3
				ldr		r12, [r6, #C_CONNECT2]
				ldr		r0, [r12]
				add		r0, r0, r3, asr #(ENVTBL_BIT + SINTBL_BIT - TL_BITS)
				str		r0, [r12]
slot3calc		ldr		r3, [r6, #S3_ENV_INC]		; calc env
				ldr		r12, [r6, #S3_ENV_CNT]
				add		r3, r3, r12
				ldr		r12, [r6, #S3_ENV_END]
				cmp		r3, r12
				bcs		slot3update
s3calcenv		str		r3, [r6, #S3_ENV_CNT]
				mov		r3, r3, lsr #ENV_BITS
				subs	r12, r3, #EVC_ENT
				addcc	r12, r11, #T_envcurve		; r12 = opntbl.envcurve
				ldrcc	r12, [r12, r3, lsl #2]
				ldr		r0, [r6, #S3_TOTALLEVEL]
				subs	r0, r0, r12
				ldr		r3, [r6, #S3_FREQ_INC]		; freq
				ldr		r12, [r6, #S3_FREQ_CNT]
				add		r3, r3, r12
				str		r3, [r6, #S3_FREQ_CNT]
				bls		slot4calc
				ldr		r12, [r10, #G_FEEDBACK3]
				add		r3, r3, r12
	if 1
				mov		r3, r3, lsl #(32 - FREQ_BITS)
	else
				mov		r3, r3, lsr #(FREQ_BITS - SIN_BITS)
				mov		r3, r3, lsl #(32 - SIN_BITS)
	endif
				mov		r3, r3, lsr #(32 - SIN_BITS)
				add		r12, r11, #T_sintable		; r12 = opntbl.sintable
				ldr		r3, [r12, r3, lsl #2]
				add		r12, r11, #T_envtable		; r12 = opntbl.envtable
				ldr		r12, [r12, r0, lsl #2]
				mul		r3, r12, r3
				ldr		r12, [r6, #C_CONNECT3]
				ldr		r0, [r12]
				add		r0, r0, r3, asr #(ENVTBL_BIT + SINTBL_BIT - TL_BITS)
				str		r0, [r12]
slot4calc		ldr		r3, [r6, #S4_ENV_INC]		; calc env
				ldr		r12, [r6, #S4_ENV_CNT]
				add		r3, r3, r12
				ldr		r12, [r6, #S4_ENV_END]
				cmp		r3, r12
				bcs		slot4update
s4calcenv		str		r3, [r6, #S4_ENV_CNT]
				mov		r3, r3, lsr #ENV_BITS
				subs	r12, r3, #EVC_ENT
				addcc	r12, r11, #T_envcurve		; r12 = opntbl.envcurve
				ldrcc	r12, [r12, r3, lsl #2]
				ldr		r0, [r6, #S4_TOTALLEVEL]
				subs	r0, r0, r12
				ldr		r3, [r6, #S4_FREQ_INC]		; freq
				ldr		r12, [r6, #S4_FREQ_CNT]
				add		r3, r3, r12
				str		r3, [r6, #S4_FREQ_CNT]
				bls		slot5calc
				ldr		r12, [r10, #G_FEEDBACK4]
				add		r3, r3, r12
	if 1
				mov		r3, r3, lsl #(32 - FREQ_BITS)
	else
				mov		r3, r3, lsr #(FREQ_BITS - SIN_BITS)
				mov		r3, r3, lsl #(32 - SIN_BITS)
	endif
				mov		r3, r3, lsr #(32 - SIN_BITS)
				add		r12, r11, #T_sintable		; r12 = opntbl.sintable
				ldr		r3, [r12, r3, lsl #2]
				add		r12, r11, #T_envtable		; r12 = opntbl.envtable
				ldr		r12, [r12, r0, lsl #2]
				mul		r3, r12, r3
				ldr		r12, [r6, #C_CONNECT4]
				ldr		r0, [r12]
				add		r0, r0, r3, asr #(ENVTBL_BIT + SINTBL_BIT - TL_BITS)
				str		r0, [r12]
slot5calc		add		r6, r6, #C_SIZE
				adds	r5, r5, #256
				bcc		slotcalc_lp

				ldr		r12, [r10, #G_OUTDC]
				ldr		r3, [r10, #G_OUTDL]
				ldr		r4, [r10, #G_OUTDR]
				add		r3, r3, r12
				add		r4, r4, r12
				mov		r3, r3, asr #FMVOL_SFTBIT
				mov		r4, r4, asr #FMVOL_SFTBIT
				ldr		r12, [r11, #T_CALC1024]
				subs	lr, lr, r12
				addle	r12, lr, r12
				mla		r8, r12, r3, r8
				mla		r9, r12, r4, r9
				bgt		mksmp_lp
				ldr		r0, [r11, #T_FMVOL]
				mov		r8, r8, asr #FMDIV_BITS
				mov		r9, r9, asr #FMDIV_BITS
				mul		r8, r0, r8
				mul		r9, r0, r9
				ldr		r0, [r1]
				add		r0, r0, r8, asr #(OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
				str		r0, [r1], #4
				ldr		r0, [r1]
				add		r0, r0, r9, asr #(OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
				str		r0, [r1], #4
				subs	r2, r2, #1
				bne		getpcm_lp
				str		r3, [r10, #G_OUTDL]
				str		r4, [r10, #G_OUTDR]
				str		lr, [r10, #G_CALCREMAIN]
				strb	r5, [r10, #G_PLAYING]
				ldmia	sp!, {r4 - r11, pc}

dcd_opngen		dcd		opngen
dcd_opnch		dcd		opnch
dcd_opncfg		dcd		opncfg + T_ORG

slot1update		ldrb	r3, [r6, #S1_ENV_MODE]
				sub		r3, r3, #1
				cmp		r3, #EM_ATTACK
				addcc	pc, pc, r3, lsl #2
				b		s1u_off						; EM_OFF
				b		s1u_rel						; EM_RELEASE
				b		s1u_dc2						; EM_DECAY2
				b		s1u_dc1						; EM_DECAY1
s1u_att			strb	r3, [r6, #S1_ENV_MODE]
				ldr		r3, [r6, #S1_DECAYLEVEL]
				str		r3, [r6, #S1_ENV_END]
				ldr		r3, [r6, #S1_ENVINCDECAY1]
				str		r3, [r6, #S1_ENV_INC]
				mov		r3, #EC_DECAY
				b		s1calcenv
s1u_dc1			strb	r3, [r6, #S1_ENV_MODE]
				mov		r3, #EC_OFF
				str		r3, [r6, #S1_ENV_END]
				ldr		r3, [r6, #S1_ENVINCDECAY2]
				str		r3, [r6, #S1_ENV_INC]
				ldr		r3, [r6, #S1_DECAYLEVEL]
				b		s1calcenv
s1u_rel			strb	r3, [r6, #S1_ENV_MODE]
s1u_dc2			add		r3, r12, #1
				str		r3, [r6, #S1_ENV_END]
				mov		r3, #0
				str		r3, [r6, #S1_ENV_INC]
				ldrb	r12, [r6, #C_PLAYING]
				and		r12, r12, #0xfe
				strb	r12, [r6, #C_PLAYING]
s1u_off			mov		r3, #EC_OFF
				b		s1calcenv

slot2update		ldrb	r3, [r6, #S2_ENV_MODE]
				sub		r3, r3, #1
				cmp		r3, #EM_ATTACK
				addcc	pc, pc, r3, lsl #2
				b		s2u_off						; EM_OFF
				b		s2u_rel						; EM_RELEASE
				b		s2u_dc2						; EM_DECAY2
				b		s2u_dc1						; EM_DECAY1
s2u_att			strb	r3, [r6, #S2_ENV_MODE]
				ldr		r3, [r6, #S2_DECAYLEVEL]
				str		r3, [r6, #S2_ENV_END]
				ldr		r3, [r6, #S2_ENVINCDECAY1]
				str		r3, [r6, #S2_ENV_INC]
				mov		r3, #EC_DECAY
				b		s2calcenv
s2u_dc1			strb	r3, [r6, #S2_ENV_MODE]
				mov		r3, #EC_OFF
				str		r3, [r6, #S2_ENV_END]
				ldr		r3, [r6, #S2_ENVINCDECAY2]
				str		r3, [r6, #S2_ENV_INC]
				ldr		r3, [r6, #S2_DECAYLEVEL]
				b		s2calcenv
s2u_rel			strb	r3, [r6, #S2_ENV_MODE]
s2u_dc2			add		r3, r12, #1
				str		r3, [r6, #S2_ENV_END]
				mov		r3, #0
				str		r3, [r6, #S2_ENV_INC]
				ldrb	r12, [r6, #C_PLAYING]
				and		r12, r12, #0xfd
				strb	r12, [r6, #C_PLAYING]
s2u_off			mov		r3, #EC_OFF
				b		s2calcenv

slot3update		ldrb	r3, [r6, #S3_ENV_MODE]
				sub		r3, r3, #1
				cmp		r3, #EM_ATTACK
				addcc	pc, pc, r3, lsl #2
				b		s3u_off						; EM_OFF
				b		s3u_rel						; EM_RELEASE
				b		s3u_dc2						; EM_DECAY2
				b		s3u_dc1						; EM_DECAY1
s3u_att			strb	r3, [r6, #S3_ENV_MODE]
				ldr		r3, [r6, #S3_DECAYLEVEL]
				str		r3, [r6, #S3_ENV_END]
				ldr		r3, [r6, #S3_ENVINCDECAY1]
				str		r3, [r6, #S3_ENV_INC]
				mov		r3, #EC_DECAY
				b		s3calcenv
s3u_dc1			strb	r3, [r6, #S3_ENV_MODE]
				mov		r3, #EC_OFF
				str		r3, [r6, #S3_ENV_END]
				ldr		r3, [r6, #S3_ENVINCDECAY2]
				str		r3, [r6, #S3_ENV_INC]
				ldr		r3, [r6, #S3_DECAYLEVEL]
				b		s3calcenv
s3u_rel			strb	r3, [r6, #S3_ENV_MODE]
s3u_dc2			add		r3, r12, #1
				str		r3, [r6, #S3_ENV_END]
				mov		r3, #0
				str		r3, [r6, #S3_ENV_INC]
				ldrb	r12, [r6, #C_PLAYING]
				and		r12, r12, #0xfb
				strb	r12, [r6, #C_PLAYING]
s3u_off			mov		r3, #EC_OFF
				b		s3calcenv

slot4update		ldrb	r3, [r6, #S4_ENV_MODE]
				sub		r3, r3, #1
				cmp		r3, #EM_ATTACK
				addcc	pc, pc, r3, lsl #2
				b		s4u_off						; EM_OFF
				b		s4u_rel						; EM_RELEASE
				b		s4u_dc2						; EM_DECAY2
				b		s4u_dc1						; EM_DECAY1
s4u_att			strb	r3, [r6, #S4_ENV_MODE]
				ldr		r3, [r6, #S4_DECAYLEVEL]
				str		r3, [r6, #S4_ENV_END]
				ldr		r3, [r6, #S4_ENVINCDECAY1]
				str		r3, [r6, #S4_ENV_INC]
				mov		r3, #EC_DECAY
				b		s4calcenv
s4u_dc1			strb	r3, [r6, #S4_ENV_MODE]
				mov		r3, #EC_OFF
				str		r3, [r6, #S4_ENV_END]
				ldr		r3, [r6, #S4_ENVINCDECAY2]
				str		r3, [r6, #S4_ENV_INC]
				ldr		r3, [r6, #S4_DECAYLEVEL]
				b		s4calcenv
s4u_rel			strb	r3, [r6, #S4_ENV_MODE]
s4u_dc2			add		r3, r12, #1
				str		r3, [r6, #S4_ENV_END]
				mov		r3, #0
				str		r3, [r6, #S4_ENV_INC]
				ldrb	r12, [r6, #C_PLAYING]
				and		r12, r12, #0xf7
				strb	r12, [r6, #C_PLAYING]
s4u_off			mov		r3, #EC_OFF
				b		s4calcenv

	END

