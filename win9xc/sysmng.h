
// Ç«Å[Ç≈Ç‡Ç¢Ç¢í ímån

enum {
	SYS_UPDATECFG		= 0x0001,
	SYS_UPDATEOSCFG		= 0x0002,
	SYS_UPDATECLOCK		= 0x0004,
	SYS_UPDATERATE		= 0x0008,
	SYS_UPDATESBUF		= 0x0010,
	SYS_UPDATEMIDI		= 0x0020,
	SYS_UPDATESBOARD	= 0x0040,
	SYS_UPDATEFDD		= 0x0080,
	SYS_UPDATEHDD		= 0x0100,
	SYS_UPDATEMEMORY	= 0x0200,
	SYS_UPDATESERIAL1	= 0x0400
};


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT	sys_updates;

#define	sysmng_initialize()	sys_updates = 0
#define	sysmng_update(a)	sys_updates |= (a)
#define	sysmng_cpureset()	sys_updates	&=									\
										(SYS_UPDATECFG | SYS_UPDATEOSCFG);	\
							sysmng_workclockreset()

void sysmng_workclockreset(void);
void sysmng_updatecaption(void);

#ifdef __cplusplus
}
#endif

