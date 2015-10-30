/**
 * @file	dclock.cpp
 * @brief
 */

#include "compiler.h"
#include "dclock.h"
#include "parts.h"
#include "np2.h"
#include "scrnmng.h"
#include "timemng.h"
#include "scrndraw.h"
#include "palettes.h"


	_DCLOCK		dclock;
	DCLOCKPAL	dclockpal;


// ------------------------------------------------------------------ font1

static const UINT8 clockchr1[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78,},
			{0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,},
			{0x78, 0xcc, 0xcc, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0xfc,},
			{0xfc, 0x18, 0x30, 0x70, 0x18, 0x0c, 0x0c, 0xcc, 0x78,},
			{0x18, 0x38, 0x78, 0xd8, 0xd8, 0xfc, 0x18, 0x18, 0x18,},
			{0xfc, 0xc0, 0xc0, 0xf8, 0x0c, 0x0c, 0x0c, 0x8c, 0x78,},
			{0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x78,},
			{0xfc, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x30, 0x30, 0x30,},
			{0x78, 0xcc, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0xcc, 0x78,},
			{0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70,}};

static const DCPOS dclockpos1[6] = {
						{dclock.dat + 0, (UINT16)(~0x00fc), 0, 0},
						{dclock.dat + 0, (UINT16)(~0xf801), 7, 0},
						{dclock.dat + 2, (UINT16)(~0x801f), 3, 0},
						{dclock.dat + 3, (UINT16)(~0x003f), 2, 0},
						{dclock.dat + 4, (UINT16)(~0xf003), 6, 0},
						{dclock.dat + 5, (UINT16)(~0xe007), 5, 0}};

static void resetfont1(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00008001;
		}
		else {
			pat = 0x30008001;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 4 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 5 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 9 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------ font2

// 5x9
static const UINT8 clockchr2[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x30, 0x48, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x10, 0x30, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,},
			{0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8,},
			{0x7c, 0x08, 0x10, 0x30, 0x10, 0x08, 0x08, 0x90, 0x60,},
			{0x20, 0x40, 0x40, 0x88, 0x88, 0x90, 0x78, 0x10, 0x20,},
			{0x3c, 0x20, 0x20, 0x70, 0x08, 0x08, 0x08, 0x90, 0x60,},
			{0x10, 0x10, 0x20, 0x70, 0x48, 0x88, 0x88, 0x90, 0x60,},
			{0x7c, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40,},
			{0x38, 0x44, 0x44, 0x48, 0x30, 0x48, 0x88, 0x88, 0x70,},
			{0x18, 0x24, 0x40, 0x44, 0x48, 0x38, 0x10, 0x20, 0x20,}};

static const DCPOS dclockpos2[6] = {
						{dclock.dat + 0, (UINT16)(~0x00fc), 0, 0},
						{dclock.dat + 0, (UINT16)(~0xf003), 6, 0},
						{dclock.dat + 2, (UINT16)(~0x00fc), 0, 0},
						{dclock.dat + 2, (UINT16)(~0xf003), 6, 0},
						{dclock.dat + 4, (UINT16)(~0x00fc), 0, 0},
						{dclock.dat + 4, (UINT16)(~0xf003), 6, 0}};

