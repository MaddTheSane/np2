
typedef struct {
const BYTE	*src;
const BYTE	*src2;
	BYTE	*dst;
	int		width;
	int		xbytes;
	int		y;
	int		xalign;
	int		yalign;
	BYTE	dirty[SURFACE_WIDTH];
} _SDRAW, *SDRAW;

typedef void (SCRNCALL * SDRAWFN)(SDRAW sdraw, int maxy);


#ifdef __cplusplus
extern "C" {
#endif

const SDRAWFN *sdraw_getproctbl(const SCRNSURF *surf);

#ifdef __cplusplus
}
#endif

