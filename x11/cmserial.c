/*	$Id: cmserial.c,v 1.1 2004/03/26 13:58:51 monaka Exp $	*/

/*
 * Copyright (c) 2004 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "commng.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>


typedef struct {
	int	hdl;
} _CMSER, *CMSER;

const UINT32 cmserial_speed[10] = {
	110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};


static UINT
serialread(COMMNG self, BYTE *data)
{
	CMSER serial = (CMSER)(self + 1);
	size_t size;
	int bytes;
	int rv;

	rv = ioctl(serial->hdl, FIONREAD, &bytes);
	if (rv == 0 && bytes > 0) {
		VERBOSE(("serialread: bytes = %d", bytes));
		size = read(serial->hdl, data, 1);
		if (size == 1) {
			VERBOSE(("serialread: data = %02x", *data));
			return 1;
		}
		VERBOSE(("serialread: read failure (%s)", strerror(errno)));
	}
	return 0;
}

static UINT
serialwrite(COMMNG self, BYTE data)
{
	CMSER serial = (CMSER)(self + 1);
	size_t size;

	size = write(serial->hdl, &data, 1);
	if (size == 1) {
		VERBOSE(("serialwrite: data = %02x", data));
		return 1;
	}
	VERBOSE(("serialwrite: write failure (%s)", strerror(errno)));
	return 0;
}

static BYTE
serialgetstat(COMMNG self)
{
	CMSER serial = (CMSER)(self + 1);
	int status;
	int rv;

	rv = ioctl(serial->hdl, TIOCMGET, &status);
	if (!(status & TIOCM_DSR)) {
		VERBOSE(("serialgetstat: DSR is disable"));
		return 0x20;
	}
	VERBOSE(("serialgetstat: DSR is enable"));
	return 0x00;
}

static long
serialmsg(COMMNG self, UINT msg, long param)
{

	(void)self;
	(void)msg;
	(void)param;

	return 0;
}

static void
serialrelease(COMMNG self)
{
	CMSER serial = (CMSER)(self + 1);

	close(serial->hdl);
	_MFREE(self);
}


/* ---- interface */

COMMNG
cmserial_create(UINT port, BYTE param, UINT32 speed)
{
	static const int cmserial_cflag[10] = {
		B110, B300, B1200, B2400, B4800,
		B9600, B19200, B38400, B57600, B115200
	};
	static const int csize[] = { CS5, CS6, CS7, CS8 };
	struct termios options;
	COMMNG ret;
	CMSER serial;
	int hdl;
	UINT i;

	VERBOSE(("cmserial_create: port = %d, param = %02x, speed = %d", port, param, speed));

	if (port == 0 || port > MAX_SERIAL_PORT_NUM) {
		VERBOSE(("cmserial_create: port is invalid"));
		goto cscre_failure;
	}

	port--;
	if (np2oscfg.com[port].mout[0] == '\0') {
		VERBOSE(("cmserial_create: com device file is disable"));
		goto cscre_failure;
	}

	hdl = open(np2oscfg.com[port].mout, O_RDWR | O_NOCTTY | O_NDELAY);
	if (hdl == -1) {
		VERBOSE(("cmserial_create: open failure %s, %s", np2oscfg.com[port].mout, strerror(errno)));
		goto cscre_failure;
	}
	fcntl(hdl, F_SETFL, 0);

	/* get current options for the port */
	tcgetattr(hdl, &options);

	/* set the baud rates */
	for (i = 0; i < NELEMENTS(cmserial_speed); i++) {
		if (cmserial_speed[i] >= speed) {
			VERBOSE(("cmserial_create: spped = %d", cmserial_speed[i]));
			break;
		}
	}
	if (i >= NELEMENTS(cmserial_speed)) {
		VERBOSE(("cmserial_create: speed is invaild"));
		goto cscre_close;
	}
	cfsetispeed(&options, cmserial_cflag[i]);
	cfsetospeed(&options, cmserial_cflag[i]);

	/* set the character size bits */
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= csize[(param >> 2) & 3];
	VERBOSE(("cmserial_create: charactor size = %d", csize[(param >> 2) & 3]));

	/* set parity check */
	switch (param & 0x30) {
	case 0x10:
		VERBOSE(("cmserial_create: odd parity"));
		options.c_cflag |= PARENB;
		options.c_cflag |= PARODD;
		options.c_cflag &= ~CSTOPB;
		options.c_iflag |= INPCK | ISTRIP;
		break;

	case 0x30:
		VERBOSE(("cmserial_create: even parity"));
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_cflag &= ~CSTOPB;
		options.c_iflag |= INPCK | ISTRIP;
		break;

	default:
		VERBOSE(("cmserial_create: non parity"));
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_iflag &= ~(INPCK | ISTRIP);
		break;
	}

	/* set stop bits XXX */
#if 0
	switch (param & 0xc0) {
	case 0x80:
		dcb.StopBits = ONE5STOPBITS;
		break;

	case 0xc0:
		dcb.StopBits = TWOSTOPBITS;
		break;

	default:
		dcb.StopBits = ONESTOPBIT;
		break;
	}
#endif

	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMSER), "SERIAL");
	if (ret == NULL) {
		VERBOSE(("cmserial_create: memory alloc failure"));
		goto cscre_close;
	}

	/* set misc flag */
	options.c_cflag |= CLOCAL | CREAD;	/* enable recv and local mode */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);	/* raw input */
	options.c_oflag &= ~OPOST;	/* raw output */

	/* set the new options for the port */
	tcsetattr(hdl, TCSANOW, &options);

#if 0
	/* set Non-blocking I/O mode */
	fcntl(hdl, F_SETFL, FNDELAY);
#endif

#if 1
	ret->connect = COMCONNECT_MIDI;
#else
	ret->connect = COMCONNECT_SERIAL;
#endif
	ret->read = serialread;
	ret->write = serialwrite;
	ret->getstat = serialgetstat;
	ret->msg = serialmsg;
	ret->release = serialrelease;
	serial = (CMSER)(ret + 1);
	serial->hdl = hdl;
	return ret;

cscre_close:
	close(hdl);
cscre_failure:
	return NULL;
}
