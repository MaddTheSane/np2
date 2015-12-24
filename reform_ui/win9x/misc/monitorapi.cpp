/**
 * @file	monitorapi.cpp
 * @brief	モニタ APIの動作の定義を行います
 */

#include "compiler.h"
#include "monitorapi.h"

#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)

typedef HMONITOR (WINAPI * FnMonitorFromWindow)(HWND, DWORD);
typedef HMONITOR (WINAPI * FnMonitorFromRect)(LPCRECT, DWORD);
typedef HMONITOR (WINAPI * FnMonitorFromPoint)(POINT, DWORD);
typedef BOOL (WINAPI * FnGetMonitorInfo)(HMONITOR, LPMONITORINFO);

/**
 * Retrieves a handle to the display monitor that has the largest area of intersection with the bounding rectangle of a specified window
 * @param[in] hwnd A handle to the window of interest
 * @param[in] dwFlags Determines the function's return value if the window does not intersect any display monitor
 * @return An HMONITOR handle to the display monitor
 */
HMONITOR MonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
	HMODULE hUser32 = ::GetModuleHandle(TEXT("USER32"));
	if (hUser32)
	{
		FnMonitorFromWindow fn = reinterpret_cast<FnMonitorFromWindow>(::GetProcAddress(hUser32, "MonitorFromWindow"));
		if (fn)
		{
			return (*fn)(hwnd, dwFlags);
		}
	}
	return NULL;
}

/**
 * Retrieves a handle to the display monitor that has the largest area of intersection with a specified rectangle
 * @param[in] lprc A pointer to a RECT structure that specifies the rectangle of interest in virtual-screen coordinates
 * @param[in] dwFlags Determines the function's return value if the window does not intersect any display monitor
 * @return An HMONITOR handle to the display monitor
 */
HMONITOR MonitorFromRect(LPCRECT lprc, DWORD dwFlags)
{
	HMODULE hUser32 = ::GetModuleHandle(TEXT("USER32"));
	if (hUser32)
	{
		FnMonitorFromRect fn = reinterpret_cast<FnMonitorFromRect>(::GetProcAddress(hUser32, "MonitorFromRect"));
		if (fn)
		{
			return (*fn)(lprc, dwFlags);
		}
	}
	return NULL;
}

/**
 * Retrieves a handle to the display monitor that contains a specified point
 * @param[in] pt A POINT structure that specifies the point of interest in virtual-screen coordinates
 * @param[in] dwFlags Determines the function's return value if the window does not intersect any display monitor
 * @return An HMONITOR handle to the display monitor
 */
HMONITOR MonitorFromPoint(POINT pt, DWORD dwFlags)
{
	HMODULE hUser32 = ::GetModuleHandle(TEXT("USER32"));
	if (hUser32)
	{
		FnMonitorFromPoint fn = reinterpret_cast<FnMonitorFromPoint>(::GetProcAddress(hUser32, "MonitorFromPoint"));
		if (fn)
		{
			return (*fn)(pt, dwFlags);
		}
	}
	return NULL;
}

/**
 * The GetMonitorInfo function retrieves information about a display monitor
 * @param[in] hMonitor A handle to the display monitor of interest
 * @param[out] lpmi A pointer to a MONITORINFO or MONITORINFOEX structure that receives information about the specified display monitor
 * @return TRUE If the function succeeds
 * @return FALSE If the function fails
 */
BOOL GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
	HMODULE hUser32 = ::GetModuleHandle(TEXT("USER32"));
	if (hUser32)
	{
#ifdef UNICODE
		FnGetMonitorInfo fn = reinterpret_cast<FnGetMonitorInfo>(::GetProcAddress(hUser32, "GetMonitorInfoW"));
#else
		FnGetMonitorInfo fn = reinterpret_cast<FnGetMonitorInfo>(::GetProcAddress(hUser32, "GetMonitorInfoA"));
#endif
		if (fn)
		{
			return (*fn)(hMonitor, lpmi);
		}
	}
	return FALSE;
}
#endif	/* !defined(HMONITOR_DECLARED) && (WINVER < 0x0500) */

/**
 * 対象のウィンドウがあるワークエリアを得る
 * @param[in] hWnd 対象のウィンドウ
 * @param[out] lpRect ワークエリア
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
BOOL GetWorkArea(HWND hWnd, RECT* lpRect)
{
	if (lpRect == NULL)
	{
		return FALSE;
	}

	HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	if (hMonitor != NULL)
	{
		MONITORINFO minfo;
		minfo.cbSize = sizeof(minfo);
		BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);
		*lpRect = minfo.rcWork;
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, lpRect, NULL);
	}
	return TRUE;
}
