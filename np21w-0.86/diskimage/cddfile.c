#include	"compiler.h"
#include	"dosio.h"
#include	"textfile.h"
#include	"cpucore.h"
#include	"fdd/sxsi.h"
#include	"cddfile.h"

#ifdef SUPPORT_KAI_IMAGES

#include	"DiskImage/img_strres.h"
#include	"DiskImage/win9x/img_dosio.h"
#include	"DiskImage/CD/cdd_iso.h"

//	ISO9660�̃{�����[���L�q�q�ɂ��`�F�b�N��L���ɂ���ꍇ�̓R�����g���O��
//	���L���ɂ����ꍇ�ACD-ROM�ȊO���}�E���g�ł��Ȃ��Ȃ�
//#define	CHECK_ISO9660

#ifdef	CHECK_ISO9660
static const UINT8 cd001[7] = {0x01,'C','D','0','0','1',0x01};
#endif

//	�ǉ�(kaiA)
BOOL isCDImage(const OEMCHAR *fname) {

const OEMCHAR	*ext;

	ext = file_getext(fname);
	if ((!file_cmpname(ext, str_cue)) ||
		(!file_cmpname(ext, str_ccd)) ||
		(!file_cmpname(ext, str_cdm)) ||
		(!file_cmpname(ext, str_mds)) ||
		(!file_cmpname(ext, str_nrg)) ||
		(!file_cmpname(ext, str_iso))) {
		return TRUE;
	}
	return FALSE;
}
//

long issec2048(FILEH fh) {

#ifdef	CHECK_ISO9660
	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
#endif
	UINT	fsize;

#ifdef	CHECK_ISO9660
	fpos = 16 * 2048;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2048_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2048_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2048_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2048_err;
	}
#endif
	fsize = file_getsize(fh);
	if ((fsize % 2048) != 0) {
		goto sec2048_err;
	}
	return(fsize / 2048);

sec2048_err:
	return(-1);
}

long issec2352(FILEH fh) {

#ifdef	CHECK_ISO9660
	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
#endif
	UINT	fsize;

#ifdef	CHECK_ISO9660
	fpos = (16 * 2352) + 16;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2352_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2352_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2352_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2352_err;
	}
#endif
	fsize = file_getsize(fh);
	if ((fsize % 2352) != 0) {
		goto sec2352_err;
	}
	return(fsize / 2352);

sec2352_err:
	return(-1);
}

long issec2448(FILEH fh) {

#ifdef	CHECK_ISO9660
	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
#endif
	UINT	fsize;

#ifdef	CHECK_ISO9660
	fpos = (16 * 2448) + 16;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2448_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2448_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2448_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2448_err;
	}
#endif
	fsize = file_getsize(fh);
	if ((fsize % 2448) != 0) {
		goto sec2448_err;
	}
	return(fsize / 2448);

sec2448_err:
	return(-1);
}

long issec(FILEH fh, _CDTRK *trk, UINT trks) {

#ifdef	CHECK_ISO9660
	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
#endif
	UINT	i;
	FILELEN	fsize;
	long	total;

	total = 0;

#ifdef	CHECK_ISO9660
	fpos = 16 * trk[0].sector_size;
	if (trk[0].sector_size != 2048) {
		fpos += 16;
	}
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec_err;
	}
#endif

	if (trks == 1) {
		trk[0].sector_size = 2048;
		trk[0].str_sec = 0;
		total = issec2048(fh);
		if (total < 0) {
			trk[0].sector_size = 2352;
			total = issec2352(fh);
		}
		if (total < 0) {
			trk[0].sector_size = 2448;
			total = issec2448(fh);
		}
		if (total < 0) {
			return(-1);
		}
		else {
			trk[0].end_sec = total - 1;
			trk[0].sectors = total;
			return(total);
		}
	}

	fsize = file_getsize(fh);
	if (trk[0].pos0 == 0) {
		trk[0].str_sec = trk[0].pos;
	}
	else {
		trk[0].str_sec = trk[0].pos0;
	}
	for (i = 1; i < trks; i++) {
		if (trk[i].pos0 == 0) {
			trk[i].str_sec = trk[i].pos;
		}
		else {
			trk[i].str_sec = trk[i].pos0;
		}
		trk[i-1].end_sec = trk[i].str_sec - 1;
		trk[i-1].sectors = trk[i-1].end_sec - trk[i-1].str_sec + 1;
		total += trk[i-1].sectors;
		fsize -= trk[i-1].sectors * trk[i-1].sector_size;
	}
	if (fsize % trk[trks-1].sector_size != 0) {
		return(-1);
	}
	if (trk[trks-1].pos0 == 0) {
		trk[trks-1].str_sec = trk[trks-1].pos;
	}
	else {
		trk[trks-1].str_sec = trk[trks-1].pos0;
	}
	trk[trks-1].end_sec = trk[trks-1].str_sec + (fsize / trk[trks-1].sector_size);
	trk[trks-1].sectors = trk[trks-1].end_sec - trk[trks-1].str_sec + 1;
	total += trk[trks-1].sectors;

	return(total);

