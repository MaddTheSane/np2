/**
 * @file	linker.c
 * @brief	リンカー オプション定義
 */

#include "compiler.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#if !defined(RESOURCE_US)
#pragma comment(lib, "SDL2_ttf.lib")
#endif
