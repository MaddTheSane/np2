//
//  OldCarbHeaders.h
//  Constructor
//
//  Created by C.W. Betts on 6/8/16.
//  Copyright © 2016 Aurbach & Associates, Inc. All rights reserved.
//

#ifndef PPMacho_OldCarbHeaders_h
#define PPMacho_OldCarbHeaders_h
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

#ifdef QD_HEADERS_ARE_PRIVATE

#pragma pack (push, 2)

typedef CALLBACK_API( void , DragGrayRgnProcPtr )(void);
typedef STACK_UPP_TYPE(DragGrayRgnProcPtr)                      DragGrayRgnUPP;

typedef SInt8                           StyledLineBreakCode;
CF_ENUM(StyledLineBreakCode) {
	smBreakWord                   = 0,
	smBreakChar                   = 1,
	smBreakOverflow               = 2
};


enum {
	invalColReq = -1    /*invalid color table request*/
};

typedef UInt32		AVIDType;
typedef AVIDType	DisplayIDType;
typedef UInt32		DisplayModeID;
typedef UInt32		VideoDeviceType;
typedef UInt32		GammaTableID;

enum {
	kFontIDNewYork = 2,
	kFontIDGeneva,
	kFontIDMonaco,
	kFontIDVenice,
	kFontIDLondon,
	kFontIDAthens,
	kFontIDSanFrancisco,
	kFontIDToronto,
	kFontIDCairo = 11,
	kFontIDLosAngeles,
	kFontIDTimes = 20,
	kFontIDHelvetica,
	kFontIDCourier,
	kFontIDSymbol,
	kFontIDMobile
};

enum {
	systemFont = 0,
	applFont
};

enum {
	commandMark	= 17,
	checkMark,
	diamondMark,
	appleMark
};

enum {
	/* transfer modes */
	srcOr			= 1,
	srcXor,
	srcBic,
	notSrcCopy,
	notSrcOr,
	notSrcXor,
	notSrcBic,
	patCopy,
	patOr,
	patXor,
	patBic,
	notPatCopy,
	notPatOr,
	notPatXor,
	notPatBic,							/* Special Text Transfer Mode */
	grayishTextOr                 = 49,
	hilitetransfermode            = 50,
	hilite                        = 50,	/* Arithmetic transfer modes */
	blend                         = 32,
	addPin,
	addOver,
	subPin,
	addMax                        = 37,
	adMax                         = 37,
	subOver,
	adMin,
	ditherCopy                    = 64,	/* Transparent mode constant */
	transparent                   = 36
};

enum {
	italicBit = 1,
	ulineBit,
	outlineBit,
	shadowBit,
	condenseBit,
	extendBit
};

enum {
	normalBit = 0,
	inverseBit,
	blueBit,
	greenBit,
	redBit,
	blackBit,
	yellowBit,
	magentaBit,
	cyanBit
};

enum {
	blackColor		= 33,   /*colors expressed in these mappings*/
	whiteColor		= 30,
	redColor		= 205,
	greenColor		= 341,
	blueColor		= 409,
	cyanColor		= 273,
	magentaColor	= 137,
	yellowColor		= 69
};

enum {
	picLParen	= 0,	/*standard picture comments*/
	picRParen	= 1,
	clutType	= 0,	/*0 if lookup table*/
	fixedType	= 1,	/*1 if fixed table*/
	directType	= 2,	/*2 if direct values*/
	gdDevType	= 0		/*0 = monochrome 1 = color*/
};

enum {
	pmCourteous                   = 0,    /*Record use of color on each device touched.*/
	pmDithered                    = 0x0001,
	pmTolerant                    = 0x0002, /*render ciRGB if ciTolerance is exceeded by best match.*/
	pmAnimated                    = 0x0004, /*reserve an index on each device touched and render ciRGB.*/
	pmExplicit                    = 0x0008, /*no reserve, no render, no record; stuff index into port.*/
	pmWhite                       = 0x0010,
	pmBlack                       = 0x0020,
	pmInhibitG2                   = 0x0100,
	pmInhibitC2                   = 0x0200,
	pmInhibitG4                   = 0x0400,
	pmInhibitC4                   = 0x0800,
	pmInhibitG8                   = 0x1000,
	pmInhibitC8                   = 0x2000, /* NSetPalette Update Constants */
	pmNoUpdates                   = 0x8000, /*no updates*/
	pmBkUpdates                   = 0xA000, /*background updates only*/
	pmFgUpdates                   = 0xC000, /*foreground updates only*/
	pmAllUpdates                  = 0xE000 /*all updates*/
};