#ifdef	CHECK_ISO9660
sec_err:
	return(-1);
#endif
}

//	��CDTRK�\���̓���
//		UINT32	str_sec;
//		UINT32	end_sec;
//		UINT32	sectors;
//		���̃����o�̐ݒ�
long set_trkinfo(FILEH fh, _CDTRK *trk, UINT trks, FILELEN imagesize) {

	UINT	i;
	FILELEN	fsize;
	long	total;

	total = 0;

	if (trks == 1) {
		trk[0].sector_size = 2048;
		trk[0].str_sec = 0;
		total = issec2048(fh);
		if (total < 0) {
			trk[0].sector_size = 2352;
			total = issec2352(fh);
		}
		if (total < 0) {
			trk[0].sector_size = 2448;
			total = issec2448(fh);
		}
		if (total < 0) {
			return(-1);
		}
		else {
			trk[0].end_sec = total - 1;
			trk[0].sectors = total;
			return(total);
		}
	}

	if (imagesize == 0) {
		fsize = file_getsize(fh);
	}
	else {
		fsize = imagesize;
	}
	if (trk[0].pos0 == 0) {
		trk[0].str_sec = trk[0].pos;
	}
	else {
		trk[0].str_sec = trk[0].pos0;
	}
	for (i = 1; i < trks; i++) {
		if (trk[i].pos0 == 0) {
			trk[i].str_sec = trk[i].pos;
		}
		else {
			trk[i].str_sec = trk[i].pos0;
		}
		trk[i-1].end_sec = trk[i].str_sec - 1;
		trk[i-1].sectors = trk[i-1].end_sec - trk[i-1].str_sec + 1;
		total += trk[i-1].sectors;
		fsize -= trk[i-1].sectors * trk[i-1].sector_size;
	}
	if (fsize % trk[trks-1].sector_size != 0) {
		return(-1);
	}
	if (trk[trks-1].pos0 == 0) {
		trk[trks-1].str_sec = trk[trks-1].pos;
	}
	else {
		trk[trks-1].str_sec = trk[trks-1].pos0;
	}
	trk[trks-1].end_sec = trk[trks-1].str_sec + (fsize / trk[trks-1].sector_size);
	trk[trks-1].sectors = trk[trks-1].end_sec - trk[trks-1].str_sec + 1;
	total += trk[trks-1].sectors;

	return(total);
}


//	----
//	�C���[�W�t�@�C�����S�g���b�N�Z�N�^��2048byte�p
REG8 sec2048_read(SXSIDEV sxsi, FILEPOS pos, UINT8 *buf, UINT size) {

	CDINFO	cdinfo;
	FILEH	fh;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}

	cdinfo = (CDINFO)sxsi->hdl;
	fh = cdinfo->fh;

	pos = pos * 2048 + cdinfo->trk[0].start_offset;
	if (file_seek(fh, pos, FSEEK_SET) != pos) {
		return(0xd0);
	}
	while(size) {
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}


//	�C���[�W�t�@�C�����S�g���b�N�Z�N�^��2352byte�p
REG8 sec2352_read(SXSIDEV sxsi, FILEPOS pos, UINT8 *buf, UINT size) {

	CDINFO	cdinfo;
	FILEH	fh;
	FILEPOS	fpos;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}

	cdinfo = (CDINFO)sxsi->hdl;
	fh = cdinfo->fh;

	while(size) {
		fpos = (pos * 2352) + 16 + cdinfo->trk[0].start_offset;
		if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}


