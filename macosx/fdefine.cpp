#include	"compiler.h"

#include	"dosio.h"
#include	"fdefine.h"

void Setfiletype(int ftype, OSType *creator, OSType *fileType) {

	*creator = 'SMil';
	*fileType = '????';

	switch(ftype) {
//		case FTYPE_SMIL:			// システム予約

		case FTYPE_TEXT:			// テキストファイル

			*creator = 'ttxt';
			*fileType = 'TEXT';
			break;

//		case FTYPE_BMP:				// Bitmap
//		case FTYPE_PICT:			// Picture (予約)
//		case FTYPE_PNG:				// Png (予約)
//		case FTYPE_WAV:				// Wave

		case FTYPE_D88:				// D88
			*fileType = '.D88';
			break;

		case FTYPE_BETA:			// ベタイメージ
			*fileType = 'BETA';
			break;

		case FTYPE_THD:				// .thd ハードディスクイメージ
			*fileType = '.THD';
			break;

		case FTYPE_HDI:				// .hdi ハードディスクイメージ
			*fileType = '.HDI';
			break;

		case FTYPE_HDD:				// .hdd ハードディスクイメージ (予約)
			*fileType = '.HDD';
			break;

		case FTYPE_S98:				// .s98 ハードディスクイメージ
			*fileType = '.S98';
			break;

		case FTYPE_MIMPI:			// mimpi defaultファイル
			*fileType = '.DEF';
			break;
#if 0
        case FTYPE_AIFF:
            *fileType = 'AIFF';
            *creator = 'hook';
            break;
#endif
	}
}

static int Getfiletype(FInfo *fndrinfo) {


	switch(fndrinfo->fdType) {
		case '.D88':
			return(FTYPE_D88);

		case 'BETA':
		case '.XDF':
		case '.DUP':
			return(FTYPE_BETA);

		case '.THD':
			return(FTYPE_THD);

		case '.HDI':
			return(FTYPE_HDI);
	}
	return(FTYPE_NONE);
}

int file_getftype(char* filename) {

	FSSpec	fss;
	Str255	fname;
	FInfo	fndrInfo;

	mkstr255(fname, filename);
	FSMakeFSSpec(0, 0, fname, &fss);
	if (FSpGetFInfo(&fss, &fndrInfo) != noErr) {
		return(FTYPE_NONE);
	}
	return(Getfiletype(&fndrInfo));
}