enum {
	interlacedDevice	= 2,
	hwMirroredDevice	= 4,
	roundedDevice,
	hasAuxMenuBar,
	burstDevice,
	ext32Device,
	ramInit				= 10,
	mainScreen,
	allInit,
	screenDevice,
	noDriver,
	screenActive,
	hiliteBit			= 7,
	pHiliteBit			= 0,
	defQDColors			= 127,
	RGBDirect			= 16,
	baseAddr32			= 4
};

enum {
	sysPatListID = 0,
	iBeamCursor,
	crossCursor,
	plusCursor,
	watchCursor
};

enum {
	kQDGrafVerbFrame = 0,
	kQDGrafVerbPaint,
	kQDGrafVerbErase,
	kQDGrafVerbInvert,
	kQDGrafVerbFill
};

typedef SInt8 PixelType;
typedef short Bits16[16];

typedef struct Cursor {
	Bits16	data;
	Bits16	mask;
	Point	hotSpot;
} Cursor, *CursPtr, **CursHandle;

typedef struct PenState {
	Point	pnLoc;
	Point	pnSize;
	short	pnMode;
	Pattern	pnPat;
} PenState;

typedef struct GammaTbl {
	short	gVersion;
	short	gType;
	short	gFormulaSize;
	short	gChanCnt;
	short	gDataCnt;
	short	gDataWidth;
	short	gFormulaData[1];
} GammaTbl, *GammaTblPtr, **GammaTblHandle;

enum {
	pixPurgeBit = 0,
	noNewDeviceBit,
	useTempMemBit,
	keepLocalBit,
	useDistantHdwrMemBit,
	useLocalHdwrMemBit,
	pixelsPurgeableBit,
	pixelsLockedBit,
	nativeEndianPixMapBit,
	mapPixBit = 16,
	newDepthBit,
	alignPixBit,
	newRowBytesBit,
	reallocPixBit,
	clipPixBit = 28,
	stretchPixBit,
	ditherPixBit,
	gwFlagErrBit
};

enum {
	returnColorTable		= 0x0001,
	returnPalette			= 0x0002,
	recordComments			= 0x0004,
	recordFontInfo			= 0x0008,
	suppressBlackAndWhite	= 0x0010
};

enum {
	pixPurge				= 1L << pixPurgeBit,
	noNewDevice				= 1L << noNewDeviceBit,
	useTempMem				= 1L << useTempMemBit,
	keepLocal				= 1L << keepLocalBit,
	useDistantHdwrMem		= 1L << useDistantHdwrMemBit,
	useLocalHdwrMem			= 1L << useLocalHdwrMemBit,
	pixelsPurgeable			= 1L << pixelsPurgeableBit,
	pixelsLocked			= 1L << pixelsLockedBit,
	kNativeEndianPixMap		= 1L << nativeEndianPixMapBit,
	kAllocDirectDrawSurface	= 1L << 14,
	mapPix					= 1L << mapPixBit,
	newDepth				= 1L << newDepthBit,
	alignPix				= 1L << alignPixBit,
	newRowBytes				= 1L << newRowBytesBit,
	reallocPix				= 1L << reallocPixBit,
	clipPix					= 1L << clipPixBit,
	stretchPix				= 1L << stretchPixBit,
	ditherPix				= 1L << ditherPixBit,
	gwFlagErr				= 1L << gwFlagErrBit
};

struct CCrsr {
	short               crsrType;               /*type of cursor*/
	PixMapHandle        crsrMap;                /*the cursor's pixmap*/
	Handle              crsrData;               /*cursor's data*/
	Handle              crsrXData;              /*expanded cursor data*/
	short               crsrXValid;             /*depth of expanded data (0 if none)*/
	Handle              crsrXHandle;            /*future use*/
	Bits16              crsr1Data;              /*one-bit cursor*/
	Bits16              crsrMask;               /*cursor's mask*/
	Point               crsrHotSpot;            /*cursor's hotspot*/
	SInt32              crsrXTable;             /*private*/
	SInt32              crsrID;                 /*private*/
};
typedef struct CCrsr                    CCrsr;
typedef CCrsr *                         CCrsrPtr;
typedef CCrsrPtr *                      CCrsrHandle;

