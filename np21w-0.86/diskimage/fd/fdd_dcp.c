#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"

#ifdef SUPPORT_KAI_IMAGES

#include	"DiskImage/fddfile.h"
#include	"DiskImage/FD/fdd_xdf.h"
#include	"DiskImage/FD/fdd_dcp.h"

typedef struct {
	UINT8		mediatype;
	_XDFINFO	xdf;
} __DCPINFO;

//	�S�g���b�N�i�[�C���[�W�`�F�b�N�p
static const __DCPINFO supportdcp[] = {
	{0x01, {162, 154,  8, 3, DISKTYPE_2HD, 0}},	//	01h	2HD- 8�Z�N�^(1.25MB)
	{0x02, {162, 160, 15, 2, DISKTYPE_2HD, 0}},	//	02h	2HD-15�Z�N�^(1.21MB)
	{0x03, {162, 160, 18, 2, DISKTYPE_2HD, 1}},	//	03h	2HQ-18�Z�N�^(1.44MB)
	{0x04, {162, 160,  8, 2, DISKTYPE_2DD, 0}},	//	04h	2DD- 8�Z�N�^( 640KB)
	{0x05, {162, 160,  9, 2, DISKTYPE_2DD, 0}},	//	05h	2DD- 9�Z�N�^( 720KB)
	{0x08, {162, 154,  9, 3, DISKTYPE_2HD, 0}},	//	08h	2HD- 9�Z�N�^(1.44MB)
	{0x11, {162, 154, 26, 1, DISKTYPE_2HD, 0}},	//	11h	BASIC-2HD
	{0x19, {162, 160, 16, 1, DISKTYPE_2DD, 0}},	//	19h	BASIC-2DD
	{0x21, {162, 154, 26, 1, DISKTYPE_2HD, 0}},	//	21h	2HD-26�Z�N�^
};

BRESULT fdd_set_dcp(FDDFILE fdd, FDDFUNC fdd_fn, const OEMCHAR *fname, int ro) {

const __DCPINFO	*dcp;
	short		attr;
	FILEH		fh;
	UINT32		fdsize;
	UINT		size;
	UINT		rsize;
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
	rsize = file_read(fh, &fdd->inf.dcp.head, DCP_HEADERSIZE);	//	DCP�w�b�_�Ǎ�
	file_close(fh);
	if (rsize != DCP_HEADERSIZE) {
		return(FAILURE);
	}

	//	�S�g���b�N�i�[�C���[�W�`�F�b�N
	dcp = supportdcp;
	while(dcp < (supportdcp + NELEMENTS(supportdcp))) {
		if (fdd->inf.dcp.head.mediatype == dcp->mediatype) {
			if (fdd->inf.dcp.head.alltrackflg == 0x01) {
				//	�S�g���b�N�i�[�t���O��0x01�̏ꍇ�A�t�@�C���T�C�Y�`�F�b�N
				size = dcp->xdf.tracks;
				size *= dcp->xdf.sectors;
				size <<= (7 + dcp->xdf.n);
				size += dcp->xdf.headersize;
				if (size != fdsize) {
					return(FAILURE);
				}
			}
			fdd->type = DISKTYPE_DCP;
			fdd->protect = ((attr & 0x01) || (ro)) ? TRUE : FALSE;
			fdd->inf.xdf = dcp->xdf;

			//	�f�B�X�N�A�N�Z�X���p�Ɋe�g���b�N�̃I�t�Z�b�g���Z�o
			tracksize = fdd->inf.xdf.sectors * (128 << fdd->inf.xdf.n);
//			trackptr = 0;
			trackptr = DCP_HEADERSIZE;
			for(i = 0; i < fdd->inf.xdf.tracks; i++) {
				if (fdd->inf.dcp.head.trackmap[i] == 0x01 || fdd->inf.dcp.head.alltrackflg == 0x01) {
					//	�g���b�N�f�[�^�����݂���(trackmap[i] = 0x01)
					//	or �S�g���b�N�i�[�t���O��0x01
					fdd->inf.dcp.ptr[i] = trackptr;
					if (i == 0 && fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS) {
						trackptr += tracksize / 2;	//	BASIC-2HD�Atrack 0�p���׍H
					}
					else {
						trackptr += tracksize;
					}
				}
				else {
					//	�C���[�W�t�@�C����ɑ��݂��Ȃ��g���b�N
					fdd->inf.dcp.ptr[i] = 0;
				}
			}

			//	�����֐��Q��o�^
			//	��read�Awrite�ȊO�͍\���̂̏��׍H��xdf�n�Ƌ��p
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
		dcp++;
	}

	return(FAILURE);
}

BRESULT fdd_read_dcp(FDDFILE fdd) {

	FILEH	hdl;
	UINT	track;
	UINT	secsize;
	long	seekp;

	fddlasterror = 0x00;
	if (fdd_seeksector_common(fdd)) {
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	track = (fdc.treg[fdc.us] << 1) + fdc.hd;
	secsize = 128 << fdd->inf.xdf.n;
	if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && track == 0) {
		//	BASIC-2HD�Atrack 0�p���׍H
		secsize /= 2;
	}
	if ((fdd->type == DISKTYPE_BETA) ||
		(fdd->type == DISKTYPE_DCP && fdd->inf.dcp.head.trackmap[track] == 0x01) ||
		(fdd->type == DISKTYPE_DCP && fdd->inf.dcp.head.alltrackflg == 0x01)) {

		seekp = fdc.R - 1;
		seekp <<= (7 + fdd->inf.xdf.n);
		if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && track == 0) {
			//	BASIC-2HD�Atrack 0�p���׍H
			seekp /= 2;
		}
		seekp += fdd->inf.dcp.ptr[track];
//		seekp += fdd->inf.xdf.headersize;

		hdl = file_open_rb(fdd->fname);
		if (hdl == FILEH_INVALID) {
			fddlasterror = 0xe0;
			return(FAILURE);
		}
		if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
			(file_read(hdl, fdc.buf, secsize) != secsize)) {
			file_close(hdl);
			fddlasterror = 0xe0;
			return(FAILURE);
		}
		file_close(hdl);
	}
	else {
		//	�t�@�C����Ƀf�[�^�̑��݂��Ȃ��Z�N�^��0xE5�Ŗ��߂ĕԂ�
		//	��DCU���ƈႤ�H
		FillMemory(fdc.buf, secsize, 0xe5);
	}
