
enum {
	GRPHXMAX	= 80,
	GRPHYMAX	= 400
};


#ifdef __cplusplus
extern "C" {
#endif

void VRAMCALL makegrph_init(void);
void VRAMCALL makegrph(int page, int alldraw);

#ifdef __cplusplus
}
#endif