struct ColorInfo {
	RGBColor            ciRGB;                  /*true RGB values*/
	short               ciUsage;                /*color usage*/
	short               ciTolerance;            /*tolerance value*/
	short               ciDataFields[3];        /*private fields*/
};
typedef struct ColorInfo                ColorInfo;
typedef ColorInfo *                     ColorInfoPtr;
typedef ColorInfoPtr *                  ColorInfoHandle;
struct Palette {
	short               pmEntries;              /*entries in pmTable*/
	short               pmDataFields[7];        /*private fields*/
	ColorInfo           pmInfo[1];
};
typedef struct Palette                  Palette;
typedef Palette *                       PalettePtr;
typedef PalettePtr *                    PaletteHandle;
typedef long                            PictInfoID;
struct CommentSpec {
	short               count;                  /* number of occurrances of this comment ID */
	short               ID;                     /* ID for the comment in the picture */
};
typedef struct CommentSpec              CommentSpec;
typedef CommentSpec *                   CommentSpecPtr;
typedef CommentSpecPtr *                CommentSpecHandle;
struct FontSpec {
	short               pictFontID;             /* ID of the font in the picture */
	short               sysFontID;              /* ID of the same font in the current system file */
	long                size[4];                /* bit array of all the sizes found (1..127) (bit 0 means > 127) */
	short               style;                  /* combined style of all occurrances of the font */
	long                nameOffset;             /* offset into the fontNamesHdl handle for the font’s name */
};
typedef struct FontSpec                 FontSpec;
typedef FontSpec *                      FontSpecPtr;
typedef FontSpecPtr *                   FontSpecHandle;
struct PictInfo {
	short               version;                /* this is always zero, for now */
	long                uniqueColors;           /* the number of actual colors in the picture(s)/pixmap(s) */
	PaletteHandle       thePalette;             /* handle to the palette information */
	CTabHandle          theColorTable;          /* handle to the color table */
	Fixed               hRes;                   /* maximum horizontal resolution for all the pixmaps */
	Fixed               vRes;                   /* maximum vertical resolution for all the pixmaps */
	short               depth;                  /* maximum depth for all the pixmaps (in the picture) */
	Rect                sourceRect;             /* the picture frame rectangle (this contains the entire picture) */
	long                textCount;              /* total number of text strings in the picture */
	long                lineCount;              /* total number of lines in the picture */
	long                rectCount;              /* total number of rectangles in the picture */
	long                rRectCount;             /* total number of round rectangles in the picture */
	long                ovalCount;              /* total number of ovals in the picture */
	long                arcCount;               /* total number of arcs in the picture */
	long                polyCount;              /* total number of polygons in the picture */
	long                regionCount;            /* total number of regions in the picture */
	long                bitMapCount;            /* total number of bitmaps in the picture */
	long                pixMapCount;            /* total number of pixmaps in the picture */
	long                commentCount;           /* total number of comments in the picture */
	long                uniqueComments;         /* the number of unique comments in the picture */
	CommentSpecHandle   commentHandle;          /* handle to all the comment information */
	long                uniqueFonts;            /* the number of unique fonts in the picture */
	FontSpecHandle      fontHandle;             /* handle to the FontSpec information */
	Handle              fontNamesHandle;        /* handle to the font names */
	long                reserved1;
	long                reserved2;
};
typedef struct PictInfo                 PictInfo;
typedef PictInfo *                      PictInfoPtr;
typedef PictInfoPtr *                   PictInfoHandle;
typedef CALLBACK_API( OSErr , InitPickMethodProcPtr )(SInt16 colorsRequested, UInt32 *dataRef, SInt16 *colorBankType);
typedef CALLBACK_API( OSErr , RecordColorsProcPtr )(UInt32 dataRef, RGBColor *colorsArray, SInt32 colorCount, SInt32 *uniqueColors);
typedef CALLBACK_API( OSErr , CalcColorTableProcPtr )(UInt32 dataRef, SInt16 colorsRequested, void *colorBankPtr, CSpecArray resultPtr);
typedef CALLBACK_API( OSErr , DisposeColorPickMethodProcPtr )(UInt32 dataRef);
typedef STACK_UPP_TYPE(InitPickMethodProcPtr)                   InitPickMethodUPP;
typedef STACK_UPP_TYPE(RecordColorsProcPtr)                     RecordColorsUPP;
typedef STACK_UPP_TYPE(CalcColorTableProcPtr)                   CalcColorTableUPP;
typedef STACK_UPP_TYPE(DisposeColorPickMethodProcPtr)           DisposeColorPickMethodUPP;

typedef SInt8                           GrafVerb;
CF_ENUM(GrafVerb) {
	chunky                        = 0,
	chunkyPlanar                  = 1,
	planar                        = 2
};

typedef short                           TruncCode;
CF_ENUM(TruncCode) {
	/* Constants for truncWhere argument in TruncString and TruncText */
	truncEnd                      = 0,    /* Truncate at end */
	truncMiddle                   = 0x4000, /* Truncate in middle */
	smTruncEnd                    = 0,    /* Truncate at end - obsolete */
	smTruncMiddle                 = 0x4000 /* Truncate in middle - obsolete */
};

