# Microsoft Developer Studio Project File - Name="np2c" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** �ҏW���Ȃ��ł������� **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=np2c - Win32 Debug
!MESSAGE ����͗L����Ҳ�̧�قł͂���܂���B ������ۼު�Ă�����ނ��邽�߂ɂ� NMAKE ���g�p���Ă��������B
!MESSAGE [Ҳ�̧�ق̴���߰�] ����ނ��g�p���Ď��s���Ă�������
!MESSAGE 
!MESSAGE NMAKE /f "np2c.mak".
!MESSAGE 
!MESSAGE NMAKE �̎��s���ɍ\�����w��ł��܂�
!MESSAGE ����� ײݏ��ϸۂ̐ݒ���`���܂��B��:
!MESSAGE 
!MESSAGE NMAKE /f "np2c.mak" CFG="np2c - Win32 Debug"
!MESSAGE 
!MESSAGE �I���\������� Ӱ��:
!MESSAGE 
!MESSAGE "np2c - Win32 Release" ("Win32 (x86) Application" �p)
!MESSAGE "np2c - Win32 Debug" ("Win32 (x86) Application" �p)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "np2c - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\crel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\x86" /I ".\dialog" /I "..\\" /I "..\common" /I "..\i286c" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\lio" /I "..\font" /I "..\generic" /D "NDEBUG" /D "TRACE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib DSOUND.LIB winmm.lib comctl32.lib wsock32.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "np2c - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\cdbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\x86" /I ".\dialog" /I "..\\" /I "..\common" /I "..\i286c" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\lio" /I "..\font" /I "..\generic" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib DSOUND.LIB winmm.lib comctl32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\bin/np2cd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "np2c - Win32 Release"
# Name "np2c - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\COMMON\BMPDATA.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\LSTARRAY.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\MILSTR.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\PARTS.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\PROFILE.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\STRRES.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\TEXTFILE.C
# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\I286C\DMAP.C
# End Source File
# Begin Source File

SOURCE=..\I286C\EGCMEM.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_0F.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_8X.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_EA.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_F6.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_FE.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_MN.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_RP.C
# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_SF.C
# End Source File
# Begin Source File

SOURCE=..\I286C\MEMORY.C
# End Source File
# Begin Source File

SOURCE=..\I286C\V30PATCH.C
# End Source File
# End Group
# Begin Group "bios"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BIOS\BIOS.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS09.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS0C.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS12.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS13.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS18.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS19.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1A.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1B.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1C.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1F.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\SXSIBIOS.C
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Group "vermouth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDIMOD.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDINST.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDIOUT.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDTABLE.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDVOICE.C
# End Source File
# End Group
# Begin Group "getsnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SOUND\GETSND\GETSMIX.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETSND.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETWAVE.C
# End Source File
# End Group
# Begin Source File

SOURCE=..\SOUND\ADPCMC.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\ADPCMG.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\BEEPC.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\BEEPG.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\CS4231C.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\CS4231G.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\FMBOARD.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\FMTIMER.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\OPNGENC.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\OPNGENG.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\PCM86C.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\PCM86G.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\PSGGENC.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\PSGGENG.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\RHYTHMC.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\S98.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\SOUND.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\SOUNDROM.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\TMS3631C.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\TMS3631G.C
# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FDD\DISKDRV.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_D88.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_MTR.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_XDF.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDDFILE.C
# End Source File
# Begin Source File

SOURCE=..\FDD\NEWDISK.C
# End Source File
# Begin Source File

SOURCE=..\FDD\SXSI.C
# End Source File
# End Group
# Begin Group "lio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\LIO\GLINE.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GPSET.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GPUT1.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GSCREEN.C
# End Source File
# Begin Source File

SOURCE=..\LIO\LIO.C
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FONT\FONT.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTDATA.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTFM7.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTMAKE.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC88.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC98.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTV98.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX1.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX68K.C
# End Source File
# End Group
# Begin Group "win9x"

# PROP Default_Filter ""
# Begin Group "dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DIALOG\D_ABOUT.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_BMP.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_CLND.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_CONFIG.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_DISK.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_MPU98.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_SCREEN.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_SOUND.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\DIALOGS.CPP
# End Source File
# End Group
# Begin Source File

SOURCE=.\COMMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\DOSIO.CPP
# End Source File
# Begin Source File

SOURCE=.\FONTMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\INI.CPP
# End Source File
# Begin Source File

SOURCE=.\JOYMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\MENU.CPP
# End Source File
# Begin Source File

SOURCE=.\MOUSEMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\Np2.cpp
# End Source File
# Begin Source File

SOURCE=.\np2.rc
# End Source File
# Begin Source File

SOURCE=.\NP2ARG.CPP
# End Source File
# Begin Source File

SOURCE=.\SCRNMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\SOUNDMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\SYSMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\TASKMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\TIMEMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\TRACE.CPP
# End Source File
# Begin Source File

SOURCE=.\WINKBD.CPP
# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\IO\ARTIC.C
# End Source File
# Begin Source File

SOURCE=..\IO\CGROM.C
# End Source File
# Begin Source File

SOURCE=..\IO\CPUIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\CRTC.C
# End Source File
# Begin Source File

SOURCE=..\IO\DIPSW.C
# End Source File
# Begin Source File

SOURCE=..\IO\DMAC.C
# End Source File
# Begin Source File

SOURCE=..\IO\EGC.C
# End Source File
# Begin Source File

SOURCE=..\IO\EMSIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\EPSONIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\FDC.C
# End Source File
# Begin Source File

SOURCE=..\IO\FDD320.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC_PSET.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC_SUB.C
# End Source File
# Begin Source File

SOURCE=..\IO\IOCORE.C
# End Source File
# Begin Source File

SOURCE=..\IO\MOUSEIF.C
# End Source File
# Begin Source File

SOURCE=..\IO\NECIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\NMIIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\NP2SYSP.C
# End Source File
# Begin Source File

SOURCE=..\IO\PIC.C
# End Source File
# Begin Source File

SOURCE=..\IO\PIT.C
# End Source File
# Begin Source File

SOURCE=..\IO\PRINTIF.C
# End Source File
# Begin Source File

SOURCE=..\IO\SERIAL.C
# End Source File
# Begin Source File

SOURCE=..\IO\SYSPORT.C
# End Source File
# Begin Source File

SOURCE=..\IO\UPD4990.C
# End Source File
# End Group
# Begin Group "cbus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CBUS\AMD98.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD118.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD14.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD26K.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD86.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARDSPB.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARDX2.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\CBUSCORE.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\CS4231IO.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\MPU98II.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\PC9861K.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\PCM86IO.C
# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\VRAM\DISPSYNC.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKEGRPH.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETEXT.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETGRP.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\PALETTES.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNBMP.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\VRAM.C
# End Source File
# End Group
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\GENERIC\CMJASTS.C
# End Source File
# Begin Source File

SOURCE=..\GENERIC\CMVER.C
# End Source File
# Begin Source File

SOURCE=..\GENERIC\DIPSWBMP.C
# End Source File
# Begin Source File

SOURCE=..\GENERIC\HOSTDRV.C
# End Source File
# Begin Source File

SOURCE=..\GENERIC\HOSTDRVS.C
# End Source File
# Begin Source File

SOURCE=..\GENERIC\NP2INFO.C
# End Source File
# End Group
# Begin Source File

SOURCE=..\CALENDAR.C
# End Source File
# Begin Source File

SOURCE=..\DEBUGSUB.C
# End Source File
# Begin Source File

SOURCE=..\KEYSTAT.C
# End Source File
# Begin Source File

SOURCE=..\NEVENT.C
# End Source File
# Begin Source File

SOURCE=..\PCCORE.C
# End Source File
# Begin Source File

SOURCE=..\STATSAVE.C
# End Source File
# Begin Source File

SOURCE=..\TIMING.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\ICONS\Np2.ico
# End Source File
# Begin Source File

SOURCE=..\ICONS\NP2DEBUG.ICO
# End Source File
# End Group
# End Target
# End Project
