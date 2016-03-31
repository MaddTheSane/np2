/**
 * @file	main.m
 * @brief	���C��
 */

#include "compiler.h"
#include "../../np2.h"
#include "../../dosio.h"

/**
 * ���C��
 * @param[in] argc ����
 * @param[in] argv ����
 * @return ���U���g �R�[�h
 */
int main(int argc, char * argv[])
{
	NSString *pstrBundlePath = [[NSBundle mainBundle] bundlePath];
	file_setcd([pstrBundlePath UTF8String]);

	char** q = &argv[1];
	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-psn_", 5) == 0)
		{
		}
		else if (strcasecmp(argv[i], "-NSDocumentRevisionsDebugMode") == 0)
		{
			i++;
		}
		else
		{
			*q++ = argv[i];
		}
	}
	*q = NULL;

	return np2_main((int)(q - argv), argv);
}