struct ITab {
	SInt32              iTabSeed;               /*copy of CTSeed from source CTable*/
	short               iTabRes;                /*bits/channel resolution of iTable*/
	Byte                iTTable[1];             /*byte colortable index values*/
};
typedef struct ITab                     ITab;
typedef ITab *                          ITabPtr;
typedef ITabPtr *                       ITabHandle;

struct FMOutput {
	short               errNum;
	Handle              fontHandle;
	UInt8               boldPixels;
	UInt8               italicPixels;
	UInt8               ulOffset;
	UInt8               ulShadow;
	UInt8               ulThick;
	UInt8               shadowPixels;
	SInt8               extra;
	UInt8               ascent;
	UInt8               descent;
	UInt8               widMax;
	SInt8               leading;
	SInt8               curStyle;
	Point               numer;
	Point               denom;
};
typedef struct FMOutput                 FMOutput;
typedef FMOutput *                      FMOutputPtr;
typedef FMOutputPtr                     FMOutPtr;

#pragma mark Carbon printing
typedef struct OpaquePMPrintContext* PMPrintContext;

#pragma mark end of carbon printing

#if TARGET_OS_MAC
#define MacInvertRect InvertRect
#define MacOffsetRect OffsetRect
#define MacSetRect SetRect
#define MacPtInRect PtInRect
#define MacLineTo LineTo
#define MacFillRect FillRect
#define MacSetPort SetPort
#define MacGetCursor GetCursor
#define MacSetCursor SetCursor
#define MacFrameRect FrameRect
#define MacUnionRgn UnionRgn
#define MacShowCursor ShowCursor
#define MacCopyRgn CopyRgn
#define MacInsetRect InsetRect
#define MacEqualRect EqualRect
#define MacOffsetRgn OffsetRgn
#define MacInvertRgn InvertRgn
#define MacXorRgn XorRgn
#define MacEqualRgn EqualRgn
#define MacPaintRgn PaintRgn
#define MacUnionRect UnionRect
#define MacSetRectRgn SetRectRgn
#define MacFrameRgn FrameRgn
#define MacDrawText DrawText
#endif

