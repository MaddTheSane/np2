# Microsoft Developer Studio Project File - Name="np2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=np2 - Win32 Trace
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "np2.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "np2.mak" CFG="np2 - Win32 Trace"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "np2 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 Unicode" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 VGA" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 VGA KBD" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 Trace" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 VGA test" ("Win32 (x86) Application" 用)
!MESSAGE "np2 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "np2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\rel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin/np2qvga.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Unicode"
# PROP BASE Intermediate_Dir "Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\uni"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "OSLANG_UTF8" /D "RESOURCE_US" /D "ADDON_SOFTKBD" /D "TRACE" /D "MEMTRACE /YX /FD /c"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /out:"..\bin\wince\np2u.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VGA"
# PROP BASE Intermediate_Dir "VGA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\vga"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SIZE_VGA" /D "TRACE" /D "MEMTRACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin\wince\np2vga.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VGA_KBD"
# PROP BASE Intermediate_Dir "VGA_KBD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\vgakbd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SIZE_VGA" /D "ADDON_SOFTKBD" /D "TRACE" /D "MEMTRACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin\wince\np2vgakbd.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Trace"
# PROP BASE Intermediate_Dir "Trace"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\trc"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "TRACE" /D "MEMTRACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin\wince\np2trc.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VGA_test"
# PROP BASE Intermediate_Dir "VGA_test"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\vgatest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SIZE_VGATEST" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin\wince\np2vgatest.exe"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\wince"
# PROP Intermediate_Dir "..\obj\wince\vc\dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\w32" /I ".\gx" /I "..\\" /I "..\common" /I "..\i286c" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "MEMTRACE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\bin\wince\np2dbg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "np2 - Win32 Release"
# Name "np2 - Win32 Unicode"
# Name "np2 - Win32 VGA"
# Name "np2 - Win32 VGA KBD"
# Name "np2 - Win32 Trace"
# Name "np2 - Win32 VGA test"
# Name "np2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\COMMON\_MEMORY.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\BMPDATA.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\LSTARRAY.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\MILSTR.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\PARTS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\PROFILE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\RECT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\RESIZE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\STRRES.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\TEXTFILE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\UCSCNV.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\I286C\I286C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_0F.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_8X.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_EA.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_F6.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_FE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_MN.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_RP.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\I286C_SF.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\MEMORY.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286C\V30PATCH.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "mem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\MEM\DMAV30.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\DMAX86.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\MEMEGC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\MEMEMS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\MEMEPP.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\MEMTRAM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\MEMVRAM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\IO\ARTIC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\CGROM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\CPUIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\CRTC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\DIPSW.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\DMAC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\EGC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\EMSIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\EPSONIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\FDC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\FDD320.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\GDC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\GDC_PSET.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\GDC_SUB.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\IOCORE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\MOUSEIF.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\NECIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\NMIIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\NP2SYSP.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\PIC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\PIT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\PRINTIF.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\SERIAL.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\SYSPORT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\IO\UPD4990.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "cbus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CBUS\AMD98.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD118.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD14.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD26K.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARD86.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARDSPB.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\BOARDX2.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\CBUSCORE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\CS4231IO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\MPU98II.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\PC9861K.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CBUS\PCM86IO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "bios"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BIOS\BIOS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS09.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS0C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS12.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS13.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS18.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS19.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1A.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1B.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1F.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\BIOS\SXSIBIOS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "lio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\LIO\GCIRCLE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\LIO\GLINE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\LIO\GPSET.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\LIO\GPUT1.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\LIO\GSCREEN.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\LIO\LIO.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\VRAM\DISPSYNC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKEGRPH.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETEXT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETGRP.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\PALETTES.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNBMP.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNDRAW.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAW.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAWQ16.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\VRAM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Group "vermouth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDIMOD.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDINST.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDIOUT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDTABLE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\VERMOUTH\MIDVOICE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "getsnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SOUND\GETSND\GETMP3.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETOGG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETSMIX.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETSND.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\GETSND\GETWAVE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\SOUND\ADPCMC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\ADPCMG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\BEEPC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\BEEPG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\CS4231C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\CS4231G.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\FMBOARD.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\FMTIMER.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\OPNGENC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\OPNGENG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\PCM86C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\PCM86G.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\PSGGENC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\PSGGENG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\RHYTHMC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\S98.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\SNDCSEC.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\SOUND.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\SOUNDROM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\TMS3631C.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SOUND\TMS3631G.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FDD\DISKDRV.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_D88.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_MTR.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_XDF.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\FDDFILE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\NEWDISK.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FDD\SXSI.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FONT\FONT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTDATA.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTFM7.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTMAKE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC88.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC98.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTV98.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX1.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX68K.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\GENERIC\CMJASTS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\GENERIC\CMNDRAW.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\GENERIC\CMVER.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\GENERIC\HOSTDRV.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\GENERIC\HOSTDRVS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\GENERIC\SOFTKBD.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "embed"

# PROP Default_Filter ""
# Begin Group "menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\EMBED\MENU\DLGABOUT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\DLGCFG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\DLGSCR.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\FILESEL.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENU\MENUSTR.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "menubase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\embed\MENUBASE\MENUBASE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENUBASE\MENUDLG.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUICON.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENUBASE\MENUMBOX.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENUBASE\MENURES.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENUBASE\MENUSYS.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\MENUBASE\MENUVRAM.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\embed\VRAMHDL.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\embed\VRAMMIX.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "wince"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\COMMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DOSIO.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FONTMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GX\GX.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\INI.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\INPUTMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JOYMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MOUSEMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NP2.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W32\NP2.RC
# End Source File
# Begin Source File

SOURCE=.\OEMTEXT.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SCRNMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SOUNDMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SYSMENU.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SYSMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TASKMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TIMEMNG.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W32\TRACE.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WINKBD.CPP

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\CALENDAR.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DEBUGSUB.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KEYSTAT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\NEVENT.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PCCORE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\STATSAVE.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\TIMING.C

!IF  "$(CFG)" == "np2 - Win32 Release"

!ELSEIF  "$(CFG)" == "np2 - Win32 Unicode"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA KBD"

!ELSEIF  "$(CFG)" == "np2 - Win32 Trace"

!ELSEIF  "$(CFG)" == "np2 - Win32 VGA test"

!ELSEIF  "$(CFG)" == "np2 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
