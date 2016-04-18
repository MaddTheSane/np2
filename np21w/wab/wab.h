/**
 * @file	wab.h
 * @brief	Window Accelerator Board Interface
 *
 * @author	$Author: SimK $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct {
	int		posx;
	int		posy;
	int		multiwindow;
	int		multithread;
	int		halftone;
	int		forcevga;
} NP2WABCFG;

typedef void NP2WAB_DrawFrame();
typedef struct {
	int ready; // 0�ȊO�Ȃ�`���Ă��ǂ���
	int multiwindow; // �ʑ����[�h
	HWND hWndMain; // ���C���E�B���h�E�̃n���h��
	HWND hWndWAB; // �E�B���h�E�A�N�Z�����[�^�ʑ��̃n���h��
	HDC hDCWAB; // �E�B���h�E�A�N�Z�����[�^�ʑ���HDC
	HBITMAP hBmpBuf; // �o�b�t�@�r�b�g�}�b�v�i��ɓ��{�j
	HDC     hDCBuf; // �o�b�t�@��HDC
	REG8 relay; // ��ʏo�̓����[��ԁibit0=�����E�B���h�E�A�N�Z�����[�^, bit1=RGB IN�X���[, ����ȊO�̃r�b�g��Reserved�Bbit0,1��00��98�O���t�B�b�N�j
	REG8 paletteChanged; // �p���b�g�v�X�V�t���O
	int realWidth; // ��ʉ𑜓x(��)
	int realHeight; // ��ʉ𑜓x(����)
	int wndWidth; // �`��̈�T�C�Y(��)
	int wndHeight; // �`��̈�T�C�Y(����)
	int fps; // ���t���b�V�����[�g�i��̍��킹�Ă���邩������Ȃ��j
	NP2WAB_DrawFrame *drawframe; // ��ʕ`��֐��BhDCBuf�ɃA�N�Z�����[�^��ʃf�[�^��]������B

	int vramoffs;
} NP2WAB;

void np2wab_init(HINSTANCE hInstance, HWND g_hWndMain);
void np2wab_reset(const NP2CFG *pConfig);
void np2wab_bind(void);
void np2wab_drawframe();
void np2wab_shutdown();

void np2wab_setRelayState(REG8 state);
void np2wab_setScreenSize(int width, int height);
void np2wab_setScreenSizeMT(int width, int height);

extern NP2WAB np2wab;
extern NP2WABCFG np2wabcfg;

#ifdef __cplusplus
}
#endif

