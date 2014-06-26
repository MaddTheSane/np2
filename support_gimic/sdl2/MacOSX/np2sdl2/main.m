/**
 * @file	main.m
 * @brief	メイン
 */

#include "compiler.h"
#include "../../np2.h"
#include "../../dosio.h"

/**
 * メイン
 * @param[in] argc 引数
 * @param[in] argv 引数
 * @return リザルト コード
 */
int main(int argc, const char * argv[])
{
	NSString *pstrBundlePath = [[NSBundle mainBundle] bundlePath];
	file_setcd([pstrBundlePath UTF8String]);

	return np2_main(argc, argv);
}
