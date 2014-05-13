/**
 *	@file	trace.h
 *	@brief	トレース コンソールの宣言およびインターフェイスの定義をします
 */

#pragma once

#ifdef TRACE

#ifdef __cplusplus
extern "C"
{
#endif

void trace_init();
void trace_term();
void trace_fmt(const char* format, ...);

#ifdef __cplusplus
}
#endif

#define	TRACEINIT()		trace_init()					/*!< 初期化 */
#define	TRACETERM()		trace_term()					/*!< 解放 */
#define	TRACEOUT(arg)	trace_fmt arg					/*!< 出力 */
#define	VERBOSE(arg)	trace_fmt arg					/*!< 出力 */
#define	APPDEVOUT(arg)	putchar(arg)					/*!< 出力 */

#else	/* defined(TRACE) */

#define	TRACEINIT()		do { } while (0 /*CONSTCOND*/)	/*!< 初期化 */
#define	TRACETERM()		do { } while (0 /*CONSTCOND*/)	/*!< 解放 */
#define	TRACEOUT(a)		do { } while (0 /*CONSTCOND*/)	/*!< 出力 */
#define	VERBOSE(a)		do { } while (0 /*CONSTCOND*/)	/*!< 出力 */
#define	APPDEVOUT(a)	do { } while (0 /*CONSTCOND*/)	/*!< 出力 */

#endif	/* defined(TRACE) */
