
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

#include	"cmndraw.h"

enum {
	MEMDBG32_BLOCKW		= 3,
	MEMDBG32_BLOCKH		= 6,
};

enum {
	MEMDBG32_PALBG		= 0,
	MEMDBG32_PALBDR,
	MEMDBG32_PALREAL,
	MEMDBG32_PALPM,
	MEMDBG32_PALPAGE0,
	MEMDBG32_PALPAGE1,
	MEMDBG32_PALTXT,

	MEMDBG32_PALS
};


#ifdef __cplusplus
extern "C" {
#endif

void memdbg32_initialize(void);
void memdbg32_setpal(CMNPALFN *palfn);
void memdbg32_getsize(int *width, int *height);
BOOL memdbg32_paint(CMNVRAM *vram, BOOL redraw);

#ifdef __cplusplus
}
#endif

#endif

