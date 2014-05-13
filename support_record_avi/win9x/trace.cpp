/**
 *	@file	trace.cpp
 *	@brief	トレース コンソールの動作の定義を行います
 */

#include "compiler.h"

#if defined(TRACE)

#include <locale.h>
#include "trace.h"

/**
 * 初期化
 */
void trace_init()
{
	// コンソール ウィンドウの表示
	::AllocConsole();

#if (_MSC_VER >= 1400)

	// 標準出力の割り当て (VS2005以降)
	FILE* fp = NULL;
	_tfreopen_s(&fp, TEXT("CON"), TEXT("w"), stdout);

#else	// (_MSC_VER >= 1400)

	// 標準出力の割り当て (VS2003以前)
	_tfreopen(TEXT("CON"), TEXT("w"), stdout);

#endif	// (_MSC_VER >= 1400)

	_tsetlocale(LC_ALL, TEXT(""));
}

/**
 * 解放
 */
void trace_term()
{
}

/**
 * 表示
 * @param[in] format フォーマット
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
