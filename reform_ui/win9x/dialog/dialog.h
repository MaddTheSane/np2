/**
 * @file	dialog.h
 * @breif	ダイアログの宣言
 */

#pragma once

void dialog_about(HWND hwndParent);
void dialog_configure(HWND hwndParent);
void dialog_scropt(HWND hWnd);
void dialog_sndopt(HWND hWnd);
void dialog_mpu98(HWND hwndParent);
void dialog_s98(HWND hWnd);
#if defined(SUPPORT_WAVEREC)
void dialog_waverec(HWND hWnd);
#endif
void dialog_serial(HWND hWnd);
void dialog_calendar(HWND hwndParent);
void dialog_newdisk(HWND hWnd);
void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_changehdd(HWND hWnd, REG8 drv);
void dialog_font(HWND hWnd);
void dialog_writebmp(HWND hWnd);