#if 0
	if (fdd->inf.dcp.head.trackmap[track] != 0x01) {
//	if (fdd->inf.dcp.ptr[track] == 0) {
		//	�t�@�C����Ƀf�[�^�̑��݂��Ȃ��Z�N�^��0xE5�Ŗ��߂ĕԂ�
		//	��DCU���ƈႤ�H
		FillMemory(fdc.buf, secsize, 0xe5);
	}
	else {
		seekp = fdc.R - 1;
		seekp <<= (7 + fdd->inf.xdf.n);
		if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && track == 0) {
			//	BASIC-2HD�Atrack 0�p���׍H
			seekp /= 2;
		}
		seekp += fdd->inf.dcp.ptr[track];
		seekp += fdd->inf.xdf.headersize;

		hdl = file_open_rb(fdd->fname);
		if (hdl == FILEH_INVALID) {
			fddlasterror = 0xe0;
			return(FAILURE);
		}
		if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
			(file_read(hdl, fdc.buf, secsize) != secsize)) {
			file_close(hdl);
			fddlasterror = 0xe0;
			return(FAILURE);
		}
		file_close(hdl);
	}
#endif
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	return(SUCCESS);
}

BRESULT makenewtrack_dcp(FDDFILE fdd) {

#if 1
	FILEH	hdl;
//	UINT	curtrack;
	UINT	newtrack;

	UINT32	tracksize;
	UINT32	length;
	UINT	size;
	UINT	rsize;
//	int		t;
	UINT8	tmp[0x0400];
//	UINT32	cur;

	int		i;
	UINT32	ptr;
	UINT32	fdsize;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	fdsize = file_getsize(hdl);

	newtrack = (fdc.treg[fdc.us] << 1) + fdc.hd;
	tracksize = fdd->inf.xdf.sectors * (128 << fdd->inf.xdf.n);
	if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && newtrack == 0) {
		tracksize /= 2;
	}
	//	���炵�n�߂�I�t�Z�b�g�擾
	ptr = 0;
	for (i = newtrack; i < DCP_TRACKMAX; i++) {
		if (fdd->inf.dcp.ptr[i] != 0) {
			ptr = fdd->inf.dcp.ptr[i];
			break;
		}
	}
	if (ptr != 0) {
//		ptr += fdd->inf.xdf.headersize;
		length = fdsize - ptr;

		while(length) {
			if (length >= (long)(sizeof(tmp))) {
				size = sizeof(tmp);
			}
			else {
				size = length;
			}
			length -= size;
			file_seek(hdl, ptr + length, 0);
			rsize = file_read(hdl, tmp, size);
			file_seek(hdl, ptr + length + tracksize, 0);
			file_write(hdl, tmp, rsize);
		}

		//	�e�g���b�N�̃I�t�Z�b�g�Čv�Z
		fdd->inf.dcp.ptr[newtrack] = ptr;
		ptr += tracksize;
		for (i = newtrack+1; i < DCP_TRACKMAX; i++) {
			if (fdd->inf.dcp.ptr[i] != 0) {
				fdd->inf.dcp.ptr[i] = ptr;
				ptr += tracksize;
			}
		}
	}
	else {
		fdd->inf.dcp.ptr[newtrack] = fdsize;
	}

	file_close(hdl);

	return(SUCCESS);
