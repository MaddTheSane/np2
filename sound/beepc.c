#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"


	_BEEP		beep;
	BEEPCFG		beepcfg;


void beep_initialize(UINT rate) {

	beepcfg.rate = rate;
	beepcfg.vol = 2;
	beepcfg.puchibase = (rate * 3) / (11025 * 2);
}

void beep_setvol(UINT vol) {

	beepcfg.vol = vol & 3;
}

void beep_changeclock(void) {

	UINT32	hz;
	UINT	rate;

	hz = pc.realclock / 25;
	rate = beepcfg.rate / 25;
	beepcfg.samplebase = (1 << 16) * rate / hz;
}

void beep_reset(void) {

	beep_changeclock();
	ZeroMemory(&beep, sizeof(beep));
	beep.mode = 1;
}

void beep_hzset(UINT16 cnt) {

	double	hz;

	sound_sync();
	beep.hz = 0;
	if ((cnt & 0xff80) && (beepcfg.rate)) {
		hz = 65536.0 / 4.0 * pc.baseclock / beepcfg.rate / cnt;
		if (hz < 0x8000) {
			beep.hz = (UINT16)hz;
			return;
		}
	}
	beep.puchi = beepcfg.puchibase;
}

void beep_modeset(void) {

	BYTE	newmode;

	newmode = (pit.mode[1] >> 2) & 3;
	beep.puchi = beepcfg.puchibase;
	if (beep.mode != newmode) {
		sound_sync();
		beep.mode = newmode;
		if (!newmode) {					// mode:#0, #1
			beep_eventinit();
		}
	}
}

static void beep_eventset(void) {

	BPEVENT	*evt;
	int		enable;
	SINT32	clock;

	enable = beep.low & beep.buz;
	if (beep.enable != enable) {
		beep.enable = enable;
		if (beep.events < BEEPEVENT_MAX) {
			clock = I286_CLOCK + I286_BASECLOCK - I286_REMCLOCK;
			evt = beep.event + beep.events;
			beep.events++;
			evt->clock = (clock - beep.clock) * beepcfg.samplebase;
			evt->enable = enable;
			beep.clock = clock;
		}
	}
}

void beep_eventinit(void) {

	beep.low = 0;
	beep.enable = 0;
	beep.lastenable = 0;
	beep.clock = soundcfg.lastclock;
					// nevent.clock + nevent.baseclock - nevent.remainclock;
	beep.events = 0;
}

void beep_eventreset(void) {

	beep.lastenable = beep.enable;
	beep.clock = soundcfg.lastclock;
	beep.events = 0;
}


void beep_lheventset(int low) {

	if (beep.low != low) {
		beep.low = low;
		if (!beep.mode) {
			if (beep.events >= (BEEPEVENT_MAX / 2)) {
				sound_sync();
			}
			beep_eventset();
		}
	}
}

void beep_oneventset(void) {

	int		buz;

	buz = (sysport.c & 8)?0:1;

	if (beep.buz != buz) {
		sound_sync();
		beep.buz = buz;
		if (buz) {
			beep.puchi = beepcfg.puchibase;
		}
		if (!beep.mode) {
			beep_eventset();
		}
		else {
			beep.cnt = 0;
		}
	}
}


// ----

#if 0
LABEL void __fastcall beep_update(void *buf, int buflength) {

		__asm {
				cmp		edx, 0
				je		beep_end

				cmp		beep.mode, 1
				jc		beep_onshot
				jne		beep_end

				cmp		beep.buz, 0
				je		beep_puchichk
				mov		ax, beep.hz
				test	ax, ax
				je		beep_puchichk

				pushad
				mov		edi, beeptbl.vol
				test	edi, edi
				je		beep_mkend
				shl		edi, 8

beep_lp1:		mov		esi, 4
beep_lp2:		add		beep.cnt, ax
				js		short beep_mina
					add		[ecx], edi
					add		[ecx+4], edi
				jmp		short beep_lped
beep_mina:			sub		[ecx], edi
					sub		[ecx+4], edi
beep_lped:		dec		esi
				jne		beep_lp2
				add		ecx, 8
				dec		edx
				jne		beep_lp1
				mov		beep.puchi, edx

beep_mkend:		popad
beep_end:		ret

				align	4
beep_puchichk:	cmp		beep.puchi, 0
				jne		short beep_puchiout
				ret
				align	4
beep_puchiout:	pushad
				mov		edi, beeptbl.vol
				shl		edi, 10
beep_puchilp:	add		[ecx], edi
				add		[ecx+4], edi
				add		ecx, 8
				dec		beep.puchi
				je		beep_puchiend
				dec		edx
				jne		beep_puchilp
beep_puchiend:	popad
				ret

				align	4
beep_onshot:	cmp		beepevent.events, 0
				je		beep_end
				cmp		pc.sampleclock, 0
				je		beep_end

				pushad
				mov		esi, offset beepevent.event
				movzx	edi, np2cfg.BEEP_VOL
				test	edi, edi
				je		beep1s_mkend
				shl		edi, 11

beep1shot_lp:	mov		ebp, pc.sampleclock
				xor		eax, eax

beep1shot_lp1:	mov		ebx, [esi]BEEP1EVENT.clock
				cmp		ebp, ebx
				jc		beep1s_envof

				cmp		beepevent.lastenable, 0
				je		beep1s_rmnchk
				add		eax, ebx
beep1s_rmnchk:	mov		edx, [esi]BEEP1EVENT.enable
				mov		beepevent.lastenable, edx
				add		esi, type BEEP1EVENT
				dec		beepevent.events
				je		beep_nonevents
				sub		ebp, ebx
				jne		beep1shot_lp1
				jmp		short beep1s_calcvol

beep1s_envof:	sub		[esi]BEEP1EVENT.clock, ebp
				cmp		beepevent.lastenable, 0
				je		beep1s_calcvol
				add		eax, ebp
beep1s_calcvol:	mul		edi
				div		pc.sampleclock
				add		[ecx], eax
				add		[ecx+4], eax
				add		ecx, 8
				dec		dword ptr [esp + 20]			; pushad‚Ìedx
				jne		beep1shot_lp
				jmp		beep1s_mkend

beep_nonevents:	cmp		beepevent.lastenable, 0
				je		beep1s_calc2
				sub		ebp, ebx
;				shr		ebp, 1
				add		eax, ebp
beep1s_calc2:	mul		edi
				div		pc.sampleclock
				add		[ecx], eax
				add		[ecx+4], eax
				add		ecx, 8
				dec		dword ptr [esp + 20]			; pushad‚Ìedx
				je		beep1s_mkend
				cmp		beepevent.lastenable, 0
				je		beep1s_mkend
beep1shot_lp2:	add		[ecx], edi
				add		[ecx+4], edi
				add		ecx, 8
				dec		dword ptr [esp + 20]			; pushad‚Ìedx
				jne		beep1shot_lp2

beep1s_mkend:
				popad
				ret
		}
}
#endif

