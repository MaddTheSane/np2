
enum {
	TEXTXMAX		= 80,
	TEXTYMAX		= 400,

	TXTATR_ST		= 0x01,		// ~シークレット
	TXTATR_BL		= 0x02,		// ブリンク
	TXTATR_RV		= 0x04,		// リバース
	TXTATR_UL		= 0x08,		// アンダーライン
	TXTATR_VL		= 0x10,		// バーチカルライン
	TXTATR_BG		= 0x10,		// 簡易グラフ
	TEXTATR_RGB		= 0xe0		// ビット並びはGRBの順
};

typedef struct {
	BYTE	timing;
	BYTE	count;
	BYTE	renewal;
	BYTE	gaiji;
	BYTE	attr;
	BYTE	curdisp;
	BYTE	curdisplast;
	BYTE	blink;
	BYTE	blinkdisp;
	UINT16	curpos;
} TRAM_T;


#ifdef __cplusplus
extern "C" {
#endif

extern	TRAM_T	tramflag;

void maketext_reset(void);
void maketext_init(void);
BYTE maketext_curblink(void);
void maketext(int text_renewal);
void maketext40(int text_renewal);

#ifdef __cplusplus
}
#endif

