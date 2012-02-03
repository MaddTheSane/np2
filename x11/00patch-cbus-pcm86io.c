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
 #elif 1		// ÇﬁÇµÇÎÇ±Ç§ÅH
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