//	�C���[�W�t�@�C�����S�g���b�N�Z�N�^��2448(2352+96)�p
REG8 sec2448_read(SXSIDEV sxsi, FILEPOS pos, UINT8 *buf, UINT size) {

	CDINFO	cdinfo;
	FILEH	fh;
	FILEPOS	fpos;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}

	cdinfo = (CDINFO)sxsi->hdl;
	fh = cdinfo->fh;
	while(size) {
		fpos = (pos * 2448) + 16 + cdinfo->trk[0].start_offset;
		if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}


//	�C���[�W�t�@�C�����Z�N�^�����ݗp
//		��RAW(2048byte)�{Audio(2352byte)��
REG8 sec_read(SXSIDEV sxsi, FILEPOS pos, UINT8 *buf, UINT size) {

	CDINFO	cdinfo;
	FILEH	fh;
	FILEPOS	fpos;
	UINT	rsize;
	UINT	i;
	UINT32	secs;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}

	cdinfo = (CDINFO)sxsi->hdl;
	fh = cdinfo->fh;

	while (size) {
		fpos = 0;
		secs = 0;
		for (i = 0; i < cdinfo->trks; i++) {
			if (cdinfo->trk[i].str_sec <= pos && pos <= cdinfo->trk[i].end_sec) {
				fpos += (pos - secs) * cdinfo->trk[i].sector_size;
				if (cdinfo->trk[i].sector_size != 2048) {
					fpos += 16;
				}
				break;
			}
			fpos += cdinfo->trk[i].sectors * cdinfo->trk[i].sector_size;
			secs += cdinfo->trk[i].sectors;
		}
		fpos += cdinfo->trk[0].start_offset;
		if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}

