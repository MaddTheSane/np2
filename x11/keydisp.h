#ifndef	NP2_X11_KEYDISP_H__
#define	NP2_X11_KEYDISP_H__

#define	keydisp_create()
#define	keydisp_destroy()
#define	keydisp_draw(cnt)		(void)cnt

#define	keydisp_setfmboard(board)	(void)board
#define	keydisp_fmkeyon(ch, value)	(void)ch, (void)value
#define	keydisp_psgmix(psg)		(void)psg
#define	keydisp_psgvol(psg, ch)		(void)psg, (void)ch
#define	keydisp_midi(msg)		(void)msg

#define	keydisp_readini()
#define	keydisp_writeini()

#endif	/* NP2_X11_KEYDISP_H__ */