#else
	return(FAILURE);
#endif
}

BRESULT refreshheader_dcp(FDDFILE fdd) {

	FILEH	hdl;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	file_seek(hdl, 0, 0);
	file_write(hdl, &fdd->inf.dcp.head, DCP_HEADERSIZE);
	file_close(hdl);

	return(SUCCESS);
}


BRESULT fdd_write_dcp(FDDFILE fdd) {

	FILEH	hdl;
	UINT	track;
	UINT	secsize;
	long	seekp;

	fddlasterror = 0x00;
	if (fdd_seeksector_common(fdd)) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if (fdd->protect) {
		fddlasterror = 0x70;
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	track = (fdc.treg[fdc.us] << 1) + fdc.hd;

	if ((fdd->type == DISKTYPE_BETA) ||
		(fdd->type == DISKTYPE_DCP && fdd->inf.dcp.head.trackmap[track] == 0x01) ||
		(fdd->type == DISKTYPE_DCP && fdd->inf.dcp.head.alltrackflg == 0x01)) {

		secsize = 128 << fdd->inf.xdf.n;
		seekp = fdc.R - 1;
		seekp <<= (7 + fdd->inf.xdf.n);
		if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && track == 0) {
			//	BASIC-2HD�Atrack 0�p���׍H
			secsize /= 2;
			seekp /= 2;
		}
		seekp += fdd->inf.dcp.ptr[track];
//		seekp += fdd->inf.xdf.headersize;

		hdl = file_open(fdd->fname);
		if (hdl == FILEH_INVALID) {
			fddlasterror = 0xc0;
			return(FAILURE);
		}
		if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
			(file_write(hdl, fdc.buf, secsize) != secsize)) {
			file_close(hdl);
			fddlasterror = 0xc0;
			return(FAILURE);
		}
		file_close(hdl);
	}
	else {
		//	�V�K�g���b�N�}����A�ċA�Ăяo��
		BRESULT	r;
		r = makenewtrack_dcp(fdd);
		if (r != SUCCESS) {
			return r;
		}
		fdd->inf.dcp.head.trackmap[track] = 0x01;
		r = refreshheader_dcp(fdd);
		if (r != SUCCESS) {
			return r;
		}
		return(fdd_write_dcp(fdd));
	}
#if 0
	if (fdd->inf.dcp.head.trackmap[track] != 0x01) {
//	if (fdd->inf.dcp.ptr[track] == 0) {
		//	�f�[�^�̑��݂��Ȃ��g���b�N�̓G���[�ɂ��Ƃ�
//		fddlasterror = 0xc0;
//		return(FAILURE);
		//	�V�K�g���b�N�}�����w�b�_���X�V��A
		//	�ċA�Ăяo��
		BRESULT	r;
		r = makenewtrack_dcp(fdd);
		if (r != SUCCESS) {
			return r;
		}
		//	r = refreshheader_dcp(fdd);
		//	if (r != SUCCESS) {
		//		return r;
		//	}
		return(fdd_write_dcp(fdd));
	}

	secsize = 128 << fdd->inf.xdf.n;
	seekp = fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	if (fdd->inf.dcp.head.mediatype == DCP_DISK_2HD_BAS && track == 0) {
		//	BASIC-2HD�Atrack 0�p���׍H
		secsize /= 2;
		seekp /= 2;
	}
	seekp += fdd->inf.dcp.ptr[track];
	seekp += fdd->inf.xdf.headersize;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_write(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	file_close(hdl);
#endif
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	if (fdd->type == DISKTYPE_DCP ) {
		if (fdd->inf.dcp.head.trackmap[track] != 0x01) {
			fdd->inf.dcp.head.trackmap[track] = 0x01;
			refreshheader_dcp(fdd);
		}
	}
	return(SUCCESS);
}

#endif