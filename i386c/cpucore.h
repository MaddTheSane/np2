/*	$Id: cpucore.h,v 1.3 2003/12/12 01:04:40 yui Exp $	*/

#include	"memory.h"

#ifndef	NP2_I386C_CPUCORE_H__
#define	NP2_I386C_CPUCORE_H__

#define	I286_MEMREADMAX		0xa4000
#define	I286_MEMWRITEMAX	0xa0000

#if defined(CPUCORE_IA32)
#include "ia32/cpu.h"
#else /* !CPUCORE_IA32 */
#include "i286/cpu.h"
#endif /* CPUCORE_IA32 */

#endif	/* !NP2_I386C_CPUCORE_H__ */
