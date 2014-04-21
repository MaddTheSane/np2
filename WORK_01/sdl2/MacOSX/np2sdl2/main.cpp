/**
 * @file	main.cpp
 * @brief	メイン
 */

#include <SDL2/SDL_main.h>

/**
 * メイン
 * @param[in] argc 引数
 * @param[in] argv 引数
 * @return リザルト コード
 */
int main(int argc, const char * argv[])
{
	return SDL_main(argc, const_cast<char**>(argv));
}
