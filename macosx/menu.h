
void menu_setsound(BYTE value);
void menu_setbeepvol(BYTE value);
void menu_setkey(BYTE value);
void menu_setwaitflg(BYTE value);
void menu_setframe(BYTE value);
void menu_setdispmode(BYTE value);
void menu_setmotorflg(BYTE value);
void menu_setdispclk(BYTE value);
void menu_setrotate(BYTE value);
void menu_setbtnrapid(BYTE value);
void menu_setbtnmode(BYTE value);
void menu_setxshift(BYTE value);
void menu_setf12copy(BYTE value);
void menu_setextmem(BYTE value);
void menu_setraster(BYTE value);

#if defined(NP2GCC)
void menu_setmouse(BYTE value);
#endif

#if 0
void menu_sets98logging(BYTE value);
#endif
void menu_setrecording(bool end);