__BEGIN_DECLS
extern void GetPort(GrafPtr *);
extern short GetPortTextFont(CGrafPtr);
extern short GetPortTextSize(CGrafPtr);
extern void PaintRect(const Rect *);
extern void ForeColor(long);
extern void MacInvertRect(const Rect *);
extern void MacSetRect(Rect *, short, short, short, short);
extern GDHandle GetDeviceList(void);
extern void MacOffsetRect(Rect * r, short, short);
extern GDHandle GetNextDevice(GDHandle);
extern BitMap *GetQDGlobalsScreenBits(BitMap *);
extern void LocalToGlobal(Point *);
extern Rect *GetPortBounds(CGrafPtr, Rect *);
extern void pStrcpy(register unsigned char *, register const unsigned char *);
extern Boolean MacPtInRect(Point, const Rect *);
extern GDHandle GetMainDevice();
extern RgnHandle NewRgn();
extern void OpenRgn();
extern void CloseRgn(RgnHandle);
extern PixMapHandle GetPortPixMap(CGrafPtr);
extern GDHandle GetGDevice();
extern void MoveTo(short, short);
extern void MacLineTo(short, short);
extern void EraseRect(const Rect *);
extern void MacFillRect(const Rect *,const Pattern *);
extern short TextWidth(const void *, short, short);
extern void DrawString(ConstStr255Param);
extern long DeltaPoint(Point, Point);
extern void MacSetPort(GrafPtr);
extern CursHandle MacGetCursor(short);
extern void MacSetCursor(const Cursor *);
extern Boolean PtInRgn(Point, RgnHandle);
extern void RGBForeColor(const RGBColor *);
extern void BackColor(long);
extern void CopyBits(const BitMap *, const BitMap *, const Rect *, const Rect *, short, RgnHandle);
extern void RGBBackColor(const RGBColor *);
extern RgnHandle GetPortVisibleRegion(CGrafPtr, RgnHandle);
extern void DisposeRgn(RgnHandle);
extern void GlobalToLocal(Point *);
extern void LMSetHiliteMode(UInt8);
extern UInt8 LMGetHiliteMode();
extern void GetClip(RgnHandle);
extern void SetClip(RgnHandle);
extern void ClipRect(const Rect *);
extern Cursor *GetQDGlobalsArrow(Cursor *);
extern PicHandle GetPicture(short);
extern void HideCursor(void);
extern void InitCursor(void);
extern void MacFrameRect(const Rect *);
extern void RectRgn(RgnHandle, const Rect *);
extern void DiffRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacUnionRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacShowCursor();
extern void DrawPicture(PicHandle, const Rect *);
extern Boolean SectRect(const Rect *, const Rect *, Rect *);
extern void PenSize(short, short);
extern void FrameArc(const Rect *, short, short);
extern void HiliteColor(const RGBColor *);
extern void LMGetHiliteRGB(RGBColor *);
extern Boolean QDIsPortBuffered(CGrafPtr);
extern void QDFlushPortBuffer(CGrafPtr, RgnHandle);
extern void ScrollRect(const Rect *, short, short, RgnHandle);
extern void MacCopyRgn(RgnHandle, RgnHandle);
extern void SetPt(Point *, short, short);
extern Rect *GetRegionBounds(RgnHandle, Rect *);
extern void InsetRgn(RgnHandle, short, short);
extern void DiffRgn(RgnHandle, RgnHandle, RgnHandle);
extern void MacInsetRect(Rect *, short, short);
extern void SetCPixel(short, short, const RGBColor *);
extern Boolean MacEqualRect(const Rect *, const Rect *);
extern void MacOffsetRgn(RgnHandle, short, short);
extern void EraseRgn(RgnHandle);
extern CTabHandle GetCTable(short);
extern void GetBackColor(RGBColor *);
extern long Color2Index(const RGBColor *);
extern void FrameRoundRect(const Rect *, short, short);
extern short Random();
extern void PenMode(short);
extern Boolean IsRegionRectangular(RgnHandle);
extern void GetPenState(PenState *);
extern void SetPenState(const PenState *);
extern void PenPixPat(PixPatHandle);
extern PixPatHandle GetPixPat(short);
extern Boolean EmptyRect(const Rect *);
extern PixMapHandle NewPixMap();
extern void DisposePixMap(PixMapHandle);
extern Boolean RectInRgn(const Rect *, RgnHandle);
extern Boolean EmptyRgn(RgnHandle);
extern void Pt2Rect(Point, Point, Rect*);
extern Rect *GetPixBounds(PixMapHandle, Rect*);
extern void SetOrigin(short, short);
extern void MacInvertRgn(RgnHandle);
extern void PenNormal();
extern const BitMap *GetPortBitMapForCopyBits(CGrafPtr);
extern void GetForeColor(RGBColor *);
extern void Index2Color(long, RGBColor *);
extern void CopyMask(const BitMap *, const BitMap *, const BitMap *, const Rect *, const Rect *, const Rect *);
extern void ObscureCursor();
extern CGrafPtr GetQDGlobalsThePort();
extern OSErr LockPortBits(GrafPtr);
extern OSErr UnlockPortBits(GrafPtr);
extern OSStatus QDAddRectToDirtyRegion(CGrafPtr, const Rect *);
extern Boolean QDSwapPort(CGrafPtr, CGrafPtr *);
extern void DisposePixPat(PixPatHandle);
extern PixPatHandle NewPixPat(void);
extern void MakeRGBPat(PixPatHandle, const RGBColor *);

#pragma mark Text funcs
extern void TextFace(StyleParameter face);
extern void TextMode(short);
extern void TextSize(short);
extern void TextFont(short);
extern short StringWidth(ConstStr255Param);
extern void TextFace(StyleParameter);
extern void GetFontInfo(FontInfo *);
extern void DrawChar(CharParameter);
extern short CharWidth(CharParameter);
extern void MeasureText(
			short         count,
			const void *  textAddr,
			void *        charLocs);
extern void MacDrawText(
			const void *  textBuf,
			short         firstByte,
			short         byteCount);
extern QDTextUPP NewQDTextUPP(QDTextProcPtr userRoutine);
extern QDTxMeasUPP NewQDTxMeasUPP(QDTxMeasProcPtr userRoutine);
extern short StdTxMeas(
					   short         byteCount,
					   const void *  textAddr,
					   Point *       numer,
					   Point *       denom,
					   FontInfo *    info);
extern void StdText(
					short         count,
					const void *  textAddr,
					Point         numer,
					Point         denom);

//Offscreen
extern PixMapHandle GetGWorldPixMap(GWorldPtr);
extern void GetGWorld(CGrafPtr *, GDHandle *);
extern QDErr NewGWorld(GWorldPtr *, short, const Rect *, CTabHandle, GDHandle, GWorldFlags);
extern Boolean LockPixels(PixMapHandle pm);
extern void SetGWorld(CGrafPtr, GDHandle);
extern void UnlockPixels(PixMapHandle);
extern void DisposeGWorld(GWorldPtr);
extern void SetGDevice(GDHandle);
extern long GetPixRowBytes(PixMapHandle);
extern GDHandle GetGWorldDevice(GWorldPtr);
extern void GDeviceChanged(GDHandle);

