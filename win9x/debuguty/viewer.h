#define	NP2VIEW_MAX	8

typedef struct {
	BYTE	vram;
	BYTE	itf;
	BYTE	A20;
} VIEWMEM_T;

enum {
	VIEWMODE_REG = 0,
	VIEWMODE_SEG,
	VIEWMODE_1MB,
	VIEWMODE_ASM,
	VIEWMODE_SND
};

enum {
	ALLOCTYPE_NONE = 0,
	ALLOCTYPE_REG,
	ALLOCTYPE_SEG,
	ALLOCTYPE_1MB,
	ALLOCTYPE_ASM,
	ALLOCTYPE_SND,

	ALLOCTYPE_ERROR = 0xffffffff
};

typedef struct {
	DWORD	type;
	DWORD	arg;
	DWORD	size;
	void	*ptr;
} VIEWMEMBUF;

typedef struct {
	HWND		hwnd;
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	DWORD		pos;
	DWORD		maxline;
	WORD		step;
	WORD		mul;
	BYTE		alive;
	BYTE		type;
	BYTE		lock;
	BYTE		active;
	WORD		seg;
	WORD		off;
	VIEWMEM_T	dmem;
	SCROLLINFO	si;
} NP2VIEW_T;

extern	NP2VIEW_T	np2view[NP2VIEW_MAX];


BOOL viewer_init(HINSTANCE hPreInst);
void viewer_term(void);

void viewer_open(void);
void viewer_allclose(void);

void viewer_allreload(BOOL force);

