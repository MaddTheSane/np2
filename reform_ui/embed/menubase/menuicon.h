/**
 * @file	menuicon.h
 * @brief	Interface of the icons of the menu
 */

#pragma once

#include "menuvram.h"
#ifdef __cplusplus
#include <map>
#include <deque>
#endif	/* __cplusplus */

/**
 * 埋め込みアイコン
 */
enum
{
	MICON_NULL			= 0,
	MICON_STOP,
	MICON_QUESTION,
	MICON_EXCLAME,
	MICON_INFO,
	MICON_FOLDER,
	MICON_FOLDERPARENT,
	MICON_FILE,
	MICON_USER
};

#ifdef __cplusplus

/**
 * @brief Icon manager
 */
class MenuIcon
{
public:
	static MenuIcon* GetInstance();
	void Initialize();
	void Deinitialize();
	void Regist(UINT nId, const MENURES& res);
	VRAMHDL Lock(UINT nId, int nWidth, int nHeight, int nBpp);
	void Unlock(VRAMHDL vram);
	void GarbageCollection();

private:
	/**
	 * @brief キャッシュ
	 */
	struct Cache
	{
		UINT nId;
		int nCount;
		VRAMHDL vram;
	};

	static MenuIcon sm_instance;				/*!< 唯一のインスタンスです */
	std::map<UINT, MENURES> m_resources;		/*!< リソース */
	std::deque<Cache> m_caches;					/*!< キャッシュ */
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline MenuIcon* MenuIcon::GetInstance()
{
	return &sm_instance;
}

extern "C"
{
#endif

void menuicon_regist(UINT16 id, const MENURES *res);

#ifdef __cplusplus
}
#endif