//Seed
extern long GetQDGlobalsRandomSeed(void);
extern void SetQDGlobalsRandomSeed(long);

//Palettes
extern void PmForeColor(short);
extern void PmBackColor(short);

#pragma mark - Font functions
extern short GetDefFontSize(void);
extern void GetFontName(short, Str255);
extern void SetOutlinePreferred(Boolean);
extern FMFontFamily FMGetFontFamilyFromName(ConstStr255Param);
extern void SetFractEnable(Boolean fractEnable);
extern short GetAppFont(void);

#pragma mark ATS
extern OSStatus ATSUFindFontFromName(
					 Ptr                iName,
					 ByteCount          iNameLength,
					 FontNameCode       iFontNameCode,
					 FontPlatformCode   iFontNamePlatform,
					 FontScriptCode     iFontNameScript,
					 FontLanguageCode   iFontNameLanguage,
					 ATSUFontID *       oFontID);
extern OSStatus ATSUMeasureText(
				ATSUTextLayout         iTextLayout,
				UniCharArrayOffset     iLineStart,
				UniCharCount           iLineLength,
				ATSUTextMeasurement *  oTextBefore,
				ATSUTextMeasurement *  oTextAfter,
				ATSUTextMeasurement *  oAscent,
				ATSUTextMeasurement *  oDescent);
extern OSStatus ATSUCreateStyle(ATSUStyle * oStyle);
extern OSStatus ATSUCreateAndCopyStyle(
					   ATSUStyle    iStyle,
					   ATSUStyle *  oStyle);
extern OSStatus ATSUDisposeStyle(ATSUStyle iStyle);
extern OSStatus ATSUSetAttributes(
				  ATSUStyle                     iStyle,
				  ItemCount                     iAttributeCount,
				  const ATSUAttributeTag        iTag[],
				  const ByteCount               iValueSize[],
				  const ATSUAttributeValuePtr   iValue[]);
extern OSStatus ATSUClearAttributes(
					ATSUStyle                iStyle,
					ItemCount                iTagCount,
					const ATSUAttributeTag   iTag[]);
extern OSStatus ATSUClearStyle(ATSUStyle);
extern OSStatus ATSUStyleIsEmpty(
				 ATSUStyle   iStyle,
				 Boolean *   oIsClear);
extern OSStatus ATSUFONDtoFontID(
				 short         iFONDNumber,
				 Style         iFONDStyle,
				 ATSUFontID *  oFontID);
extern OSStatus ATSUCreateTextLayout(ATSUTextLayout *);
extern OSStatus ATSUCreateAndCopyTextLayout(
							ATSUTextLayout    iTextLayout,
							ATSUTextLayout *  oTextLayout);
extern OSStatus ATSUCreateTextLayoutWithTextPtr(
								ConstUniCharArrayPtr   iText,
								UniCharArrayOffset     iTextOffset,
								UniCharCount           iTextLength,
								UniCharCount           iTextTotalLength,
								ItemCount              iNumberOfRuns,
								const UniCharCount     iRunLengths[],
								ATSUStyle              iStyles[],
								ATSUTextLayout *       oTextLayout);
extern OSStatus ATSUDisposeTextLayout(ATSUTextLayout iTextLayout);
extern OSStatus ATSUSetTextPointerLocation(
						   ATSUTextLayout         iTextLayout,
						   ConstUniCharArrayPtr   iText,
						   UniCharArrayOffset     iTextOffset,
						   UniCharCount           iTextLength,
						   UniCharCount           iTextTotalLength);
extern OSStatus ATSUSetLayoutControls(
					  ATSUTextLayout                iTextLayout,
					  ItemCount                     iAttributeCount,
					  const ATSUAttributeTag        iTag[],
					  const ByteCount               iValueSize[],
					  const ATSUAttributeValuePtr   iValue[]);
extern OSStatus ATSUClearLayoutControls(
						ATSUTextLayout           iTextLayout,
						ItemCount                iTagCount,
						const ATSUAttributeTag   iTag[]);
extern OSStatus ATSUSetRunStyle(
				ATSUTextLayout       iTextLayout,
				ATSUStyle            iStyle,
				UniCharArrayOffset   iRunStart,
				UniCharCount         iRunLength);
extern OSStatus ATSUBreakLine(
			  ATSUTextLayout        iTextLayout,
			  UniCharArrayOffset    iLineStart,
			  ATSUTextMeasurement   iLineWidth,
			  Boolean               iUseAsSoftLineBreak,
			  UniCharArrayOffset *  oLineBreak);