//	----
BRESULT cd_reopen(SXSIDEV sxsi) {

	CDINFO	cdinfo;
	FILEH	fh;

	cdinfo = (CDINFO)sxsi->hdl;
	fh = file_open_rb(cdinfo->path);
	if (fh != FILEH_INVALID) {
		cdinfo->fh = fh;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

void cd_close(SXSIDEV sxsi) {

	CDINFO	cdinfo;

	cdinfo = (CDINFO)sxsi->hdl;
	file_close(cdinfo->fh);
}

void cd_destroy(SXSIDEV sxsi) {

	_MFREE((CDINFO)sxsi->hdl);
}
//	----

void set_secread(SXSIDEV sxsi, const _CDTRK *trk, UINT trks) {

	UINT		i;
	UINT16		secsize;

	secsize = trk[0].sector_size;
	for (i = 1; i < trks; i++) {
		if (secsize != trk[i].sector_size) {
			secsize = 0;
			break;
		}
	}
	if (secsize != 0) {
		switch (secsize) {
			case	2048:
				sxsi->read = sec2048_read;
				break;
			case	2352:
				sxsi->read = sec2352_read;
				break;
			case	2448:
				sxsi->read = sec2448_read;
				break;
		}
	}
	else {
		sxsi->read = sec_read;
	}
}

//
//#define	TOCLOGOUT
#ifdef	TOCLOGOUT
#define	TOCLOG(fmt, val)	\
			_stprintf(logbuf, fmt, val);	\
			textfile_write(tfh, logbuf);
static const OEMCHAR str_logB[] = OEMTEXT("._CDTRK.Before.log");
static const OEMCHAR str_logA[] = OEMTEXT("._CDTRK.After.log");
#endif
//

//	�C���[�W�t�@�C���̎��̂��J���A�e����\�z
BRESULT setsxsidev(SXSIDEV sxsi, const OEMCHAR *path, const _CDTRK *trk, UINT trks) {

	FILEH	fh;
	long	totals;
	CDINFO	cdinfo;
	UINT	mediatype;
	UINT	i;
#ifdef	TOCLOGOUT
	OEMCHAR		logpath[MAX_PATH];
	OEMCHAR		logbuf[2048];
	TEXTFILEH	tfh;
#endif

	//	trk�Atrks�͗L���Ȓl���ݒ�ς݂Ȃ̂��O��
	if ((trk == NULL) || (trks == 0)) {
		goto sxsiope_err1;
	}

	fh = file_open_rb(path);
	if (fh == FILEH_INVALID) {
		goto sxsiope_err1;
	}

	cdinfo = (CDINFO)_MALLOC(sizeof(_CDINFO), path);
	if (cdinfo == NULL) {
		goto sxsiope_err2;
	}
	ZeroMemory(cdinfo, sizeof(_CDINFO));
	cdinfo->fh = fh;
	trks = min(trks, NELEMENTS(cdinfo->trk) - 1);
	CopyMemory(cdinfo->trk, trk, trks * sizeof(_CDTRK));

#ifdef	TOCLOGOUT
	file_cpyname(logpath, path, NELEMENTS(logpath));
	file_cutext(logpath);
	file_catname(logpath, str_logB, NELEMENTS(logpath));

	tfh = textfile_create(logpath, 0x800);
	if (tfh == NULL) {
		return(FAILURE);
	}

	TOCLOG(OEMTEXT("STR _CDTRK LOG\r\n"), 0);
	for (i = 0; i < trks; i++) {
		TOCLOG(OEMTEXT("trk[%02d]\r\n"), i);
		TOCLOG(OEMTEXT("  adr_ctl        = 0x%02X\r\n"),     cdinfo->trk[i].adr_ctl);
		TOCLOG(OEMTEXT("  point          = %02d\r\n"),       cdinfo->trk[i].point);
		TOCLOG(OEMTEXT("  [pos0][pos][ ]              = [%18I32d]"), cdinfo->trk[i].pos0);
		TOCLOG(OEMTEXT("[%18I32d][                  ]\r\n"),         cdinfo->trk[i].pos);
		TOCLOG(OEMTEXT("  sec[ ][str][end]            = [                  ][%18I32d]"), cdinfo->trk[i].str_sec);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].end_sec);
		TOCLOG(OEMTEXT("  sectors        = %I32d\r\n"),      cdinfo->trk[i].sectors);
		TOCLOG(OEMTEXT("  sector_size    = %d\r\n"),         cdinfo->trk[i].sector_size);
		TOCLOG(OEMTEXT("  sector [pregap][start][end] = [%18I32d]"), cdinfo->trk[i].pregap_sector);
		TOCLOG(OEMTEXT("[%18I32d]"),     cdinfo->trk[i].start_sector);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].end_sector);
		TOCLOG(OEMTEXT("  img_sec[pregap][start][end] = [%18I32d]"), cdinfo->trk[i].img_pregap_sec);
		TOCLOG(OEMTEXT("[%18I32d]"),     cdinfo->trk[i].img_start_sec);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].img_end_sec);
		TOCLOG(OEMTEXT("  offset [pregap][start][end] = [0x%016I64X]"), cdinfo->trk[i].pregap_offset);
		TOCLOG(OEMTEXT("[0x%016I64X]"),     cdinfo->trk[i].start_offset);
		TOCLOG(OEMTEXT("[0x%016I64X]\r\n"), cdinfo->trk[i].end_offset);
		TOCLOG(OEMTEXT("  pregap_sectors = %I32d\r\n"),      cdinfo->trk[i].pregap_sectors);
		TOCLOG(OEMTEXT("  track_sectors  = %I32d\r\n"),      cdinfo->trk[i].track_sectors);
	}
	TOCLOG(OEMTEXT("END _CDTRK LOG\r\n"), 0);

	textfile_close(tfh);
#endif

#if 1
	if (sxsi->totals == -1) {
		totals = set_trkinfo(fh, cdinfo->trk, trks, 0);
		if (totals < 0) {
			goto sxsiope_err3;
		}
		sxsi->totals = totals;
	}
#else
	totals = issec(fh, cdinfo->trk, trks);	//	�Ƃ肠����
	sxsi->read = sec2048_read;
	totals = issec2048(cdinfo->fh);
	if (totals < 0) {
		sxsi->read = sec2352_read;
		totals = issec2352(cdinfo->fh);
	}
	if (totals < 0) {
		sxsi->read = sec2448_read;
		totals = issec2448(cdinfo->fh);
	}
	if (totals < 0) {
		sxsi->read = sec_read;
		totals = issec(cdinfo->fh, cdinfo->trk, trks);
	}
	if (totals < 0) {
		goto sxsiope_err3;
	}
