/**
 * @file	fmboard.h
 */

#pragma once

#if !defined(DISABLE_SOUND)

#include	"fmtimer.h"
#include "opna.h"
#include	"opngen.h"
#include	"pcm86.h"
#include	"cs4231.h"

#ifdef __cplusplus
extern "C" {
#endif

extern	UINT32		g_usesound;
extern	_OPNA		g_opn;

extern	_FMTIMER	g_fmtimer;
extern	_OPNGEN		g_opngen;
extern	_PCM86		pcm86;
extern	_CS4231		cs4231;
extern	_PSGGEN		g_psg2;

#if defined(SUPPORT_PX)
extern	OPN_T		g_opn2;
extern	OPN_T		g_opn3;
extern	_PSGGEN		g_psg3;
extern	_RHYTHM		g_rhythm2;
extern	_RHYTHM		g_rhythm3;
extern	_ADPCM		g_adpcm2;
extern	_ADPCM		g_adpcm3;
#endif	// defined(SUPPORT_PX)

REG8 fmboard_getjoy(PSGGEN psg);

void fmboard_extreg(void (*ext)(REG8 enable));
void fmboard_extenable(REG8 enable);

void fmboard_reset(const NP2CFG *pConfig, UINT32 type);
void fmboard_bind(void);

#ifdef __cplusplus
}
#endif

#else

#define	fmboard_reset(c, t)
#define	fmboard_bind()

#endif