extern OSStatus
ATSUGetSoftLineBreaks(
					  ATSUTextLayout       iTextLayout,
					  UniCharArrayOffset   iRangeStart,
					  UniCharCount         iRangeLength,
					  ItemCount            iMaximumBreaks,
					  UniCharArrayOffset   oBreaks[],
					  ItemCount *          oBreakCount);
extern OSStatus ATSUDrawText(
			 ATSUTextLayout        iTextLayout,
			 UniCharArrayOffset    iLineOffset,
			 UniCharCount          iLineLength,
			 ATSUTextMeasurement   iLocationX,
			 ATSUTextMeasurement   iLocationY);
extern OSStatus ATSUGetLayoutControl(
					 ATSUTextLayout          iTextLayout,
					 ATSUAttributeTag        iTag,
					 ByteCount               iExpectedValueSize,
					 ATSUAttributeValuePtr   oValue,
					 ByteCount *             oActualValueSize);

#pragma mark - Pen functions
extern void PenPat(const Pattern *);
extern void ShowPen(void);
extern void HidePen(void);

#pragma mark - Printing
extern OSStatus PMUnflattenPageFormat(
					  Handle          flatFormat,
					  PMPageFormat *  pageFormat);
extern OSStatus PMUnflattenPrintSettings(
						 Handle             flatSettings,
						 PMPrintSettings *  printSettings);
extern OSStatus PMConvertOldPrintRecord(
						Handle             printRecordHandle,
						PMPrintSettings *  printSettings,
						PMPageFormat *     pageFormat);
extern OSStatus PMNewPageFormat(PMPageFormat *);
extern OSStatus PMDefaultPageFormat(PMPageFormat);
extern OSStatus PMNewPrintSettings(PMPrintSettings * printSettings);
extern OSStatus PMDefaultPrintSettings(PMPrintSettings printSettings);
extern OSStatus PMMakeOldPrintRecord(
					 PMPrintSettings   printSettings,
					 PMPageFormat      pageFormat,
					 Handle *          printRecordHandle);
extern OSStatus PMFlattenPageFormat(
					PMPageFormat   pageFormat,
					Handle *       flatFormat);
extern OSStatus PMDisposePageFormat(PMPageFormat);
extern OSStatus PMFlattenPrintSettings(
					   PMPrintSettings   printSettings,
					   Handle *          flatSettings);
extern OSStatus PMDisposePrintSettings(PMPrintSettings);
extern OSStatus PMError(void);
extern OSStatus PMGetGrafPtr(
			 PMPrintContext   printContext,
			 GrafPtr *        grafPort);
extern OSStatus PMBeginDocument(
				PMPrintSettings   printSettings,
				PMPageFormat      pageFormat,
				PMPrintContext *  printContext);
extern OSStatus PMEndDocument(PMPrintContext printContext);
extern OSStatus PMBeginPage(
			PMPrintContext   printContext,
			const PMRect *   pageFrame);
extern OSStatus PMEndPage(PMPrintContext);
extern OSStatus PMBegin(void);
extern OSStatus PMEnd(void);
extern OSStatus PMPageSetupDialog(
				  PMPageFormat   pageFormat,
				  Boolean *      accepted);
extern OSStatus PMPrintDialog(
			  PMPrintSettings   printSettings,
			  PMPageFormat      constPageFormat,
			  Boolean *         accepted);

#pragma mark -

#pragma mark CGContext
extern OSStatus SyncCGContextOriginWithPort(CGContextRef, CGrafPtr);
extern OSStatus CreateCGContextForPort(CGrafPtr, CGContextRef *);

//misc.
extern GDHandle DMGetFirstScreenDevice(Boolean);
extern GDHandle DMGetNextScreenDevice(GDHandle, Boolean);
extern OSErr DMGetDisplayIDByGDevice(GDHandle, DisplayIDType *, Boolean);
extern OSErr SetDepth(GDHandle, short, short, short);
extern Pattern *GetQDGlobalsGray(Pattern *);
extern void SetEmptyRgn(RgnHandle rgn);
extern void MacXorRgn(RgnHandle, RgnHandle, RgnHandle);
extern void SectRgn(RgnHandle, RgnHandle, RgnHandle);
extern Boolean MacEqualRgn(RgnHandle, RgnHandle);
extern void SetQDGlobalsArrow(const Cursor *);
extern Pattern *GetQDGlobalsDarkGray(Pattern*);
extern Pattern *GetQDGlobalsLightGray(Pattern*);
extern Pattern *GetQDGlobalsBlack(Pattern *);
extern Pattern *GetQDGlobalsWhite(Pattern *);
extern void MacPaintRgn(RgnHandle);
extern void MacUnionRect(const Rect *, const Rect *, Rect *);
extern void BackPat(const Pattern *);
extern PicHandle OpenPicture(const Rect *);
extern void ClosePicture(void);
extern void Line(short, short);
extern void PaintOval(const Rect *);
extern void FrameOval(const Rect *);
extern void GetFNum(ConstStr255Param, short*);
extern OSErr GetPixMapInfo(
			  PixMapHandle,
			  PictInfo *,
			  short,
			  short,
			  short,
			  short);
