
void dialog_changefdd(BYTE drv);
void dialog_changehdd(BYTE drv);

void dialog_font(void);

void fsspec2path(FSSpec *fs, char *dst, int leng);
void dialog_writebmp(void);
void dialog_s98(void);

BOOL dialog_fileselect(char *name, int size, WindowRef parent);
BOOL dialog_filewriteselect(OSType type, char *title, FSSpec *fsc, WindowRef parentWindow);
