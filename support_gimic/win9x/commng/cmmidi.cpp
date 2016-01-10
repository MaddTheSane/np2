/**
 * @file	cmmidi.cpp
 * @brief	MIDI クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmmidi.h"
#include "np2.h"
#include "mimpidef.h"

#include "cmmidiin32.h"
#include "cmmidiout32.h"
#if defined(MT32SOUND_DLL)
#include "cmmidioutmt32sound.h"
#endif	// defined(MT32SOUND_DLL)
#if defined(VERMOUTH_LIB)
#include "cmmidioutvermouth.h"
#endif	// defined(VERMOUTH_LIB)
#include "keydisp.h"

#define MIDIOUTS(a, b, c)	(((c) << 16) + (b << 8) + (a))
#define MIDIOUTS2(a)		(*(UINT16 *)(a))
#define MIDIOUTS3(a)		((*(UINT32 *)(a)) & 0xffffff)

const TCHAR cmmidi_midimapper[] = TEXT("MIDI MAPPER");
#if defined(VERMOUTH_LIB)
const TCHAR cmmidi_vermouth[] = TEXT("VERMOUTH");
#endif
#if defined(MT32SOUND_DLL)
const TCHAR cmmidi_mt32sound[] = TEXT("MT32Sound");
#endif

LPCTSTR cmmidi_mdlname[12] = {
		TEXT("MT-32"),	TEXT("CM-32L"),		TEXT("CM-64"),
		TEXT("CM-300"),	TEXT("CM-500(LA)"),	TEXT("CM-500(GS)"),
		TEXT("SC-55"),	TEXT("SC-88"),		TEXT("LA"),
		TEXT("GM"),		TEXT("GS"),			TEXT("XG")};

enum {		MIDI_MT32 = 0,	MIDI_CM32L,		MIDI_CM64,
			MIDI_CM300,		MIDI_CM500LA,	MIDI_CM500GS,
			MIDI_SC55,		MIDI_SC88,		MIDI_LA,
			MIDI_GM,		MIDI_GS,		MIDI_XG,	MIDI_OTHER};

static const UINT8 EXCV_MTRESET[] = {
			0xf0,0x41,0x10,0x16,0x12,0x7f,0x00,0x00,0x00,0x01,0xf7};
static const UINT8 EXCV_GMRESET[] = {
			0xf0,0x7e,0x7f,0x09,0x01,0xf7};
static const UINT8 EXCV_GM2RESET[] = {
			0xf0,0x7e,0x7f,0x09,0x03,0xf7};
static const UINT8 EXCV_GSRESET[] = {
			0xf0,0x41,0x10,0x42,0x12,0x40,0x00,0x7f,0x00,0x41,0xf7};
static const UINT8 EXCV_XGRESET[] = {
			0xf0,0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0xf7};

enum {
	MIDI_EXCLUSIVE		= 0xf0,
	MIDI_TIMECODE		= 0xf1,
	MIDI_SONGPOS		= 0xf2,
	MIDI_SONGSELECT		= 0xf3,
	MIDI_CABLESELECT	= 0xf5,
	MIDI_TUNEREQUEST	= 0xf6,
	MIDI_EOX			= 0xf7,
	MIDI_TIMING			= 0xf8,
	MIDI_START			= 0xfa,
	MIDI_CONTINUE		= 0xfb,
	MIDI_STOP			= 0xfc,
	MIDI_ACTIVESENSE	= 0xfe,
	MIDI_SYSTEMRESET	= 0xff
};

enum {
	MIDI_BUFFER			= (1 << 10),

	MIDICTRL_READY		= 0,
	MIDICTRL_2BYTES,
	MIDICTRL_3BYTES,
	MIDICTRL_EXCLUSIVE,
	MIDICTRL_TIMECODE,
	MIDICTRL_SYSTEM
};

struct _cmmidi;
typedef struct _cmmidi	_CMMIDI;
typedef struct _cmmidi	*CMMIDI;

typedef struct {
	UINT8	prog;
	UINT8	press;
	UINT16	bend;
	UINT8	ctrl[28];
} _MIDICH, *MIDICH;

struct _cmmidi {
	CComMidiIn32* m_pMidiIn;
	CComMidiOut* m_pMidiOut;

	UINT		midictrl;
	UINT		midisyscnt;
	UINT		mpos;

	UINT8		midilast;
	UINT8		midiexcvwait;
	UINT8		midimodule;

	UINT8		buffer[MIDI_BUFFER];
	_MIDICH		mch[16];

	UINT8		def_en;
	MIMPIDEF	def;
};

static const UINT8 midictrltbl[] = { 0, 1, 5, 7, 10, 11, 64,
									65, 66, 67, 84, 91, 93,
									94,						// for SC-88
									71, 72, 73, 74};		// for XG

static	UINT8	midictrlindex[128];


// ----

static UINT module2number(const OEMCHAR *module) {

	UINT	i;

	for (i=0; i<NELEMENTS(cmmidi_mdlname); i++) {
		if (!milstr_extendcmp(module, cmmidi_mdlname[i])) {
			break;
		}
	}
	return(i);
}

// ----

static void midiallnoteoff(CMMIDI midi) {

	UINT	i;
	UINT8	msg[4];

	for (i=0; i<0x10; i++) {
		msg[0] = (UINT8)(0xb0 + i);
		msg[1] = 0x7b;
		msg[2] = 0x00;
		keydisp_midi(msg);
		midi->m_pMidiOut->Short(MIDIOUTS3(msg));
	}
}

static void midireset(CMMIDI midi) {

const UINT8	*excv;
	UINT	excvsize;

	switch(midi->midimodule) {
		case MIDI_GM:
			excv = EXCV_GMRESET;
			excvsize = sizeof(EXCV_GMRESET);
			break;

		case MIDI_CM300:
		case MIDI_CM500GS:
		case MIDI_SC55:
		case MIDI_SC88:
		case MIDI_GS:
			excv = EXCV_GSRESET;
			excvsize = sizeof(EXCV_GSRESET);
			break;

		case MIDI_XG:
			excv = EXCV_XGRESET;
			excvsize = sizeof(EXCV_XGRESET);
			break;

		case MIDI_MT32:
		case MIDI_CM32L:
		case MIDI_CM64:
		case MIDI_CM500LA:
		case MIDI_LA:
			excv = EXCV_MTRESET;
			excvsize = sizeof(EXCV_MTRESET);
			break;

		default:
			excv = NULL;
			excvsize = 0;
			break;
	}
	if (excv) {
		midi->m_pMidiOut->Long(excv, excvsize);
	}
	midiallnoteoff(midi);
}

static void midisetparam(CMMIDI midi) {

	UINT8	i;
	UINT	j;
	MIDICH	mch;

	mch = midi->mch;
	for (i=0; i<16; i++, mch++) {
		if (mch->press != 0xff) {
			midi->m_pMidiOut->Short(MIDIOUTS(0xa0+i, mch->press, 0));
		}
		if (mch->bend != 0xffff) {
			midi->m_pMidiOut->Short((mch->bend << 8) + 0xe0+i);
		}
		for (j=0; j<NELEMENTS(midictrltbl); j++) {
			if (mch->ctrl[j+1] != 0xff) {
				midi->m_pMidiOut->Short(MIDIOUTS(0xb0+i, midictrltbl[j], mch->ctrl[j+1]));
			}
		}
		if (mch->prog != 0xff) {
			midi->m_pMidiOut->Short(MIDIOUTS(0xc0+i, mch->prog, 0));
		}
	}
}


// ----

static UINT midiread(CMMIDI midi, UINT8 *data)
{
	if (midi->m_pMidiIn)
	{
		return midi->m_pMidiIn->Read(data);
	}
	return 0;
}

static UINT midiwrite(CMMIDI midi, UINT8 data) {

	MIDICH	mch;
	int		type;

	switch(data) {
		case MIDI_TIMING:
		case MIDI_START:
		case MIDI_CONTINUE:
		case MIDI_STOP:
		case MIDI_ACTIVESENSE:
		case MIDI_SYSTEMRESET:
			return(1);
	}
	if (midi->midictrl == MIDICTRL_READY) {
		if (data & 0x80) {
			midi->mpos = 0;
			switch(data & 0xf0) {
				case 0xc0:
				case 0xd0:
					midi->midictrl = MIDICTRL_2BYTES;
					break;

				case 0x80:
				case 0x90:
				case 0xa0:
				case 0xb0:
				case 0xe0:
					midi->midictrl = MIDICTRL_3BYTES;
					midi->midilast = data;
					break;

				default:
					switch(data) {
						case MIDI_EXCLUSIVE:
							midi->midictrl = MIDICTRL_EXCLUSIVE;
							break;

						case MIDI_TIMECODE:
							midi->midictrl = MIDICTRL_TIMECODE;
							break;

						case MIDI_SONGPOS:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 3;
							break;

						case MIDI_SONGSELECT:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 2;
							break;

						case MIDI_CABLESELECT:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 1;
							break;

//						case MIDI_TUNEREQUEST:
//						case MIDI_EOX:
						default:
							return(1);
					}
					break;
			}
		}
		else {						// Key-onのみな気がしたんだけど忘れた…
			// running status
			midi->buffer[0] = midi->midilast;
			midi->mpos = 1;
			midi->midictrl = MIDICTRL_3BYTES;
		}
	}
	midi->buffer[midi->mpos] = data;
	midi->mpos++;

	switch(midi->midictrl) {
		case MIDICTRL_2BYTES:
			if (midi->mpos >= 2) {
				midi->buffer[1] &= 0x7f;
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xa0:
						mch->press = midi->buffer[1];
						break;

					case 0xc0:
						if (midi->def_en) {
							type = midi->def.ch[midi->buffer[0] & 0x0f];
							if (type < MIMPI_RHYTHM) {
								midi->buffer[1] = 
										midi->def.map[type][midi->buffer[1]];
							}
						}
						mch->prog = midi->buffer[1];
						break;
				}
				keydisp_midi(midi->buffer);
				midi->m_pMidiOut->Short(MIDIOUTS2(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(2);
			}
			break;

		case MIDICTRL_3BYTES:
			if (midi->mpos >= 3) {
				*(UINT16 *)(midi->buffer + 1) &= 0x7f7f;
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xb0:
						if (midi->buffer[1] == 123) {
							mch->press = 0;
							mch->bend = 0x4000;
							mch->ctrl[1+1] = 0;			// Modulation
							mch->ctrl[5+1] = 127;		// Explession
							mch->ctrl[6+1] = 0;			// Hold
							mch->ctrl[7+1] = 0;			// Portament
							mch->ctrl[8+1] = 0;			// Sostenute
							mch->ctrl[9+1] = 0;			// Soft
						}
						else {
							mch->ctrl[midictrlindex[midi->buffer[1]]]
															= midi->buffer[2];
						}
						break;

					case 0xe0:
						mch->bend = *(UINT16 *)(midi->buffer + 1);
						break;
				}
				keydisp_midi(midi->buffer);
				midi->m_pMidiOut->Short(MIDIOUTS3(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(3);
			}
			break;

		case MIDICTRL_EXCLUSIVE:
			if (data == MIDI_EOX) {
				midi->m_pMidiOut->Long(midi->buffer, midi->mpos);
				midi->midictrl = MIDICTRL_READY;
				return(midi->mpos);
			}
			else if (midi->mpos >= MIDI_BUFFER) {		// おーばーふろー
				midi->midictrl = MIDICTRL_READY;
			}
			break;

		case MIDICTRL_TIMECODE:
			if (midi->mpos >= 2) {
				if ((data == 0x7e) || (data == 0x7f)) {
					// exclusiveと同じでいい筈…
					midi->midictrl = MIDICTRL_EXCLUSIVE;
				}
				else {
					midi->midictrl = MIDICTRL_READY;
					return(2);
				}
			}
			break;

		case MIDICTRL_SYSTEM:
			if (midi->mpos >= midi->midisyscnt) {
				midi->midictrl = MIDICTRL_READY;
				return(midi->midisyscnt);
			}
			break;
	}
	return(0);
}

static INTPTR midimsg(CMMIDI midi, UINT msg, INTPTR param) {

	COMFLAG	flag;

	switch(msg) {
		case COMMSG_MIDIRESET:
			midireset(midi);
			return(1);

		case COMMSG_SETFLAG:
			flag = (COMFLAG)param;
			if ((flag) &&
				(flag->size == sizeof(_COMFLAG) + sizeof(midi->mch)) &&
				(flag->sig == COMSIG_MIDI)) {
				CopyMemory(midi->mch, flag + 1, sizeof(midi->mch));
				midisetparam(midi);
				return(1);
			}
			break;

		case COMMSG_GETFLAG:
			flag = (COMFLAG)_MALLOC(sizeof(_COMFLAG) + sizeof(midi->mch),
																"MIDI FLAG");
			if (flag) {
				flag->size = sizeof(_COMFLAG) + sizeof(midi->mch);
				flag->sig = COMSIG_MIDI;
				flag->ver = 0;
				flag->param = 0;
				CopyMemory(flag + 1, midi->mch, sizeof(midi->mch));
				return((INTPTR)flag);
			}
			break;

		case COMMSG_MIMPIDEFFILE:
			mimpidef_load(&midi->def, (OEMCHAR *)param);
			return(1);

		case COMMSG_MIMPIDEFEN:
			midi->def_en = (param)?TRUE:FALSE;
			return(1);
	}
	return(0);
}

static void midirelease(CMMIDI midi) {

	midiallnoteoff(midi);
	delete midi->m_pMidiOut;
	if (midi->m_pMidiIn)
	{
		delete midi->m_pMidiIn;
	}
}


// ----

void cmmidi_initailize(void) {

	UINT	i;

	ZeroMemory(midictrlindex, sizeof(midictrlindex));
	for (i=0; i<NELEMENTS(midictrltbl); i++) {
		midictrlindex[midictrltbl[i]] = (UINT8)(i + 1);
	}
	midictrlindex[32] = 1;
}

CMMIDI cmmidi_create(LPCTSTR midiout, LPCTSTR midiin, LPCTSTR module)
{
	CComMidiOut* pMidiOut = NULL;
#if defined(VERMOUTH_LIB)
	if ((pMidiOut == NULL) && (!milstr_cmp(midiout, cmmidi_vermouth)))
	{
		pMidiOut = CComMidiOutVermouth::CreateInstance();
	}
#endif	// defined(VERMOUTH_LIB)
#if defined(MT32SOUND_DLL)
	if ((pMidiOut == NULL) && (!milstr_cmp(midiout, cmmidi_mt32sound)))
	{
		pMidiOut = CComMidiOutMT32Sound::CreateInstance();
	}
#endif	// defined(MT32SOUND_DLL)
	if (pMidiOut == NULL)
	{
		pMidiOut = CComMidiOut32::CreateInstance(midiout);
	}

	CComMidiIn32* pMidiIn = CComMidiIn32::CreateInstance(midiin);

	if ((!pMidiOut) && (!pMidiIn))
	{
		return NULL;
	}

	if (pMidiOut == NULL)
	{
		pMidiOut = new CComMidiOut;
	}

	CMMIDI midi = new _cmmidi;
	ZeroMemory(midi, sizeof(*midi));
	midi->m_pMidiIn = pMidiIn;
	midi->m_pMidiOut = pMidiOut;
	midi->midictrl = MIDICTRL_READY;
//	midi->midisyscnt = 0;
//	midi->mpos = 0;

	midi->midilast = 0x80;
//	midi->midiexcvwait = 0;
	midi->midimodule = (UINT8)module2number(module);
	FillMemory(midi->mch, sizeof(midi->mch), 0xff);
	return midi;
}

// ---- クラス

/**
 * インスタンス作成
 * @param[in] lpMidiOut MIDIOUT デバイス
 * @param[in] lpMidiIn MIDIIN デバイス
 * @param[in] lpModule モジュール
 * @return インスタンス
 */
