http://kohada.2ch.net/test/read.cgi/gameurawaza/1322351485/
PC98エミュ総合 パート4

http://kohada.2ch.net/test/read.cgi/gameurawaza/1322351485/793
793 名前：名無しさん＠お腹いっぱい。[sage] 投稿日：2012/02/03(金) 12:57:16.33 ID:2GNrjSvL [1/3]
>>783
おつ

最近fmgenで「闇の稜線」がメーカーロゴまで進まない
調べたら>>232からですね
他のゲームが動くようになるみたいだからトレードオフですね
Xnp2では動くから使い分けします

http://kohada.2ch.net/test/read.cgi/gameurawaza/1322351485/800
800 名前：名無しさん＠お腹いっぱい。[sage] 投稿日：2012/02/03(金) 22:52:05.32 ID:Pb/w9HuR
TRACEかましてログ見たところPCMのFIFOに出力がされていないのに
FIFO割り込み要求フラグが要求ありに設定されるのが原因…かな？
pcm86_ia468関数内の
　>if (pcm86.virbuf <= pcm86.fifosize) {
　のif分に条件「&& pcm86.virbuf > 0」を追加
　※「むしろこう？」の方ね
…で>>793は回避出来てる…気はするんだけどこれでいいのだろか？

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
 #elif 1		// むしろこう？
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
