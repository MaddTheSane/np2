
#if 0
#define	UPDATE_CLOCK		0x00000001
#define	UPDATE_RATE			0x00000002
#define	UPDATE_SBUF			0x00000004
#define	UPDATE_MIDI			0x00000008
#define	UPDATE_SBOARD		0x00000010
#define	UPDATE_HDD			0x00000020
#define	UPDATE_MEMORY		0x00000040
#define	UPDATE_SERIAL1		0x00000080

extern int updates;
#endif




void disable_windowmenu(void);

void xmenu_setsound(BYTE value);
void xmenu_setbeepvol(BYTE value);
void xmenu_setkey(BYTE value);
void xmenu_setwaitflg(BYTE value);
void xmenu_setframe(BYTE value);
void xmenu_setdispmode(BYTE value);
void xmenu_setmouse(BYTE value);
void xmenu_setmotorflg(BYTE value);
void xmenu_setwinsnap(BYTE value);
void xmenu_setdispclk(BYTE value);
void xmenu_seti286save(BYTE value);
void xmenu_setmimpidef(BYTE value);
void xmenu_setroltate(BYTE value);
void xmenu_sets98logging(BYTE value);
void xmenu_setbtnrapid(BYTE value);
void xmenu_setbtnmode(BYTE value);
void xmenu_setmsrapid(BYTE value);
void xmenu_setxshift(BYTE value);
void xmenu_setf12copy(BYTE value);
void xmenu_setextmem(BYTE value);
void xmenu_setraster(BYTE value);
void xmenu_setbackground(BYTE value);
void xmenu_setbgsound(BYTE value);
void xmenu_setscrnmul(BYTE value);							// ver0.26
void xmenu_setshortcut(BYTE value);							// ver0.30
void xmenu_setsstp(BYTE value);								// ver0.29


#if 0												// îpé~ä÷êî  // ver0.28
void xmenu_setjoystick(BYTE value);
#endif
