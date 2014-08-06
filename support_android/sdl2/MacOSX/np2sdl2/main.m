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

	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-psn_", 5) == 0)
		{
			argc--;
			memmove(argv + i, argv + i + 1, (argc - i + 1) * sizeof(argv[0]));
			break;
		}
	}

	return np2_main(argc, argv);
}
