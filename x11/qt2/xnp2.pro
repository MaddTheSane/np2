TEMPLATE= app

TARGET= xnp2

CONFIG+= qt x11 warn_on release thread
DESTDIR= .

DEFINES+= SYSRESPATH=\"/usr/X11R6/lib/X11/xnp2\"

DEFINES+= USE_SDLMIXER
TMAKE_CFLAGS+= `sdl-config --cflags`
TMAKE_CXXFLAGS+= `sdl-config --cflags`
TMAKE_LIBS+= `sdl-config --libs` -lSDL_mixer


#
# common part
#

DEFINES+= USE_QT

INCLUDEPATH+= . .. ../.. ../../common ../../generic
INCLUDEPATH+= ../../bios ../../cbus ../../fdd ../../font
INCLUDEPATH+= ../../io ../../lio ../../vram
INCLUDEPATH+= ../../sound ../../sound/vermouth
INCLUDEPATH+= ../debug
INCLUDEPATH+= ../../i286c

SOURCES+= ../../i286c/i286c.c ../../i286c/i286c_mn.c ../../i286c/i286c_ea.c \
          ../../i286c/i286c_0f.c ../../i286c/i286c_8x.c ../../i286c/i286c_sf.c \
          ../../i286c/i286c_f6.c ../../i286c/i286c_fe.c ../../i286c/i286c_rp.c \
          ../../i286c/memory.c ../../i286c/egcmem.c ../../i286c/dmap.c \
          ../../i286c/v30patch.c

SOURCES+= ../../common/strres.c ../../common/milstr.c ../../common/_memory.c \
          ../../common/textfile.c ../../common/profile.c ../../common/rect.c \
          ../../common/lstarray.c ../../common/parts.c ../../common/codecnv.c \
          ../../common/bmpdata.c ../../common/mimpidef.c ../../common/wavefile.c

SOURCES+= ../../bios/bios.c ../../bios/bios02.c ../../bios/bios08.c \
          ../../bios/bios09.c ../../bios/bios0c.c ../../bios/bios12.c \
          ../../bios/bios13.c ../../bios/bios18.c ../../bios/bios19.c \
          ../../bios/bios1a.c ../../bios/bios1b.c ../../bios/bios1c.c

SOURCES+= ../../cbus/cbuscore.c ../../cbus/pc9861k.c ../../cbus/mpu98ii.c \
          ../../cbus/amd98.c ../../cbus/board118.c ../../cbus/board14.c \
          ../../cbus/board26k.c ../../cbus/board86.c ../../cbus/boardspb.c \
          ../../cbus/boardx2.c ../../cbus/cs4231io.c ../../cbus/pcm86io.c

SOURCES+= ../../fdd/diskdrv.c ../../fdd/newdisk.c ../../fdd/fddfile.c \
          ../../fdd/fdd_xdf.c ../../fdd/fdd_d88.c ../../fdd/fdd_mtr.c \
          ../../fdd/sxsi.c

SOURCES+= ../../font/font.c ../../font/fontdata.c ../../font/fontmake.c \
          ../../font/fontpc88.c ../../font/fontpc98.c ../../font/fontv98.c \
          ../../font/fontfm7.c ../../font/fontx1.c ../../font/fontx68k.c \

SOURCES+= ../../io/iocore.c ../../io/artic.c ../../io/cgrom.c ../../io/cpuio.c \
          ../../io/crtc.c ../../io/dipsw.c ../../io/dmac.c ../../io/egc.c \
          ../../io/extmem.c ../../io/fdc.c ../../io/fdd320.c ../../io/gdc.c \
          ../../io/gdc_sub.c ../../io/gdc_pset.c \
          ../../io/mouseif.c ../../io/nmiio.c ../../io/np2sysp.c \
          ../../io/pic.c ../../io/pit.c ../../io/printif.c ../../io/serial.c \
          ../../io/sysport.c ../../io/upd4990.c \
	  ../../io/necio.c ../../io/epsonio.c

SOURCES+= ../../lio/lio.c ../../lio/gscreen.c ../../lio/gview.c \
          ../../lio/gcolor1.c ../../lio/gcolor2.c ../../lio/gcls.c \
          ../../lio/gpset.c ../../lio/gline.c ../../lio/gput1.c

SOURCES+= ../../pccore.c ../../nevent.c ../../calendar.c ../../timing.c \
          ../../debugsub.c ../../statsave.c

SOURCES+= ../../sound/sound.c ../../sound/soundrom.c ../../sound/s98.c \
          ../../sound/adpcmc.c ../../sound/adpcmg.c \
          ../../sound/beepc.c ../../sound/beepg.c \
          ../../sound/cs4231c.c ../../sound/cs4231g.c \
          ../../sound/fmboard.c ../../sound/fmtimer.c \
          ../../sound/opngenc.c ../../sound/opngeng.c \
          ../../sound/pcm86c.c ../../sound/pcm86g.c \
          ../../sound/psggenc.c ../../sound/psggeng.c \
          ../../sound/rhythmc.c ../../sound/rhythmg.c \
          ../../sound/sndcsec.c \
          ../../sound/tms3631c.c ../../sound/tms3631g.c

SOURCES+= ../../sound/vermouth/midiout.c ../../sound/vermouth/midimod.c \
          ../../sound/vermouth/midinst.c ../../sound/vermouth/midvoice.c \
          ../../sound/vermouth/midtable.c

SOURCES+= ../../vram/vram.c ../../vram/scrndraw.c ../../vram/sdraw.c \
          ../../vram/dispsync.c ../../vram/palettes.c ../../vram/maketext.c \
          ../../vram/maketgrp.c ../../vram/makegrph.c ../../vram/scrnbmp.c

SOURCES+= ../dosio.c ../ini.c ../trace.c ../cmmidi.c ../np2.c ../toolkit.c \
          ../drawmng.c ../kbdmng.c \
          ../commng.c ../joymng.c ../soundmng.c ../sysmng.c ../taskmng.c \
          ../timemng.c \
          ../main.c

SOURCES+= ../snddrv/esd.c ../snddrv/netbsd.c ../snddrv/oss.c ../snddrv/sdl.c

SOURCES+= ../qt/qtdraw.cpp ../qt/qtfont.cpp ../qt/qtkeyboard.cpp \
          ../qt/qtmouse.cpp ../qt/qtmain.cpp ../qt/qtwrapper.cpp

HEADERS+= ../qt/xnp2.h ../qt/qtdraw.h

TRANSLATIONS= ../qt/xnp2_ja.ts
