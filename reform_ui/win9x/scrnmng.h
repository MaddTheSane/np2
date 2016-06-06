/**
 * @file	scrnmng.h
 * @brief	�X�N���[�� �}�l�[�W�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

enum
{
	RGB24_B	= 0,
	RGB24_G	= 1,
	RGB24_R	= 2
};

typedef struct {
	UINT8	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
	int		extend;
} SCRNSURF;

enum
{
	SCRNMODE_FULLSCREEN		= 0x01,
	SCRNMODE_HIGHCOLOR		= 0x02,
	SCRNMODE_ROTATE			= 0x10,
	SCRNMODE_ROTATEDIR		= 0x20,
	SCRNMODE_ROTATELEFT		= (SCRNMODE_ROTATE + 0),
	SCRNMODE_ROTATERIGHT	= (SCRNMODE_ROTATE + SCRNMODE_ROTATEDIR),
	SCRNMODE_ROTATEMASK		= 0x30,
};

enum
{
	FSCRNMOD_NORESIZE		= 0x00,
	FSCRNMOD_ASPECTFIX8		= 0x01,
	FSCRNMOD_ASPECTFIX		= 0x02,
	FSCRNMOD_LARGE			= 0x03,
	FSCRNMOD_ASPECTMASK		= 0x03,
	FSCRNMOD_SAMERES		= 0x04,
	FSCRNMOD_SAMEBPP		= 0x08
};

#ifdef __cplusplus
extern "C"
{
#endif

void scrnmng_setwidth(int posx, int width);
void scrnmng_setextend(int extend);
void scrnmng_setheight(int posy, int height);
BOOL scrnmng_isfullscreen(void);
BOOL scrnmng_haveextend(void);
UINT scrnmng_getbpp(void);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_update(void);
void scrnmng_allflash(void);
void scrnmng_palchanged(void);
RGB16 scrnmng_makepal16(RGB32 pal32);

#ifdef __cplusplus

// ---- for windows

#include "subwnd\dd2.h"

/**
 * @brief �X�N���[�� �}�l�[�W��
 */
class CScreenManager : public DDraw2
{
public:
	static CScreenManager* GetInstance();

	static void Initialize(HWND hWnd);
	CScreenManager();
	bool Create(HWND hWnd, UINT8 scrnmode);
	void Destroy();
	void OnMouseMove(const POINT& pt);
	void EnableUI();
	void DisableUI();
	void SetWidth(int nWidth);
	void SetExtend(int nExtend);
	void SetHeight(int nHeight);
	void SetMultiple(int nMultiple);
	int GetMultiple() const;
	const SCRNSURF* Lock();
	void Unlock(const SCRNSURF *surf);
	void Update();
	bool HasExtendColumn() const;
	void AllFlash();
	void ChangePalette();
	void EnterSizing();

protected:
	static CScreenManager sm_instance;		/*!< �C���X�^���X */
	UINT m_nScreenMode;						/*!< ��ʃ��[�h */
	int m_nWidth;							/*!< �� */
	int m_nExtend;							/*!< �g���� */
	int m_nHeight;							/*!< ���� */
	int m_nMultiple;						/*!< �g�嗦 */
	int m_nCliping;							/*!< �N���b�s���O�� */
	bool m_bHasExtendColumn;				/*!< �g���J����������? */
	bool m_bDisplayedMenu;					/*!< ���j���[�\������? */
	bool m_bAllFlash;						/*!< �t���b�V���v�� */
	bool m_bChangedPalette;					/*!< �p���b�g�ύX */
	int m_nMenuHeight;						/*!< ���j���[�̍��� */
	RECT m_rcProjection;					/*!< ���e�̈� */
	RECT m_rcSurface;						/*!< �T�[�t�F�X�̈� */
	RECT				scrnclip;
	RECT				rectclip;
	LPDIRECTDRAWSURFACE m_pBackSurface;		/*!< �o�b�N�T�[�t�F�X */
	SCRNSURF m_scrnsurf;					/*!< �o�b�N�T�[�t�F�X��� */

	void RenewalClientSize(bool bWndLoc);
	void ClearOutOfRect(const RECT* target, const RECT* base);
	void ClearOutScreen();
	void ClearOutFullscreen();
	virtual void OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes);
	void PaletteSet();

#if defined(SUPPORT_DCLOCK)
public:
	bool IsDispClockClick(const POINT *pt) const;
	void DispClock();

private:
	LPDIRECTDRAWSURFACE m_pClockSurface;
#endif	// defined(SUPPORT_DCLOCK)
};

/**
 * �C���X�^���X���擾
 * @return �C���X�^���X
 */
inline CScreenManager* CScreenManager::GetInstance()
{
	return &sm_instance;
}

/**
 * �m�[�}���f�B�X�v���C�p�̊g���J�������T�|�[�g���Ă��邩?
 * @retval true �T�|�[�g
 * @retval false ���T�|�[�g
 */
inline bool CScreenManager::HasExtendColumn() const
{
	return m_bHasExtendColumn;
}

/**
 * �{�����擾
 * @return �{��
 */
inline int CScreenManager::GetMultiple() const
{
	return m_nMultiple;
}

/**
 * �t���b�V���v��
 */
inline void CScreenManager::AllFlash()
{
	m_bAllFlash = true;
}

/**
 * �p���b�g�X�V�v��
 */
inline void CScreenManager::ChangePalette()
{
	m_bChangedPalette = true;
}

void scrnmng_entersizing(void);
void scrnmng_sizing(UINT side, RECT *rect);
void scrnmng_exitsizing(void);

}
#endif
