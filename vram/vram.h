
#ifndef VRAMCALL
#define	VRAMCALL
#endif


typedef struct {
	UINT	operate;
	UINT	tramwait;
	UINT	vramwait;
	UINT	grcgwait;
} VRAM_T;

// operate:		bit0	access page
//				bit1	egc enable
//				bit2	grcg bit6
//				bit3	grcg bit7
//				bit4	analog enable

enum {
	VOP_ACCESSBIT	= 0x01,
	VOP_EGCBIT		= 0x02,
	VOP_GRCGBIT		= 0x0c,
	VOP_ANALOGBIT	= 0x10,

	VOP_ACCESSMASK	= ~(0x01),
	VOP_EGCMASK		= ~(0x02),
	VOP_GRCGMASK	= ~(0x0c),
	VOP_ANALOGMASK	= ~(0x10)
};


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 vramplaneseg[4];

extern	VRAM_T	vramop;
extern	BYTE	tramupdate[0x1000];
extern	BYTE	vramupdate[0x8000];

void vram_init(void);

#ifdef __cplusplus
}
#endif

