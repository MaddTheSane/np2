/**
 *	@file	trace.h
 *	@brief	�g���[�X �R���\�[���̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
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

#define	TRACEINIT()		trace_init()					/*!< ������ */
#define	TRACETERM()		trace_term()					/*!< ��� */
#define	TRACEOUT(arg)	trace_fmt arg					/*!< �o�� */
#define	VERBOSE(arg)	trace_fmt arg					/*!< �o�� */
#define	APPDEVOUT(arg)	putchar(arg)					/*!< �o�� */

#else	/* defined(TRACE) */

#define	TRACEINIT()		do { } while (0 /*CONSTCOND*/)	/*!< ������ */
#define	TRACETERM()		do { } while (0 /*CONSTCOND*/)	/*!< ��� */
#define	TRACEOUT(a)		do { } while (0 /*CONSTCOND*/)	/*!< �o�� */
#define	VERBOSE(a)		do { } while (0 /*CONSTCOND*/)	/*!< �o�� */
#define	APPDEVOUT(a)	do { } while (0 /*CONSTCOND*/)	/*!< �o�� */

#endif	/* defined(TRACE) */
