/*	$Id: cpucore.h,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

#ifndef	NP2_I386C_CPUCORE_H__
#define	NP2_I386C_CPUCORE_H__

#define	I286_MEMREADMAX		0xa4000
#define	I286_MEMWRITEMAX	0xa0000

#if defined(CPU386)
#include "ia32/cpu.h"
#else /* !CPU386 */
#include "i286/cpu.h"
#endif /* CPU386 */

#endif	/* !NP2_I386C_CPUCORE_H__ */