extern void PaintRoundRect(
			   const Rect *,
			   short,
			   short);
extern void MacSetRectRgn(RgnHandle, short, short, short, short);
extern void FillCRect(const Rect*, PixPatHandle);
extern Boolean EqualPt(Point, Point);
extern void DisposeCTable(CTabHandle);
extern short QDError(void);
extern void LMSetLastSPExtra(SInt32);
extern void CalcCMask(
		  const BitMap *    srcBits,
		  const BitMap *    dstBits,
		  const Rect *      srcRect,
		  const Rect *      dstRect,
		  const RGBColor *  seedRGB,
		  ColorSearchUPP    matchProc,
		  long              matchData);
extern OSErr BitMapToRegion(RgnHandle, const BitMap*);
extern void DisposePort(CGrafPtr port);
extern PaletteHandle NewPalette(
		   short        entries,
		   CTabHandle   srcColors,
		   short        srcUsage,
		   short        srcTolerance);
extern void DisposePalette(PaletteHandle srcPalette);
extern void NSetPalette(
			WindowRef       dstWindow,
			PaletteHandle   srcPalette,
			short           nCUpdates);
extern Ptr GetPixBaseAddr(PixMapHandle);
extern void KillPicture(PicHandle);
extern CGrafPtr CreateNewPort(void);
extern long GetCTSeed(void);
extern GDHandle GetMaxDevice(const Rect *);
extern void FramePoly(PolyHandle);
extern void MapRgn(RgnHandle, const Rect*, const Rect*);
extern void SetCCursor(CCrsrHandle cCrsr);
extern CCrsrHandle GetCCursor(short crsrID);
extern void DisposeCCursor(CCrsrHandle cCrsr);
extern void MacFrameRgn(RgnHandle rgn);
extern short TruncString(
			short       width,
			Str255      theString,
			TruncCode   truncWhere);
extern void SetPortBounds(CGrafPtr, const Rect*);
extern void GetCPixel(short, short, RGBColor*);
extern void EraseRoundRect(
			   const Rect *  r,
			   short         ovalWidth,
			   short         ovalHeight);
extern void PicComment(short, short, Handle);
extern StyledLineBreakCode
StyledLineBreak(
				Ptr      textPtr,
				long     textLen,
				long     textStart,
				long     textEnd,
				long     flags,
				Fixed *  textWidth,
				long *   textOffset);
extern long VisibleLength(Ptr, long);
extern Style GetPortTextFace(CGrafPtr);
extern void EraseRoundRect(const Rect*, short, short);
extern UInt8 LMGetFractEnable(void);
extern void LMSetFractEnable(UInt8 value);
extern void SetStdCProcs(CQDProcs * procs);
extern Boolean GetOutlinePreferred(void);
extern void DisposeDragGrayRgnUPP(DragGrayRgnUPP);
extern DragGrayRgnUPP NewDragGrayRgnUPP(DragGrayRgnProcPtr);
extern void OpColor(const RGBColor*);
extern Rect *QDGetPictureBounds(PicHandle, Rect*);
extern GWorldFlags UpdateGWorld(
			 GWorldPtr *   offscreenGWorld,
			 short         pixelDepth,
			 const Rect *  boundsRect,
			 CTabHandle    cTable,
			 GDHandle      aGDevice,
			 GWorldFlags   flags);
extern Boolean IsPortRegionBeingDefined(CGrafPtr);
extern void SetPortVisibleRegion(CGrafPtr, RgnHandle);
extern short GetPortTextMode(CGrafPtr);
extern void SetPortGrafProcs(CGrafPtr, CQDProcsPtr);
extern short GetPixDepth(PixMapHandle);
extern CQDProcsPtr GetPortGrafProcs(CGrafPtr);
extern Boolean IsPortColor(CGrafPtr);
extern Boolean TestDeviceAttribute(GDHandle, short);
extern void MovePortTo(short, short);
extern void PortSize(short, short);
extern short GetSysFont(void);
extern FMOutPtr FMSwapFont(const FMInput * inRec);

__END_DECLS

#pragma pack (pop)

#endif

#endif
