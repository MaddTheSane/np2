/**
 * @file	recvideo.h
 * @brief	�^��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(SUPPORT_RECVIDEO)

#include <vfw.h>

/**
 * @brief �^��N���X
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
	static RecodeVideo sm_instance;		/**< �B��̃C���X�^���X�ł� */

	bool m_bEnable;						/**< �L���t���O */
	bool m_bDirty;						/**< �_�[�e�B �t���O */

	int m_nNumber;						/**< �t�@�C���ԍ� */
	int m_nStep;						/**< �N���b�N */
	UINT8* m_pWork8;					/**< ���[�N */
	UINT8* m_pWork24;					/**< ���[�N */

	PAVIFILE m_pAvi;					/**< AVIFILE */
	PAVISTREAM m_pStm;					/**< AVISTREAM */
	PAVISTREAM m_pStmTmp;				/**< AVISTREAM */
	UINT m_nFrame;						/**< �t���[���� */
	DWORD m_dwSize;						/**< �T�C�Y */

	TCHAR m_szPath[MAX_PATH];			/**< �x�[�X �p�X */
	BITMAPINFOHEADER m_bmih;			/**< BITMAPINFOHEADER */
	COMPVARS m_cv;						/**< COMPVARS */

	bool OpenFile();
	void CloseFile();
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
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
