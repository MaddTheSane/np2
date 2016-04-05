/**
 * @file	net.h
 * @brief	Virtual LAN Interface
 *
 * @author	$Author: SimK $
 */

typedef void NP2NET_PacketHandler(const UINT8 *buf, int size);

// send_packet�̓f�[�^��LAN�ɑ��M�������Ƃ��ɊO����Ă΂�܂��B�f�[�^�𑗐M����֐�������ăZ�b�g���Ă���Ă��������B
// recieve_packet��LAN����f�[�^����M�����Ƃ��ɌĂ�ł��������B���̊֐��̓��Z�b�g���Ƀf�o�C�X���Z�b�g���ɗ���̂ō��K�v�͂���܂���B
// ���݂�TAP�f�o�C�X�݂̂̃T�|�[�g�ł����Asend_packet��recieve_packet�ɑ������镨������Ă���TAP�ȊO�ł�OK�Ȃ͂�
typedef struct {
	NP2NET_PacketHandler	*send_packet;
	NP2NET_PacketHandler	*recieve_packet;
} NP2NET;

#ifdef __cplusplus
extern "C" void np2net_init(void);
extern "C" void np2net_shutdown(void);
#else
extern void np2net_init(void);
extern void np2net_shutdown(void);
#endif
void np2net_reset(const NP2CFG *pConfig);
void np2net_bind(void);

extern	NP2NET	np2net;