#endif

	mediatype = 0;
	for (i = 0; i < trks; i++) {
		if (cdinfo->trk[i].adr_ctl == TRACK_DATA) {
			mediatype |= SXSIMEDIA_DATA;
		}
		else if (cdinfo->trk[i].adr_ctl == TRACK_AUDIO) {
			mediatype |= SXSIMEDIA_AUDIO;
		}
	}

	//	���[�h�A�E�g�g���b�N�𐶐�
	cdinfo->trk[trks].adr_ctl	= 0x10;
	cdinfo->trk[trks].point		= 0xaa;
//	cdinfo->trk[trks].pos		= totals;
	cdinfo->trk[trks].pos		= sxsi->totals;

	cdinfo->trks = trks;
	file_cpyname(cdinfo->path, path, NELEMENTS(cdinfo->path));

	sxsi->reopen		= cd_reopen;
	sxsi->close			= cd_close;
	sxsi->destroy		= cd_destroy;
	sxsi->hdl			= (INTPTR)cdinfo;
//	sxsi->totals		= totals;
	sxsi->cylinders		= 0;
	sxsi->size			= 2048;
	sxsi->sectors		= 1;
	sxsi->surfaces		= 1;
	sxsi->headersize	= 0;
	sxsi->mediatype		= mediatype;

#ifdef	TOCLOGOUT
	file_cpyname(logpath, path, NELEMENTS(logpath));
	file_cutext(logpath);
	file_catname(logpath, str_logA, NELEMENTS(logpath));

	tfh = textfile_create(logpath, 0x800);
	if (tfh == NULL) {
		return(FAILURE);
	}

	TOCLOG(OEMTEXT("STR _CDTRK LOG\r\n"), 0);
	for (i = 0; i < trks; i++) {
		TOCLOG(OEMTEXT("trk[%02d]\r\n"), i);
		TOCLOG(OEMTEXT("  adr_ctl        = 0x%02X\r\n"),     cdinfo->trk[i].adr_ctl);
		TOCLOG(OEMTEXT("  point          = %02d\r\n"),       cdinfo->trk[i].point);
		TOCLOG(OEMTEXT("  [pos0][pos][ ]              = [%18I32d]"), cdinfo->trk[i].pos0);
		TOCLOG(OEMTEXT("[%18I32d][                  ]\r\n"),         cdinfo->trk[i].pos);
		TOCLOG(OEMTEXT("  sec[ ][str][end]            = [                  ][%18I32d]"), cdinfo->trk[i].str_sec);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].end_sec);
		TOCLOG(OEMTEXT("  sectors        = %I32d\r\n"),      cdinfo->trk[i].sectors);
		TOCLOG(OEMTEXT("  sector_size    = %d\r\n"),         cdinfo->trk[i].sector_size);
		TOCLOG(OEMTEXT("  sector [pregap][start][end] = [%18I32d]"), cdinfo->trk[i].pregap_sector);
		TOCLOG(OEMTEXT("[%18I32d]"),     cdinfo->trk[i].start_sector);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].end_sector);
		TOCLOG(OEMTEXT("  img_sec[pregap][start][end] = [%18I32d]"), cdinfo->trk[i].img_pregap_sec);
		TOCLOG(OEMTEXT("[%18I32d]"),     cdinfo->trk[i].img_start_sec);
		TOCLOG(OEMTEXT("[%18I32d]\r\n"), cdinfo->trk[i].img_end_sec);
		TOCLOG(OEMTEXT("  offset [pregap][start][end] = [0x%016I64X]"), cdinfo->trk[i].pregap_offset);
		TOCLOG(OEMTEXT("[0x%016I64X]"),     cdinfo->trk[i].start_offset);
		TOCLOG(OEMTEXT("[0x%016I64X]\r\n"), cdinfo->trk[i].end_offset);
		TOCLOG(OEMTEXT("  pregap_sectors = %I32d\r\n"),      cdinfo->trk[i].pregap_sectors);
		TOCLOG(OEMTEXT("  track_sectors  = %I32d\r\n"),      cdinfo->trk[i].track_sectors);
	}
	TOCLOG(OEMTEXT("END _CDTRK LOG\r\n"), 0);

	textfile_close(tfh);
#endif

	return(SUCCESS);

sxsiope_err3:
	_MFREE(cdinfo);

sxsiope_err2:
	file_close(fh);

sxsiope_err1:
	return(FAILURE);
}

#endif