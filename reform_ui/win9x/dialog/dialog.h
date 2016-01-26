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
void dialog_serial(HWND hWnd);
void dialog_calendar(HWND hwndParent);

// d_bmp.cpp
void dialog_writebmp(HWND hWnd);

// d_disk.cpp
void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_changehdd(HWND hWnd, REG8 drv);
void dialog_newdisk(HWND hWnd);

// d_font.cpp
void dialog_font(HWND hWnd);

// d_soundlog.cpp
void dialog_soundlog(HWND hWnd);
