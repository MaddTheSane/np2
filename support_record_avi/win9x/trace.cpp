/**
 *	@file	trace.cpp
 *	@brief	�g���[�X �R���\�[���̓���̒�`���s���܂�
 */

#include "compiler.h"

#if defined(TRACE)

#include <locale.h>
#include "trace.h"

/**
 * ������
 */
void trace_init()
{
	// �R���\�[�� �E�B���h�E�̕\��
	::AllocConsole();

#if (_MSC_VER >= 1400)

	// �W���o�͂̊��蓖�� (VS2005�ȍ~)
	FILE* fp = NULL;
	_tfreopen_s(&fp, TEXT("CON"), TEXT("w"), stdout);

#else	// (_MSC_VER >= 1400)

	// �W���o�͂̊��蓖�� (VS2003�ȑO)
	_tfreopen(TEXT("CON"), TEXT("w"), stdout);

#endif	// (_MSC_VER >= 1400)

	_tsetlocale(LC_ALL, TEXT(""));
}

/**
 * ���
 */
void trace_term()
{
}

/**
 * �\��
 * @param[in] format �t�H�[�}�b�g
 */
void trace_fmt(const char* format, ...)
{
	va_list arg;

	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);

	printf("\n");
}

#endif	// defined(TRACE)
