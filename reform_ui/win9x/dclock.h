/**
 * @file	dclock.h
 * @brief
 */

#pragma once

enum
{
	DCLOCK_WIDTH	= 56,
	DCLOCK_HEIGHT	= 12,
	DCLOCK_YALIGN	= (56 / 8)
};


struct DCPOS
{
	UINT8	*pos;
	UINT16	mask;
	UINT8	rolbit;
	UINT8	reserved;
};

/**
 * @brief 時刻表示クラス
 */
struct DispClock
{
	DispClock();
	void Reset();
	void Update();
	void Redraw();
	bool IsDisplayed() const;
	void CountDown(UINT nFrames);
	bool Make();
	void Draw(UINT nBpp, void* lpBuffer, int nYAlign) const;

	/**
	 * @brief QuadBytes
	 */
	union QuadBytes
	{
		UINT8 b[8];			//!< bytes
		UINT64 q;			//!< quad
	};

	const UINT8* m_pFont;		//!< フォント
	const DCPOS* pos;
	QuadBytes m_nCounter;		//!< カウンタ
	UINT8 m_cTime[8];			//!< 現在時間
	UINT8	bak[8];
	UINT16 m_cDirty;			//!< 描画フラグ drawing;
	UINT8 m_cCharaters;			//!< 文字数
	UINT8 dat[(DCLOCK_HEIGHT * DCLOCK_YALIGN) + 4];

	static UINT8 CountPos(UINT nCount);
	void Draw8(void* lpBuffer, int nYAlign) const;
	void Draw16(void* lpBuffer, int nYAlign) const;
	void Draw24(void* lpBuffer, int nYAlign) const;
	void Draw32(void* lpBuffer, int nYAlign) const;
};

typedef struct DispClock	_DCLOCK;
typedef struct DispClock	*DCLOCK;


/**
 * @brief 時刻表示パレット クラス
 */
struct DispClockPalette
{
	void Initialize();
	void SetPalette(UINT bpp);
	void SetPalette8();
	void SetPalette16();

	RGB32	pal32[4];
	RGB16	pal16[4];
	UINT32	pal8[4][16];
};

typedef struct DispClockPalette DCLOCKPAL;

#ifdef __cplusplus
extern "C"
{
#endif

extern	_DCLOCK		dclock;
extern	DCLOCKPAL	dclockpal;

#ifdef __cplusplus
}
#endif

inline void dclock_init(void)
{
	dclockpal.Initialize();
}

inline void dclock_palset(UINT bpp)
{
	dclockpal.SetPalette(bpp);
}

inline void dclock_reset(void)
{
	dclock.Reset();
}

inline void dclock_callback(void)
{
	dclock.Update();
}

inline void dclock_redraw(void)
{
	dclock.Redraw();
}

inline BOOL dclock_disp(void)
{
	return dclock.IsDisplayed();
}