CComMidi* CComMidi::CreateInstance(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule)
{
	CComMidi* pMidi = new CComMidi;
	if (!pMidi->Initialize(lpMidiOut, lpMidiIn, lpModule))
	{
		delete pMidi;
		pMidi = NULL;
	}
	return pMidi;
}

/**
 * コンストラクタ
 */
CComMidi::CComMidi()
	: CComBase(COMCONNECT_MIDI)
	, m_pMidi(NULL)
{
}

/**
 * デストラクタ
 */
CComMidi::~CComMidi()
{
	if (m_pMidi)
	{
		midirelease(m_pMidi);
		delete m_pMidi;
	}
}

/**
 * 初期化
 * @param[in] lpMidiOut MIDIOUT デバイス
 * @param[in] lpMidiIn MIDIIN デバイス
 * @param[in] lpModule モジュール
 * @retval true 成功
 * @retval false 失敗
 */
bool CComMidi::Initialize(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule)
{
	m_pMidi = cmmidi_create(lpMidiOut, lpMidiIn, lpModule);
	return (m_pMidi != NULL);
}

/**
 * 読み込み
 * @param[out] pData バッファ
 * @return サイズ
 */
UINT CComMidi::Read(UINT8* pData)
{
	return midiread(m_pMidi, pData);
}

/**
 * 書き込み
 * @param[out] cData データ
 * @return サイズ
 */
UINT CComMidi::Write(UINT8 cData)
{
	return midiwrite(m_pMidi, cData);
}

/**
 * ステータスを得る
 * @return ステータス
 */
UINT8 CComMidi::GetStat()
{
	return 0x00;
}

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParam パラメタ
 * @return リザルト コード
 */
INTPTR CComMidi::Message(UINT nMessage, INTPTR nParam)
{
	return midimsg(m_pMidi, nMessage, nParam);
}
