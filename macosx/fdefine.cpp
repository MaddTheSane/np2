#include	"compiler.h"

#include	"dosio.h"
#include	"fdefine.h"
#include	"strres.h"

void Setfiletype(int ftype, OSType *creator, OSType *fileType) {

	*creator = 'SMil';
	*fileType = '????';

	switch(ftype) {
//		case FTYPE_SMIL:			// �V�X�e���\��

		case FTYPE_TEXT:			// �e�L�X�g�t�@�C��

			*creator = 'ttxt';
			*fileType = 'TEXT';
			break;

//		case FTYPE_BMP:				// Bitmap
//		case FTYPE_PICT:			// Picture (�\��)
//		case FTYPE_PNG:				// Png (�\��)
//		case FTYPE_WAV:				// Wave

		case FTYPE_D88:				// D88
			*fileType = '.D88';
			break;

		case FTYPE_BETA:			// �x�^�C���[�W
			*fileType = 'BETA';
			break;

		case FTYPE_THD:				// .thd �n�[�h�f�B�X�N�C���[�W
			*fileType = '.THD';
			break;

		case FTYPE_HDI:				// .hdi �n�[�h�f�B�X�N�C���[�W
			*fileType = '.HDI';
			break;

		case FTYPE_HDD:				// .hdd �n�[�h�f�B�X�N�C���[�W (�\��)
			*fileType = '.HDD';
			break;

		case FTYPE_S98:				// .s98 �n�[�h�f�B�X�N�C���[�W
			*fileType = '.S98';
			break;

		case FTYPE_MIMPI:			// mimpi default�t�@�C��
			*fileType = '.DEF';
			break;
#if 0
        case FTYPE_AIFF:
            *fileType = 'AIFF';
            *creator = 'hook';
            break;
#endif
        case FTYPE_INI:
            *fileType = 'TEXT';
            *creator = 'SMil';
            break;
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
			ftype = FTYPE_INI;
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
    else if ((!milstr_cmp(p, "rom")) || (!milstr_cmp(p, "cfg")) || (!milstr_cmp(p, "sav"))) {
        ftype = FTYPE_NONE;
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
