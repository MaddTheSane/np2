
enum {
	KEY_PC98		= 0,
	KEY_KEY101		= 1,
	KEY_KEY106		= 2,
	KEY_TYPEMAX		= 3,
	KEY_UNKNOWN		= 0xff
};

#ifdef __cplusplus
extern "C" {
#endif

void winkeyinit106(void);
void winkeydown106(WPARAM wParam, LPARAM lParam);
void winkeyup106(WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

