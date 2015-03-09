/**
 * @file	main.c
 * @brief	メイン
 */

#include "compiler.h"
#include "np2.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#if !defined(RESOURCE_US)
#pragma comment(lib, "SDL2_ttf.lib")
#endif

/**
 * メイン
 * @param[in] argc 引数
 * @param[in] argv 引数
 * @return リザルト コード
 */
int main(int argc, char *argv[])
{
	return np2_main(argc, argv);
}
