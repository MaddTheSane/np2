#include	"compiler.h"

#include	"dosio.h"
#include	"fdefine.h"
#include	"strres.h"

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

static int GetFileExt(char* filename) {

    char*	p;
    int		ftype;
    
    p = file_getext((char *)filename);
    if ((!milstr_cmp(p, str_d88)) || (!milstr_cmp(p, str_d98))) {
			ftype = FTYPE_D88;
		}
    else if ((!milstr_cmp(p, str_ini))) {
			ftype = FTYPE_TEXT;
		}
    else if ((!milstr_cmp(p, str_bmp))) {
			ftype = FTYPE_BMP;
		}
    else if ((!milstr_cmp(p, str_thd))) {
			ftype = FTYPE_THD;
		}
    else if ((!milstr_cmp(p, str_hdi))) {
			ftype = FTYPE_HDI;
		}
    else {
        ftype = FTYPE_BETA;
    }
    return(ftype);
}

int file_getftype(char* filename) {

	FSSpec	fss;
	Str255	fname;
	FInfo	fndrInfo;
    int		ftype;

	mkstr255(fname, filename);
	FSMakeFSSpec(0, 0, fname, &fss);
	if (FSpGetFInfo(&fss, &fndrInfo) != noErr) {
		return(FTYPE_NONE);
	}
    ftype = Getfiletype(&fndrInfo);
	if (ftype == FTYPE_NONE) {
        ftype = GetFileExt(filename);
	}
	return(ftype);
}
