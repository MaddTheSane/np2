//	BKDSK�p�c�Ȃ񂾂��ǎ���HDB��p

#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"

#ifdef SUPPORT_KAI_IMAGES

#include	"DiskImage/fddfile.h"
#include	"DiskImage/FD/fdd_xdf.h"
#include	"DiskImage/FD/fdd_dcp.h"
#include	"DiskImage/FD/fdd_bkdsk.h"

static const _XDFINFO supportbkdsk[] = {
			{0, 154, 26, 1, DISKTYPE_2HD, 0},	//	BKDSK(HDB)	BASIC 2HD
};

BRESULT fdd_set_bkdsk(FDDFILE fdd, FDDFUNC fdd_fn, const OEMCHAR *fname, int ro) {

const _XDFINFO	*xdf;
	short		attr;
	FILEH		fh;
	UINT32		fdsize;
	UINT		size;
	UINT		tracksize;
	UINT32		trackptr;
	UINT		i;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fh = file_open(fname);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
	fdsize = file_getsize(fh);
	file_close(fh);

	//	�C���[�W�t�@�C���T�C�Y�`�F�b�N
//			{0, 154, 26, 1, DISKTYPE_2HD, 0},	//	BKDSK(HDB)	BASIC 2HD
	xdf = supportbkdsk;
	size = xdf->tracks;
	size *= xdf->sectors;
	size -= (xdf->sectors / 2);
	size <<= (7 + xdf->n);

	size += xdf->headersize;
	if (size != fdsize) {
		return(FAILURE);
	}

	fdd->type = DISKTYPE_BETA;
	fdd->protect = ((attr & 0x01) || (ro)) ? TRUE : FALSE;
	fdd->inf.xdf = *xdf;

	//	�f�B�X�N�A�N�Z�X���p�Ɋe�g���b�N�̃I�t�Z�b�g���Z�o
	tracksize = fdd->inf.xdf.sectors * (128 << fdd->inf.xdf.n);
	//	track 0�p
	fdd->inf.bkdsk.ptr[0] = 0;
	trackptr = tracksize / 2;
	//
	for(i = 1; i < fdd->inf.xdf.tracks; i++) {
		fdd->inf.bkdsk.ptr[i] = trackptr;
		trackptr += tracksize;
	}

	//	�����֐��Q��o�^
	//	��read�Awrite��dcp�n�A����ȊO�͍\���̂̏��׍H��xdf�n�Ƌ��p
	fdd_fn->eject		= fdd_eject_xxx;
	fdd_fn->diskaccess	= fdd_diskaccess_common;
	fdd_fn->seek		= fdd_seek_common;
	fdd_fn->seeksector	= fdd_seeksector_common;
	fdd_fn->read		= fdd_read_dcp;
	fdd_fn->write		= fdd_write_dcp;
	fdd_fn->readid		= fdd_readid_common;
	fdd_fn->writeid		= fdd_dummy_xxx;
	fdd_fn->formatinit	= fdd_dummy_xxx;
	fdd_fn->formating	= fdd_formating_xxx;
	fdd_fn->isformating	= fdd_isformating_xxx;

	return(SUCCESS);
}

#endif