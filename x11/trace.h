#ifndef	NP2_X11_TRACE_H__
#define	NP2_X11_TRACE_H__

#ifndef TRACE

#define	TRACEINIT()
#define	TRACETERM()
#define	TRACEOUT(a)
#ifndef	VERBOSE
#define	VERBOSE(s)
#endif

#else

#ifdef	__cplusplus
extern "C" {
#endif

void trace_init(void);
void trace_term(void);
void trace_fmt(const char *str, ...);

#define	TRACEINIT()		trace_init()
#define	TRACETERM()		trace_term()
#define	TRACEOUT(arg)	trace_fmt arg
#ifndef	VERBOSE
#define	VERBOSE(arg)	trace_fmt arg
#endif

#ifdef	__cplusplus
};
#endif

#endif

#endif	/* NP2_X11_TRACE_H__ */
