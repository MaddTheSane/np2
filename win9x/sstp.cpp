#include	"compiler.h"
#include	<commctrl.h>
#include	"np2.h"
#include	"scrnmng.h"
#include	"sstp.h"


static	HWND		sstphwnd = NULL;
static	BOOL		sstpinused = FALSE;
static	int			sstp_stat = SSTP_READY;
static	SOCKET		hSocket = INVALID_SOCKET;
static	WSAData		wsadata;
static	char		sstpstr[0x1000];
static	char		sstprcv[0x1000];
static	DWORD		sstppos = 0;
static	void		(*sstpproc)(HWND, char *) = NULL;

static const char sendermes[] = 										\
						"SEND SSTP/1.2\r\n"								\
						"Sender: Neko Project II\r\n"					\
						"Script: \\h\\s0%s\\e\r\n"						\
						"Option: notranslate\r\n"						\
						"Charset: Shift_JIS\r\n"						\
						"\r\n";


static HANDLE check_sakura(void) {

	HANDLE	hsakura;

	if ((hsakura = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "sakura")) != NULL) {
		CloseHandle(hsakura);
	}
	return(hsakura);
}


// ------------------------------------------------------------------ Async...

BOOL sstp_send(char *msg, void *proc) {

	sockaddr_in	s_in;

	if (hSocket != INVALID_SOCKET) {
		sstp_stat = SSTP_BUSY;
		return(FAILURE);
	}
	if ((!np2oscfg.sstp) || (scrnmng_isfullscreen()) || (!check_sakura())) {
		sstp_stat = SSTP_NOSEND;
		return(FAILURE);
	}

	if ((!sstphwnd) || (WSAStartup(0x0101, &wsadata))) {
		sstp_stat = SSTP_ERROR;
		return(FAILURE);
	}

	wsprintf(sstpstr, sendermes, msg);
	sstprcv[0] = 0;
	sstppos = 0;

	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		goto sstp_senderror;
	}

	if (WSAAsyncSelect(hSocket, sstphwnd, WM_SSTP,
							FD_CONNECT + FD_READ + FD_WRITE + FD_CLOSE)) {
		goto sstp_senderror;
	}

	s_in.sin_family = AF_INET;
	*(DWORD *)(&s_in.sin_addr) = 0x0100007f;
	s_in.sin_port = htons(np2oscfg.sstpport);
	if (connect(hSocket, (sockaddr *)&s_in, sizeof(s_in)) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			goto sstp_senderror;
		}
	}
	sstp_stat = SSTP_SENDING;
	sstpproc = (void (*)(HWND, char *))proc;
	return(SUCCESS);

sstp_senderror:;
	if (hSocket != INVALID_SOCKET) {
		closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}
	WSACleanup();
	sstp_stat = SSTP_ERROR;
	return(FAILURE);
}



void sstp_connect(void) {

	if (hSocket != INVALID_SOCKET) {
		send(hSocket, sstpstr, strlen(sstpstr), 0);
	}
}

void sstp_readSocket(void) {

	if (hSocket != INVALID_SOCKET) {
		DWORD	available;
		int		len;
		char	buf[256];
		while(1) {
			if (ioctlsocket(hSocket, FIONREAD, &available) != NULL) {
				break;
			}
			if (!available) {
				break;
			}
			if (available >= sizeof(buf)) {
				available = sizeof(buf);
			}
			len = recv(hSocket, buf, available, 0);
			if (len >= (int)((sizeof(sstprcv) - 1) - sstppos)) {
				len = (int)((sizeof(sstprcv) - 1) - sstppos);
			}
			if (len > 0) {
				CopyMemory(sstprcv + sstppos, buf, len);
				sstppos += len;
				sstprcv[sstppos] = '\0';
			}
		}
	}
}

static BOOL disconnection(void) {

	if (hSocket != INVALID_SOCKET) {
		closesocket(hSocket);
		WSACleanup();
		hSocket = INVALID_SOCKET;
		sstp_stat = SSTP_READY;
		return(SUCCESS);
	}
	return(FAILURE);
}

void sstp_disconnect(void) {

	if (!disconnection()) {
		if (sstpproc) {
			sstpproc(sstphwnd, sstprcv);
		}
	}
}


// ---------------------------------------------------------------------------

void sstp_construct(HWND hwnd) {

	sstphwnd = hwnd;
	sstp_stat = SSTP_READY;
	hSocket = INVALID_SOCKET;
}

void sstp_destruct(void) {

	disconnection();
}

int sstp_result(void) {

	return(sstp_stat);
}



// ----------------------------------------------------------------- ���M����

// �֐��ꔭ�A���M�����B

BOOL sstp_sendonly(char *msg) {

	WSAData		lwsadata;
	SOCKET		lSocket;
	sockaddr_in	s_in;
	char		msgstr[0x1000];
	BOOL		ret = FAILURE;

	if ((np2oscfg.sstp) && (check_sakura()) &&
		(!WSAStartup(0x0101, &lwsadata))) {
		if ((lSocket = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
			s_in.sin_family = AF_INET;
			*(DWORD *)(&s_in.sin_addr) = 0x0100007f;
			s_in.sin_port = htons(np2oscfg.sstpport);
			if (connect(lSocket, (sockaddr *)&s_in, sizeof(s_in))
															!= SOCKET_ERROR) {
				wsprintf(msgstr, sendermes, msg);
				send(lSocket, msgstr, strlen(msgstr), 0);
				ret = SUCCESS;
			}
			closesocket(lSocket);
		}
		WSACleanup();
	}
	return(ret);
}

