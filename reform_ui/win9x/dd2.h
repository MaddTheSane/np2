

#pragma once

#include <ddraw.h>
#include "cmndraw.h"

/**
 * @brief DirectDraw2 class
 */
class DD2Surface
{
public:
	DD2Surface();
	~DD2Surface();

	bool Create(HWND hWnd, int nWidth, int nHeight);
	void Release();
	CMNVRAM* Lock();
	void Unlock();
	void Blt(const POINT *pt, const RECT *rect);
	UINT16 GetPalette16(RGB32 pal) const;

protected:
	HWND					hwnd;
	LPDIRECTDRAW			ddraw1;
	LPDIRECTDRAW2			ddraw;
	LPDIRECTDRAWSURFACE		primsurf;
	LPDIRECTDRAWSURFACE		backsurf;
	DDPIXELFORMAT			ddpf;
	LPDIRECTDRAWCLIPPER		clipper;
	LPDIRECTDRAWPALETTE		palette;
	int						cliping;
	RGB32					pal16;
	UINT8					r16b;
	UINT8					l16r;
	UINT8					l16g;
	CMNVRAM					vram;
	PALETTEENTRY			pal[256];
};


#define DD2HDL DD2Surface*
DD2HDL dd2_create(HWND hwnd, int width, int height);
void dd2_release(DD2HDL dd2hdl);
CMNVRAM *dd2_bsurflock(DD2HDL dd2);
void dd2_bsurfunlock(DD2HDL dd2);
void dd2_blt(DD2HDL dd2, const POINT *pt, const RECT *rect);
UINT16 dd2_get16pal(DD2HDL dd2, RGB32 pal);

