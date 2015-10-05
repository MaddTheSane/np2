

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
