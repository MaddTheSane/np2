/**
 * @file	monitorapi.h
 * @brief	モニタ APIの宣言およびインターフェイスの定義をします
 */

#pragma once

#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)

typedef HANDLE HMONITOR;							/*!< Declares A handle */

#define MONITOR_DEFAULTTONULL		0x00000000		/*!< Returns a handle to the display monitor that is nearest to the window */
#define MONITOR_DEFAULTTOPRIMARY	0x00000001		/*!< Returns NULL */
#define MONITOR_DEFAULTTONEAREST	0x00000002		/*!< Returns a handle to the primary display monitor */

#define MONITORINFOF_PRIMARY		0x00000001		/*!< This is the primary display monitor */

/**
 * @brief The MONITORINFO structure contains information about a display monitor
 */
struct tagMONITORINFO
{
	DWORD	cbSize;			/*!< The size of the structure, in bytes */
	RECT	rcMonitor;		/*!< A RECT structure that specifies the display monitor rectangle */
	RECT	rcWork;			/*!< A RECT structure that specifies the work area rectangle of the display monitor */
	DWORD	dwFlags;		/*!< A set of flags that represent attributes of the display monitor */
};
typedef struct tagMONITORINFO MONITORINFO;
typedef struct tagMONITORINFO *LPMONITORINFO;

HMONITOR MonitorFromWindow(HWND hwnd, DWORD dwFlags);
HMONITOR MonitorFromRect(LPCRECT lprc, DWORD dwFlags);
HMONITOR MonitorFromPoint(POINT pt, DWORD dwFlags);
BOOL GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi);

#endif	/* !defined(HMONITOR_DECLARED) && (WINVER < 0x0500) */

BOOL GetWorkArea(HWND hWnd, RECT* lpRect);
