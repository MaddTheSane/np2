/**
 * @file	cmmidiout.h
 * @brief	MIDI OUT 基底クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief MIDI OUT 基底クラス
 */
class CComMidiOut
{
public:
	/**
	 * デストラクタ
	 */
	virtual ~CComMidiOut()
	{
	}

	/**
	 * ショート メッセージ
	 */
	virtual void Short(UINT32 nMessage)
	{
	}

	/**
	 * ロング メッセージ
	 */
	virtual void Long(const UINT8* lpMessage, UINT cbMessage)
	{
	}
};
