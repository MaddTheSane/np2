
enum {
	NORMAL2608	= 0,
	EXTEND2608	= 1
};


#ifdef __cplusplus
extern "C" {
#endif

void S98_init(void);
void S98_trash(void);
BOOL S98_open(const char *filename);
void S98_close(void);
void S98_put(REG8 module, REG8 addr, REG8 data);

#ifdef __cplusplus
}
#endif

