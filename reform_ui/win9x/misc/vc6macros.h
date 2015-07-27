/**
 * @file	vc6macros.h
 * @brief	VC6 ópÉ}ÉNÉç
 */

#pragma once

#ifndef _countof
//! countof
#define _countof(x)		(sizeof((x)) / sizeof((x)[0]))
#endif	// _countof

#if (_MSC_VER < 1300)
//! for scope
#define for					if (0 /*NEVER*/) { /* no process */ } else for
#endif	// (_MSC_VER < 1300)

#ifndef GET_X_LPARAM
//! x-coordinate from LPARAM
#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
//! y-coordinate from LPARAM
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))
#endif

// for VC6SDK
#if (_MSC_VER < 1300)
#ifndef LONG_PTR
#define LONG_PTR			LONG
#endif
#ifndef DWORD_PTR
#define DWORD_PTR			DWORD
#endif
#ifndef GetWindowLongPtr
#define GetWindowLongPtr	GetWindowLong
#endif
#ifndef SetWindowLongPtr
#define SetWindowLongPtr	SetWindowLong
#endif
#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC		GWL_WNDPROC
#endif
#ifndef GWLP_HINSTANCE
#define GWLP_HINSTANCE		GWL_HINSTANCE
#endif
#ifndef GWLP_HWNDPARENT
#define GWLP_HWNDPARENT		GWL_HWNDPARENT
#endif
#ifndef GWLP_USERDATA
#define GWLP_USERDATA		GWL_USERDATA
#endif
#ifndef GWLP_ID
#define GWLP_ID				GWL_ID
#endif
#endif
