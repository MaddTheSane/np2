TEMPLATE= app

TARGET= xnp2

CONFIG+= qt warn_on release

# kluge for NetBSD
QMAKE_RUN_CC      = $$QMAKE_CC -c $(CFLAGS) $(INCPATH) -o $obj $src
QMAKE_RUN_CC_IMP  = $$QMAKE_CC -c $(CFLAGS) $(INCPATH) -o $@ $<
QMAKE_RUN_CXX     = $$QMAKE_CXX -c $(CXXFLAGS) $(INCPATH) -o $obj $src
QMAKE_RUN_CXX_IMP = $$QMAKE_CXX -c $(CXXFLAGS) $(INCPATH) -o $@ $<

# Load config
exists(qtconfig.tmpl) {
	include (qtconfig.tmpl)
}

# set SYSRESPATH
isEmpty(SYSRESPATH) {
	SYSRESPATH=/usr/X11R6/lib/X11/xnp2
}
DEFINES+= SYSRESPATH=\"$$SYSRESPATH\"


#
# audio part
#

# SDL_mixer
contains(SOUND_DRIVER, SDLMIXER) {
	CONFIG*= thread
	DEFINES+= USE_SDLMIXER
	QMAKE_CFLAGS*= `sdl-config --cflags`
	QMAKE_CXXFLAGS*= `sdl-config --cflags`
	QMAKE_LIBS*= `sdl-config --libs`
	QMAKE_LIBS+= -lSDL_mixer
}


#
# common part
#

DEFINES+= USE_QT=1

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
          ../../bios/bios1a.c ../../bios/bios1b.c ../../bios/bios1c.c \
          ../../bios/sxsicios.c

SOURCES+= ../../cbus/cbuscore.c ../../cbus/pc9861k.c ../../cbus/mpu98ii.c \
          ../../cbus/amd98.c ../../cbus/board118.c ../../cbus/board14.c \
          ../../cbus/board26k.c ../../cbus/board86.c ../../cbus/boardspb.c \
          ../../cbus/boardx2.c ../../cbus/cs4231io.c ../../cbus/pcm86io.c \
          ../../cbus/saciio.c ../../cbus/scsicmd.c ../../cbus/scsiio.c

SOURCES+= ../../fdd/diskdrv.c ../../fdd/newdisk.c ../../fdd/fddfile.c \
          ../../fdd/fdd_xdf.c ../../fdd/fdd_d88.c ../../fdd/fdd_mtr.c \
          ../../fdd/sxsi.c

SOURCES+= ../../font/font.c ../../font/fontdata.c ../../font/fontmake.c \
          ../../font/fontpc88.c ../../font/fontpc98.c ../../font/fontv98.c \
          ../../font/fontfm7.c ../../font/fontx1.c ../../font/fontx68k.c \

SOURCES+= ../../generic/cmjasts.c \
          ../../generic/hostdrv.c ../../generic/hostdrvs.c

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
          ../../sound/tms3631c.c ../../sound/tms3631g.c \
          ../../sound/wavemix.c

SOURCES+= ../../sound/getsnd/getsnd.c ../../sound/getsnd/getsmix.c \
          ../../sound/getsnd/getmp3.c ../../sound/getsnd/getogg.c \
          ../../sound/getsnd/getwave.c

SOURCES+= ../../sound/vermouth/midiout.c ../../sound/vermouth/midimod.c \
          ../../sound/vermouth/midinst.c ../../sound/vermouth/midvoice.c \
          ../../sound/vermouth/midtable.c

SOURCES+= ../../vram/vram.c ../../vram/scrndraw.c ../../vram/sdraw.c \
          ../../vram/dispsync.c ../../vram/palettes.c ../../vram/maketext.c \
          ../../vram/maketgrp.c ../../vram/makegrph.c ../../vram/scrnbmp.c

SOURCES+= ../dosio.c ../ini.c ../trace.c ../cmmidi.c ../np2.c ../toolkit.c \
          ../commng.c ../drawmng.c ../joymng.c ../kbdmng.c ../soundmng.c \
	  ../sysmng.c ../taskmng.c ../timemng.c \
          ../main.c

SOURCES+= ../snddrv/esd.c ../snddrv/netbsd.c ../snddrv/oss.c ../snddrv/sdl.c

SOURCES+= qtdraw.cpp qtfont.cpp qtkeyboard.cpp qtmouse.cpp \
          qtmain.cpp qtwrapper.cpp

HEADERS+= xnp2.h qtdraw.h

TRANSLATIONS= xnp2_ja.ts