static void resetfont2(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000002;
		}
		else {
			pat = 0x00020002;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 4 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 5 * DCLOCK_YALIGN)) = pat;
		pat <<= 1;
		*(UINT32 *)(dclock.dat + 1 + ( 9 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------ font3

// 4x9
static const UINT8 clockchr3[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60,},
			{0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,},
			{0x60, 0x90, 0x90, 0x10, 0x20, 0x40, 0x40, 0x80, 0xf0,},
			{0x60, 0x90, 0x90, 0x10, 0x60, 0x10, 0x90, 0x90, 0x60,},
			{0x20, 0x60, 0x60, 0xa0, 0xa0, 0xa0, 0xf0, 0x20, 0x20,},
			{0xf0, 0x80, 0x80, 0xe0, 0x90, 0x10, 0x90, 0x90, 0x60,},
			{0x60, 0x90, 0x90, 0x80, 0xe0, 0x90, 0x90, 0x90, 0x60,},
			{0xf0, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,},
			{0x60, 0x90, 0x90, 0x90, 0x60, 0x90, 0x90, 0x90, 0x60,},
			{0x60, 0x90, 0x90, 0x90, 0x70, 0x10, 0x90, 0x90, 0x60,}};

static const DCPOS dclockpos3[6] = {
						{dclock.dat + 0, (UINT16)(~0x00f0), 0, 0},
						{dclock.dat + 0, (UINT16)(~0x8007), 5, 0},
						{dclock.dat + 1, (UINT16)(~0xc003), 6, 0},
						{dclock.dat + 2, (UINT16)(~0x001e), 3, 0},
						{dclock.dat + 3, (UINT16)(~0x000f), 4, 0},
						{dclock.dat + 4, (UINT16)(~0x0078), 1, 0}};

static void resetfont3(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000010;
		}
		else {
			pat = 0x00400010;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 4 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 5 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 9 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------ font4

// 5x8
static const UINT8 clockchr4[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70,},
			{0x00, 0x70, 0x88, 0x08, 0x08, 0x30, 0x40, 0x88, 0xf8,},
			{0x00, 0x70, 0x88, 0x08, 0x30, 0x08, 0x08, 0x08, 0xf0,},
			{0x00, 0x10, 0x30, 0x50, 0x50, 0x90, 0xf8, 0x10, 0x10,},
			{0x00, 0x38, 0x40, 0x60, 0x10, 0x08, 0x08, 0x08, 0xf0,},
			{0x00, 0x18, 0x20, 0x40, 0xb0, 0xc8, 0x88, 0x88, 0x70,},
			{0x00, 0x70, 0x88, 0x88, 0x10, 0x10, 0x10, 0x20, 0x20,},
			{0x00, 0x70, 0x88, 0x88, 0x70, 0x50, 0x88, 0x88, 0x70,},
			{0x00, 0x70, 0x88, 0x88, 0x88, 0x78, 0x10, 0x20, 0xc0,}};

static void resetfont4(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000004;
		}
		else {
			pat = 0x00040004;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 5 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 6 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 9 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------ font5

static const UINT8 clockchr5[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20,},
			{0x00, 0x00, 0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xf8,},
			{0x00, 0x00, 0xf8, 0x10, 0x20, 0x10, 0x08, 0x88, 0x70,},
			{0x00, 0x00, 0x30, 0x50, 0x50, 0x90, 0xf8, 0x10, 0x10,},
			{0x00, 0x00, 0xf8, 0x80, 0xf0, 0x08, 0x08, 0x88, 0x70,},
			{0x00, 0x00, 0x30, 0x40, 0xf0, 0x88, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0xf8, 0x08, 0x10, 0x20, 0x20, 0x40, 0x40,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70,},
			{0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x78, 0x10, 0x60,}};

static const DCPOS dclockpos5[6] = {
						{dclock.dat + 0, (UINT16)(~0x00f8), 0, 0},
						{dclock.dat + 0, (UINT16)(~0xe003), 6, 0},
						{dclock.dat + 2, (UINT16)(~0x007c), 1, 0},
						{dclock.dat + 2, (UINT16)(~0xf001), 7, 0},
						{dclock.dat + 4, (UINT16)(~0x003e), 2, 0},
						{dclock.dat + 5, (UINT16)(~0x00f8), 0, 0}};

static void resetfont5(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000006;
		}
		else {
			pat = 0x00030006;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 6 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 7 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + ( 9 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------ font6

// 4x6
static const UINT8 clockchr6[11][16] = {
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x10, 0x20, 0x40, 0xf0,},
			{0x00, 0x00, 0x00, 0xf0, 0x20, 0x60, 0x10, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x40, 0x80, 0xa0, 0xa0, 0xf0, 0x20,},
			{0x00, 0x00, 0x00, 0xf0, 0x80, 0x60, 0x10, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x40, 0x80, 0xe0, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0xe0, 0x10, 0x10, 0x20, 0x20, 0x40,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x60, 0x90, 0x90, 0x60,},
			{0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x70, 0x20, 0x40,}};

static const DCPOS dclockpos6[6] = {
						{dclock.dat + 0, (UINT16)(~0x00f0), 0, 0},
						{dclock.dat + 0, (UINT16)(~0x8007), 5, 0},
						{dclock.dat + 1, (UINT16)(~0x000f), 4, 0},
						{dclock.dat + 2, (UINT16)(~0x0078), 1, 0},
						{dclock.dat + 3, (UINT16)(~0x00f0), 0, 0},
						{dclock.dat + 3, (UINT16)(~0x8007), 5, 0}};

static void resetfont6(void) {

	UINT32	pat;

	if (np2oscfg.clk_x) {
		if (np2oscfg.clk_x <= 4) {
			pat = 0x00000020;
		}
		else {
			pat = 0x00000220;
		}
		*(UINT32 *)(dclock.dat + 1 + ( 8 * DCLOCK_YALIGN)) = pat;
		*(UINT32 *)(dclock.dat + 1 + (10 * DCLOCK_YALIGN)) = pat;
	}
}


// ------------------------------------------------------------------------

typedef struct {
const UINT8	*fnt;
const DCPOS	*pos;
	void	(*init)(void);
} DCLOCKFNT;

static const DCLOCKFNT fonttype[] =
					{{clockchr1[0], dclockpos1, resetfont1},
					 {clockchr2[0], dclockpos2, resetfont2},
					 {clockchr3[0], dclockpos3, resetfont3},
					 {clockchr4[0], dclockpos2, resetfont4},
					 {clockchr5[0], dclockpos5, resetfont5},
					 {clockchr6[0], dclockpos6, resetfont6}};


// ------------------------------------------------------------------------

/**
 * コンストラクタ
 */
DispClock::DispClock()
{
	ZeroMemory(this, sizeof(*this));
}

/**
 * リセット
 */
void DispClock::Reset()
{
	ZeroMemory(this, sizeof(*this));

	if (np2oscfg.clk_x)
	{
		if (np2oscfg.clk_x <= 4)
		{
			np2oscfg.clk_x = 4;
		}
		else if (np2oscfg.clk_x <= 6)
		{
			np2oscfg.clk_x = 6;
		}
		else
		{
			np2oscfg.clk_x = 0;
		}
	}
	if (np2oscfg.clk_fnt >= NELEMENTS(fonttype))
	{
		np2oscfg.clk_fnt = 0;
	}

	m_cCharaters = np2oscfg.clk_x;

	const DCLOCKFNT* fnt = fonttype + np2oscfg.clk_fnt;
	m_pFont = fnt->fnt;
	this->pos = fnt->pos;
	fnt->init();
	dclock_callback();
	dclock_redraw();
}

/**
 * 更新
 */
void DispClock::Update()
{
	if ((scrnmng_isfullscreen()) && (m_cCharaters))
	{
		_SYSTIME st;
		timemng_gettime(&st);

		UINT8 buf[6];
		buf[0] = (st.hour / 10) + 1;
		buf[1] = (st.hour % 10) + 1;
		buf[2] = (st.minute / 10) + 1;
		buf[3] = (st.minute % 10) + 1;
		if (m_cCharaters > 4)
		{
			buf[4] = (st.second / 10) + 1;
			buf[5] = (st.second % 10) + 1;
		}

		UINT8 count = 13;
		for (int i = m_cCharaters; i--; )
		{
			if (m_cTime[i] != buf[i])
			{
				m_cTime[i] = buf[i];
				m_nCounter.b[i] = count;
				m_cDirty |= (1 << i);
				count += 4;
			}
		}
	}
}

/**
 * 再描画
 */
void DispClock::Redraw()
{
	m_cDirty = 0x3f;
}

/**
 * 描画が必要?
 * @retval true 描画中
 * @retval false 非描画
 */
bool DispClock::IsDisplayed() const
{
	return ((m_cDirty != 0) || (m_nCounter.q != 0));
}

static const UINT8 s_dclocky[13] = {0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3};

/**
 *
 */
UINT8 DispClock::CountPos(UINT nCount)
{
	if (nCount < _countof(s_dclocky))
	{
		return s_dclocky[nCount];
	}
	else
	{
		return 255;
	}
}

/**
 * カウントダウン処理
 * @param[in] nFrames フレーム数
 */
void DispClock::CountDown(UINT nFrames)
{
	if ((m_cCharaters == 0) || (m_nCounter.q == 0))
	{
		return;
	}

	if (nFrames == 0)
	{
		nFrames = 1;
	}

	for (UINT i = 0; i < m_cCharaters; i++)
	{
		UINT nRemain = m_nCounter.b[i];
		if (nRemain == 0)
		{
			continue;
		}
		const UINT8 y = CountPos(nRemain);

		if (nFrames < nRemain)
		{
			nRemain -= nFrames;
		}
		else
		{
			nRemain = 0;
		}
		if (y != CountPos(nRemain))
		{
			m_cDirty |= (1 << i);
		}
		m_nCounter.b[i] = nRemain;
	}
}

/**
 * バッファに描画
 * @retval true 更新あり
 * @retval false 更新なし
 */
bool DispClock::Make()
{
	if ((m_cCharaters == 0) || (m_cDirty == 0))
	{
		return false;
	}

	for (UINT i = 0; i < m_cCharaters; i++)
	{
		if ((m_cDirty & (1 << i)) == 0)
		{
			continue;
		}

		UINT nNumber = m_cTime[i];
		UINT nPadding = 3;

		const UINT nRemain = m_nCounter.b[i];
		if (nRemain == 0)
		{
			this->bak[i] = nNumber;
		}
		else if (nRemain < _countof(s_dclocky))
		{
			nPadding -= s_dclocky[nRemain];
		}
		else
		{
			nNumber = this->bak[i];
		}

		const DCPOS* eax = &this->pos[i];
		UINT8* q = eax->pos;
		const UINT16 mask = eax->mask;
		const UINT8 rolbit = eax->rolbit;
		for (UINT y = 0; y < nPadding; y++)
		{
			*(reinterpret_cast<UINT16*>(q)) &= mask;
			q += DCLOCK_YALIGN;
		}

		const UINT8* p = m_pFont + (nNumber * 16);
		for (UINT y = 0; y < 9; y++)
		{
			*(reinterpret_cast<UINT16*>(q)) &= mask;
			q[0] |= (p[y] >> rolbit);
			q[1] |= (p[y] << (8 - rolbit));
			q += DCLOCK_YALIGN;
		}
	}
	m_cDirty = 0;
	return true;
}

/**
 * 描画
 * @param[in] nBpp 色数
 * @param[out] ptr 描画ポインタ
 * @param[int] nYAlign アライメント
 */
void DispClock::Draw(UINT nBpp, void* lpBuffer, int nYAlign) const
{
	switch (nBpp)
	{
		case 8:
			Draw8(lpBuffer, nYAlign);
			break;

		case 16:
			Draw16(lpBuffer, nYAlign);
			break;

		case 24:
			Draw24(lpBuffer, nYAlign);
			break;

		case 32:
			Draw32(lpBuffer, nYAlign);
			break;
	}
}

/**
 * 描画(8bpp)
 * @param[out] ptr 描画ポインタ
 * @param[int] nYAlign アライメント
 */
void DispClock::Draw8(void* lpBuffer, int nYAlign) const
{
	const UINT8* p = this->dat;

	for (UINT i = 0; i < 4; i++)
	{
		const UINT32* pPattern = dclockpal.pal8[i];
		for (UINT j = 0; j < 3; j++)
		{
			for (UINT x = 0; x < DCLOCK_YALIGN; x++)
			{
				(static_cast<UINT32*>(lpBuffer))[x * 2 + 0] = pPattern[p[x] >> 4];
				(static_cast<UINT32*>(lpBuffer))[x * 2 + 1] = pPattern[p[x] & 15];
			}
			p += DCLOCK_YALIGN;
			lpBuffer = reinterpret_cast<void*>(reinterpret_cast<INTPTR>(lpBuffer) + nYAlign);
		}
	}
}

/**
 * 描画(16bpp)
 * @param[out] ptr 描画ポインタ
 * @param[int] nYAlign アライメント
 */
void DispClock::Draw16(void* lpBuffer, int nYAlign) const
{
	const UINT8* p = this->dat;

	for (UINT i = 0; i < 4; i++)
	{
		const RGB16 pal = dclockpal.pal16[i];
		for (UINT j = 0; j < 3; j++)
		{
			for (UINT x = 0; x < (8 * DCLOCK_YALIGN); x++)
			{
				(static_cast<UINT16*>(lpBuffer))[x] = (p[x >> 3] & (0x80 >> (x & 7))) ? pal : 0;
			}
			p += DCLOCK_YALIGN;
			lpBuffer = reinterpret_cast<void*>(reinterpret_cast<INTPTR>(lpBuffer) + nYAlign);
		}
	}
}

/**
 * 描画(24bpp)
 * @param[out] ptr 描画ポインタ
 * @param[int] nYAlign アライメント
 */
void DispClock::Draw24(void* lpBuffer, int nYAlign) const
{
	const UINT8* p = this->dat;
	UINT8* q = static_cast<UINT8*>(lpBuffer);

	for (UINT i = 0; i < 4; i++)
	{
		const RGB32 pal = dclockpal.pal32[i];
		for (UINT j = 0; j < 3; j++)
		{
			for (UINT x = 0; x < (8 * DCLOCK_YALIGN); x++)
			{
				if (p[x >> 3] & (0x80 >> (x & 7)))
				{
					q[0] = pal.p.b;
					q[1] = pal.p.g;
					q[2] = pal.p.g;
				}
				else
				{
					q[0] = 0;
					q[1] = 1;
					q[2] = 2;
				}
				q += 3;
			}
			p += DCLOCK_YALIGN;
			q += nYAlign - (8 * DCLOCK_YALIGN) * 3;
		}
	}
}

/**
 * 描画(32bpp)
 * @param[out] ptr 描画ポインタ
 * @param[int] nYAlign アライメント
 */
void DispClock::Draw32(void* lpBuffer, int nYAlign) const
{
	const UINT8* p = this->dat;

	for (UINT i = 0; i < 4; i++)
	{
		const UINT32 pal = dclockpal.pal32[i].d;
		for (UINT j = 0; j < 3; j++)
		{
			for (UINT x = 0; x < (8 * DCLOCK_YALIGN); x++)
			{
				(static_cast<UINT32*>(lpBuffer))[x] = (p[x >> 3] & (0x80 >> (x & 7))) ? pal : 0;
			}
			p += DCLOCK_YALIGN;
			lpBuffer = reinterpret_cast<void*>(reinterpret_cast<INTPTR>(lpBuffer) + nYAlign);
		}
	}
}



// ----

/**
 * 初期化
 */
void DispClockPalette::Initialize()
{
	::pal_makegrad(this->pal32, 4, np2oscfg.clk_color1, np2oscfg.clk_color2);
}

/**
 * パレット設定
 * @param[in] bpp 色
 */
void DispClockPalette::SetPalette(UINT bpp)
{
	switch (bpp)
	{
		case 8:
			SetPalette8();
			break;

		case 16:
			SetPalette16();
			break;
	}
}

/**
 * 8bpp パレット設定
 */
void DispClockPalette::SetPalette8()
{
	for (UINT i = 0; i < 16; i++)
	{
		UINT nBits = 0;
		for (UINT j = 1; j < 0x10; j <<= 1)
		{
			nBits <<= 8;
			if (i & j)
			{
				nBits |= 1;
			}
		}
		for (UINT j = 0; j < 4; j++)
		{
			this->pal8[j][i] = nBits * (START_PALORG + j);
		}
	}
}

/**
 * 16bpp パレット設定
 */
void DispClockPalette::SetPalette16()
{
	for (UINT i = 0; i < 4; i++)
	{
		this->pal16[i] = scrnmng_makepal16(this->pal32[i]);
	}
}
