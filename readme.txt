

// ---- screen

�@PC-9801�V���[�Y�̉�ʃT�C�Y�͕W���� 641x400�B
�@VGA�ł͎��܂�Ȃ��̂� �����I��VGA�Ɏ��߂�ׂ� ��ʉ��T�C�Y�� width + extend
�Ƃ���B
�@8 < width < 640
  8 < height < 480
  extend = 0 or 1

typedef struct {
	BYTE	*ptr;		// VRAM�|�C���^
	int		xalign;		// x�����I�t�Z�b�g
	int		yalign;		// y�����I�t�Z�b�g
	int		width;		// ����
	int		height;		// �c��
	UINT	bpp;		// �X�N���[���F�r�b�g
	int		extend;		// ���g��
} SCRNSURF;

�@�T�[�t�F�X�T�C�Y�� (width + extern) x height�B


const SCRNSURF *scrnmng_surflock(void);
�@��ʕ`��J�n

void scrnmng_surfunlock(const SCRNSURF *surf);
�@��ʕ`��I��(���̃^�C�~���O�ŕ`��)


void scrnmng_setwidth(int posx, int width)
void scrnmng_setextend(int extend)
void scrnmng_setheight(int posy, int height)
�@�`��T�C�Y�̕ύX
�@�E�B���h�E�T�C�Y�̕ύX����
�@�t���X�N���[�����ł���� �\���̈��ύX�B
�@SCRNSURF�ł͂��̒l��Ԃ��悤�ɂ���
�@posx, width�� 8�̔{��

BOOL scrnmng_isfullscreen(void)
�@�t���X�N���[����Ԃ̎擾
�@�@return: ��0�Ńt���X�N���[��

BOOL scrnmng_haveextend(void)
�@������Ԃ̎擾
�@�@return: ��0�� �����g���T�|�[�g

UINT scrnmng_getbpp(void)
�@�X�N���[���F�r�b�g���̎擾
�@�@return: �r�b�g��(8/16/24/32)

void scrnmng_palchanged(void)
�@�p���b�g�X�V�̒ʒm(8bit�X�N���[���T�|�[�g���̂�)

RGB16 scrnmng_makepal16(RGB32 pal32)
�@RGB32���� 16bit�F���쐬����B(16bit�X�N���[���ɃT�|�[�g���̂�)



// ---- sound

NP2�̃T�E���h�f�[�^�� sound.c�̈ȉ��̊֐����擾
  const SINT32 *sound_pcmlock(void)
  void sound_pcmunlock(const SINT32 *hdl)


SOUND_CRITICAL  �Z�}�t�H������(see sndcsec.c)
SOUNDRESERVE    �\��o�b�t�@�̃T�C�Y(�~���b)
�@�T�E���h�����荞�ݏ�������ꍇ�̎w��B
�@���荞�݂̍ő剄�؎��Ԃ�SOUNDRESERVE�Ŏw��B
�@(Win9x�̏ꍇ�A���O�Ń����O�o�b�t�@��������̂� ���荞�ݖ����E�w�莞�Ԓʂ��
�@�T�E���h���C�g������̂ŁA���̏����͕s�v������)


UINT soundmng_create(UINT rate, UINT ms)
�@�T�E���h�X�g���[���̊m��
    input:  rate    �T���v�����O���[�g(11025/22050/44100)
            ms      �T���v�����O�o�b�t�@�T�C�Y(�~���b)
�@�@return: �l�������o�b�t�@�̃T���v�����O��

            ms�ɏ]���K�v�͂Ȃ�(SDL�Ƃ��o�b�t�@�T�C�Y�����肳���̂�)
            NP2�̃T�E���h�o�b�t�@����� �Ԃ�l�݂̂𗘗p���Ă��܂��B


void soundmng_destroy(void)
�@�T�E���h�X�g���[���̏I��

void soundmng_reset(void)
�@�T�E���h�X�g���[���̃��Z�b�g

void soundmng_play(void)
�@�T�E���h�X�g���[���̍Đ�

void soundmng_stop(void)
�@�T�E���h�X�g���[���̒�~

void soundmng_sync(void)
�@�T�E���h�X�g���[���̃R�[���o�b�N

void soundmng_setreverse(BOOL reverse)
�@�T�E���h�X�g���[���̏o�͔��]�ݒ�
    input:  reverse ��0�ō��E���]

BOOL soundmng_pcmplay(UINT num, BOOL loop)
�@PCM�Đ�
    input:  num     PCM�ԍ�
            loop    ��0�Ń��[�v

void soundmng_pcmstop(UINT num)
�@PCM��~
    input:  num     PCM�ԍ�



// ---- mouse

BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear)
  �}�E�X�̏�Ԏ擾
    input:  clear   ��0�� ��Ԃ��擾��ɃJ�E���^�����Z�b�g����
    output: *x      clear�����x�����J�E���g
            *y      clear�����y�����J�E���g
    return: bit7    ���{�^���̏�� (0:����)
            bit5    �E�{�^���̏�� (0:����)



// ---- serial/parallel/midi

COMMNG commng_create(UINT device)
  �V���A���I�[�v��
    input:  �f�o�C�X�ԍ�
    return: �n���h�� (���s��NULL)


void commng_destroy(COMMNG hdl)
  �V���A���N���[�Y
    input:  �n���h�� (���s��NULL)



// ---- joy stick

BYTE joymng_getstat(void)
�@�W���C�X�e�B�b�N�̏�Ԏ擾

�@�@return: bit0    ��{�^���̏�� (0:����)
            bit1    ���{�^���̏��
            bit2    ���{�^���̏��
            bit3    �E�{�^���̏��
            bit4    �A�˃{�^���P�̏��
            bit5    �A�˃{�^���Q�̏��
            bit6    �{�^���P�̏��
            bit7    �{�^���Q�̏��



// ----

void sysmng_update(UINT bitmap)
�@��Ԃ��ω������ꍇ�ɃR�[�������B

void sysmng_cpureset(void)
�@���Z�b�g���ɃR�[�������



void taskmng_exit(void)
�@�V�X�e�����I������B



// ----

�@���s�R�[�h�̈���

�@�ȉ��̃\�[�X�R�[�h���Ōʂɐݒ肵�Ă��܂��B
�@�@(Windows�� API�ɂ���� \r\n�̏ꍇ��\n�̏ꍇ������̂Łc)
�@�Ecommon/_memory.c
�@�Edebugsub.c
�@�Estatsave.c


