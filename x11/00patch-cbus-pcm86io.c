http://kohada.2ch.net/test/read.cgi/gameurawaza/1322351485/793
http://kohada.2ch.net/test/read.cgi/gameurawaza/1322351485/800

793 ���O�F���������񁗂��������ς��B[sage] ���e���F2012/02/03(��) 12:57:16.33 ID:2GNrjSvL [1/3]
>>783
����

�ŋ�fmgen�Łu�ł̗Ő��v�����[�J�[���S�܂Ői�܂Ȃ�
���ׂ���>>232����ł���
���̃Q�[���������悤�ɂȂ�݂���������g���[�h�I�t�ł���
Xnp2�ł͓�������g���������܂�

800 ���O�F���������񁗂��������ς��B[sage] ���e���F2012/02/03(��) 22:52:05.32 ID:Pb/w9HuR
TRACE���܂��ă��O�����Ƃ���PCM��FIFO�ɏo�͂�����Ă��Ȃ��̂�
FIFO���荞�ݗv���t���O���v������ɐݒ肳���̂������c���ȁH
pcm86_ia468�֐�����
�@>if (pcm86.virbuf <= pcm86.fifosize) {
�@��if���ɏ����u&& pcm86.virbuf > 0�v��ǉ�
�@���u�ނ��낱���H�v�̕���
�c��>>793�͉���o���Ă�c�C�͂���񂾂��ǂ���ł����̂��납�H

Index: cbus/pcm86io.c
===================================================================
RCS file: /cvs/np2/cbus/pcm86io.c,v
retrieving revision 1.9
diff -u -r1.9 pcm86io.c
--- cbus/pcm86io.c	17 Jun 2004 10:09:46 -0000	1.9
+++ cbus/pcm86io.c	3 Feb 2012 16:12:11 -0000
@@ -187,20 +187,24 @@
 	REG8	ret;
 
 	ret = pcm86.fifo & (~0x10);
-#if 1
+#if 0
 	if (pcm86gen_intrq()) {
 		ret |= 0x10;
 	}
 #elif 1		// �ނ��낱���H
 	if (pcm86.fifo & 0x20) {
 		sound_sync();
-		if (pcm86.virbuf <= pcm86.fifosize) {
+		if (pcm86.virbuf > 0 && pcm86.virbuf <= pcm86.fifosize) {
+#if 0
 			if (pcm86.write) {
 				pcm86.write = 0;
 			}
 			else {
 				ret |= 0x10;
 			}
+#else
+			ret |= 0x10;
+#endif
 		}
 	}
 #else
