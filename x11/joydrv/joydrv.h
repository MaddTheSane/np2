/*	$Id: joydrv.h,v 1.1 2004/07/26 15:53:27 monaka Exp $	*/

#ifndef	NP2_X11_JOYDRV_JOYDRV_H__
#define	NP2_X11_JOYDRV_JOYDRV_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * joystick driver
 */
typedef struct {
	SINT16	axis[JOY_NAXIS];
	BYTE	button[JOY_NBUTTON];
} JOYINFO_T;

#if defined(USE_SDL_JOYSTICK)
#define	joydrv_initialize	joydrv_sdl_initialize
#define	joydrv_terminate	joydrv_sdl_terminate
#define	joydrv_open		joydrv_sdl_open
#define	joydrv_close		joydrv_sdl_close
#define	joydrv_update		joydrv_sdl_update
#define	joydrv_getstat		joydrv_sdl_getstat
#define	joydrv_getstat_with_map	joydrv_sdl_getstat_with_map
#endif

joydrv_handle_t *joydrv_initialize(void);
void joydrv_terminate(void);
joydrv_handle_t joydrv_open(const char *dev);
void joydrv_close(joydrv_handle_t hdl);
void joydrv_update(joydrv_handle_t hdl);
BOOL joydrv_getstat(joydrv_handle_t hdl, JOYINFO_T *ji);
BOOL joydrv_getstat_with_map(joydrv_handle_t hdl, JOYINFO_T *ji, UINT8 *axismap, UINT8 *btnmap);

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_JOYDRV_JOYDRV_H__ */
