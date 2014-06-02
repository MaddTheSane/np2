/**
 * @file	main.c
 * @brief	���C��
 */

#include "compiler.h"
#include "np2.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#if !defined(RESOURCE_US)
#pragma comment(lib, "SDL2_ttf.lib")
#endif

/**
 * ���C��
 * @param[in] argc ����
 * @param[in] argv ����
 * @return ���U���g �R�[�h
 */
int main(int argc, char *argv[])
{
	return np2_main(argc, argv);
}
