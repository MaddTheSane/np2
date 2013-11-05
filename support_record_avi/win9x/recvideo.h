/**
 * @file	recvideo.h
 * @brief	録画クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#if defined(SUPPORT_RECVIDEO)

#include <vfw.h>

/**
 * @brief 録画クラス
 */
class RecodeVideo
{
public:
	static RecodeVideo& GetInstance();

	RecodeVideo();
	~RecodeVideo();
	bool Open(LPCTSTR lpFilename, HWND hWnd = NULL);
	void Close();
	void Write();
	void Update();

private:
	static RecodeVideo sm_instance;		/**< 唯一のインスタンスです */

	bool m_bEnable;						/**< 有効フラグ */
	bool m_bDirty;						/**< ダーティ フラグ */

	int m_nNumber;						/**< ファイル番号 */
	int m_nStep;						/**< クロック */
	UINT8* m_pWork8;					/**< ワーク */
	UINT8* m_pWork24;					/**< ワーク */

	PAVIFILE m_pAvi;					/**< AVIFILE */
	PAVISTREAM m_pStm;					/**< AVISTREAM */
	PAVISTREAM m_pStmTmp;				/**< AVISTREAM */
	UINT m_nFrame;						/**< フレーム数 */
	DWORD m_dwSize;						/**< サイズ */

	TCHAR m_szPath[MAX_PATH];			/**< ベース パス */
	BITMAPINFOHEADER m_bmih;			/**< BITMAPINFOHEADER */
	COMPVARS m_cv;						/**< COMPVARS */

	bool OpenFile();
	void CloseFile();
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline RecodeVideo& RecodeVideo::GetInstance()
{
	return sm_instance;
}

#define recvideo_open			RecodeVideo::GetInstance().Open
#define recvideo_close			RecodeVideo::GetInstance().Close
#define recvideo_write			RecodeVideo::GetInstance().Write
#define recvideo_update			RecodeVideo::GetInstance().Update

#else	// defined(SUPPORT_RECVIDEO)

static inline bool recvideo_open(LPCTSTR f) { return false; }
static inline void recvideo_close() { }
static inline void recvideo_write() { }
static inline void recvideo_update() { }

#endif	// defined(SUPPORT_RECVIDEO)
