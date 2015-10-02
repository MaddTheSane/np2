/**
 * @file	menu.h
 * @brief	メニューの宣言およびインターフェイスの定義をします
 */

#pragma once

void sysmenu_initialize();
void sysmenu_settoolwin(UINT8 value);
void sysmenu_setkeydisp(UINT8 value);
void sysmenu_setwinsnap(UINT8 value);
void sysmenu_setbackground(UINT8 value);
void sysmenu_setbgsound(UINT8 value);
void sysmenu_setscrnmul(UINT8 value);

#define	MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

BOOL menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos);
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, BOOL bSeparator);
int menu_addmenures(HMENU hMenu, int nPos, UINT uID, BOOL bSeparator);
int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID);
BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags, UINT_PTR uIDNewItem, UINT uID);
void menu_addmenubar(HMENU popup, HMENU menubar);

void xmenu_initialize(HMENU hMenu);
void xmenu_update(HMENU hMenu);
void xmenu_setroltate(HMENU hMenu, UINT8 value);
void xmenu_sets98logging(UINT8 value);
void xmenu_setwaverec(UINT8 value);
