/**
 * @file	dd2.h
 * @brief	DirectDraw2 �`��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <ddraw.h>
#include "cmndraw.h"

/**
 * @brief DirectDraw2 class
 */
class DDraw2
{
public:
	DDraw2();
	~DDraw2();

	bool Create(HWND hWnd, int nWidth, int nHeight);
	void Destroy();
	void Blt(LPDIRECTDRAWSURFACE pSurface, const POINT* pt, const RECT* lpRect = NULL);
	UINT16 GetPalette16(RGB32 pal) const;
	operator LPDIRECTDRAW2();
	UINT GetBpp() const;

protected:
	HWND					m_hWnd;				/*!< �E�B���h�E �n���h�� */
	LPDIRECTDRAW			m_pDDraw;			/*!< DirectDraw �C���X�^���X */
	LPDIRECTDRAW2			m_pDDraw2;			/*!< DirectDraw2 �C���X�^���X */
	LPDIRECTDRAWSURFACE		m_pPrimarySurface;	/*!< �v���C�}�� �T�[�t�F�X */
	LPDIRECTDRAWCLIPPER		m_pClipper;			/*!< �N���b�p�[ */
	LPDIRECTDRAWPALETTE		m_pPalette;			/*!< �p���b�g */
	UINT					m_nBpp;				/*!< BPP */
	RGB32					m_pal16;			/*!< 16BPP�}�X�N */
	UINT8					m_r16b;				/*!< B �V�t�g�� */
	UINT8					m_l16r;				/*!< R �V�t�g�� */
	UINT8					m_l16g;				/*!< G �V�t�g�� */
	PALETTEENTRY			m_pal[256];			/*!< �p���b�g */
};

/**
 * DirectDraw2 �C���X�^���X�𓾂�
 * @return DirectDraw2 �C���X�^���X
 */
inline DDraw2::operator LPDIRECTDRAW2()
{
	return m_pDDraw2;
}

/**
 * BPP�𓾂�
 * @return BPP
 */
inline UINT DDraw2::GetBpp() const
{
	return m_nBpp;
}

/**
 * @brief DirectDraw2 surface class
 */
class DDraw2Surface
{
public:
	DDraw2Surface();
	~DDraw2Surface();
	bool Create(DDraw2& dd2, int nWidth, int nHeight);
	void Destroy();
	CMNVRAM* Lock();
	void Unlock();
	operator LPDIRECTDRAWSURFACE();

protected:
	LPDIRECTDRAWSURFACE		m_pBackSurface;		/*!< �o�b�N �T�[�t�F�X */
	CMNVRAM					m_vram;				/*!< VRAM */
};

/**
 * �o�b�N�T�[�t�F�X�𓾂�
 * @return �o�b�N�T�[�t�F�X
 */
inline DDraw2Surface::operator LPDIRECTDRAWSURFACE()
{
	return m_pBackSurface;
}
