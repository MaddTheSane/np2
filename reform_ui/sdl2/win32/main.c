/**
 * @file	main.c
 * @brief	���C��
 */

#include "compiler.h"
#include "np2.h"
#include "..\fontmng.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "zlib.lib")

#if !defined(RESOURCE_US)
#pragma comment(lib, "SDL2_ttf.lib")
#endif

/** �v���g�^�C�v */
void board118_deinitialize(void);

/**
 * ���C��
 * @param[in] argc ����
 * @param[in] argv ����
 * @return ���U���g �R�[�h
 */
int main(int argc, char *argv[])
{
	UINT nLength;
	TCHAR szFont[MAX_PATH];
	int ret;

	nLength = GetWindowsDirectory(szFont, SDL_arraysize(szFont));
	lstrcpy(szFont + nLength, TEXT("\\Fonts\\msgothic.ttc"));
	fontmng_setdeffontname(szFont);

	ret = np2_main(argc, argv);

	board118_deinitialize();

	return ret;
}
