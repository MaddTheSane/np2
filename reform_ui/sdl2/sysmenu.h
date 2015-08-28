
enum {
	MENUTYPE_NORMAL	= 0
};


#ifdef __cplusplus
extern "C" {
#endif

BRESULT sysmenu_initialize(void);
void sysmenu_deinitialize(void);

BRESULT sysmenu_menuopen(UINT menutype, int x, int y);

#ifdef __cplusplus
}
#endif

