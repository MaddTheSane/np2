#ifndef TRACE
#define	TRACEINIT()
#define	TRACETERM()
#define	TRACEOUT(a)
#define	VERBOSE(a)

#else

#ifdef __cplusplus
extern "C" {
#endif
extern void trace_init(void);
extern void trace_term(void);
extern void trace_fmt(const char *str, ...);
#ifdef __cplusplus
}
#endif

#define	TRACEINIT()		trace_init()
#define	TRACETERM()		trace_term()
#define	TRACEOUT(arg)	trace_fmt arg
#define	VERBOSE(arg)	trace_fmt arg
#endif
