

// ---- screen

　PC-9801シリーズの画面サイズは標準で 641x400。
　VGAでは収まらないので 強制的にVGAに収める為に 画面横サイズは width + extend
とする。
　8 < width < 640
  8 < height < 480
  extend = 0 or 1

typedef struct {
	BYTE	*ptr;		// VRAMポインタ
	int		xalign;		// x方向オフセット
	int		yalign;		// y方向オフセット
	int		width;		// 横幅
	int		height;		// 縦幅
	UINT	bpp;		// スクリーン色ビット
	int		extend;		// 幅拡張
} SCRNSURF;

　サーフェスサイズは (width + extern) x height。


const SCRNSURF *scrnmng_surflock(void);
　画面描画開始

void scrnmng_surfunlock(const SCRNSURF *surf);
　画面描画終了(このタイミングで描画)


void scrnmng_setwidth(int posx, int width)
void scrnmng_setextend(int extend)
void scrnmng_setheight(int posy, int height)
　描画サイズの変更
　ウィンドウサイズの変更する
　フルスクリーン中であれば 表示領域を変更。
　SCRNSURFではこの値を返すようにする


BOOL scrnmng_isfullscreen(void)
　フルスクリーン状態の取得
　　return: 非0でフルスクリーン

BOOL scrnmng_haveextend(void)
　横幅状態の取得
　　return: 非0で 横幅拡張サポート

UINT scrnmng_getbpp(void)
　スクリーン色ビット数の取得
　　return: ビット数(8/16/24/32)

void scrnmng_palchanged(void)
　パレット更新の通知(8bitスクリーンサポート時のみ)

UINT16 scrnmng_makepal16(RGB32 pal32)
　RGB32から 16bit色を作成する。(16bitスクリーンにサポート時のみ)



// ---- sound

NP2のサウンドデータは sound.cの以下の関数より取得
  const SINT32 *sound_pcmlock(void)
  void sound_pcmunlock(const SINT32 *hdl)


UINT soundmng_create(UINT rate, UINT ms)
　サウンドストリームの確保
    input:  rate    サンプリングレート(11025/22050/44100)
            ms      サンプリングバッファサイズ(ミリ秒)
　　return: 獲得したバッファのサンプリング数

void soundmng_destroy(void)
　サウンドストリームの終了

void soundmng_reset(void)
　サウンドストリームのリセット

void soundmng_play(void)
　サウンドストリームの再生

void soundmng_stop(void)
　サウンドストリームの停止

void soundmng_sync(void)
　サウンドストリームのコールバック

void soundmng_setreverse(BOOL reverse)
　サウンドストリームの出力反転設定
    input:  reverse 非0で左右反転

BOOL soundmng_pcmplay(UINT num, BOOL loop)
　PCM再生
    input:  num     PCM番号
            loop    非0でループ

void soundmng_pcmstop(UINT num)
　PCM停止
    input:  num     PCM番号



// ---- mouse

BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear)
  マウスの状態取得
    input:  clear   非0で 状態を取得後にカウンタをリセットする
    output: *x      clearからのx方向カウント
            *y      clearからのy方向カウント
    return: bit7    左ボタンの状態 (0:押下)
            bit5    右ボタンの状態 (0:押下)



// ---- serial/parallel/midi

COMMNG commng_create(UINT device)
  シリアルオープン
    input:  デバイス番号
    return: ハンドル (失敗時NULL)


void commng_destroy(COMMNG hdl)
  シリアルクローズ
    input:  ハンドル (失敗時NULL)



// ---- joy stick

BYTE joymng_getstat(void)
　ジョイスティックの状態取得

　　return: bit0    上ボタンの状態 (0:押下)
            bit1    下ボタンの状態
            bit2    左ボタンの状態
            bit3    右ボタンの状態
            bit4    連射ボタン１の状態
            bit5    連射ボタン２の状態
            bit6    ボタン１の状態
            bit7    ボタン２の状態



// ----

void sysmng_update(UINT bitmap)
　状態が変化した場合にコールされる。

void sysmng_cpureset(void)
　リセット時にコールされる



void taskmng_exit(void)
　システムを終了する。

