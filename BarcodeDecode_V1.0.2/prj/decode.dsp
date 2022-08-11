# Microsoft Developer Studio Project File - Name="decode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=decode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "decode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "decode.mak" CFG="decode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "decode - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "decode - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "decode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "decode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "decode - Win32 Release"
# Name "decode - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\iconv\ascii.c
# End Source File
# Begin Source File

SOURCE=..\src\BarcodeFormat.c
# End Source File
# Begin Source File

SOURCE=..\src\BitArray.c
# End Source File
# Begin Source File

SOURCE=..\src\BitMatrix.c
# End Source File
# Begin Source File

SOURCE=..\src\BitSource.c
# End Source File
# Begin Source File

SOURCE=..\src\CharacterSetECI.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\CodaBarReader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code128Reader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code39Reader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code93Reader.c
# End Source File
# Begin Source File

SOURCE=..\src\DecodeHints.c
# End Source File
# Begin Source File

SOURCE=..\src\DetectorResult.c
# End Source File
# Begin Source File

SOURCE=..\src\DiyMalloc.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\EAN13Reader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\EAN8Reader.c
# End Source File
# Begin Source File

SOURCE=..\src\iconv\Euc_cn.c
# End Source File
# Begin Source File

SOURCE=..\src\iconv\gb2312.c
# End Source File
# Begin Source File

SOURCE=..\src\GenericGF.c
# End Source File
# Begin Source File

SOURCE=..\src\GenericGFPoly.c
# End Source File
# Begin Source File

SOURCE=..\src\GlobalHistogramBinarizer.c
# End Source File
# Begin Source File

SOURCE=..\src\GreyscaleLuminanceSource.c
# End Source File
# Begin Source File

SOURCE=..\src\GridSampler.c
# End Source File
# Begin Source File

SOURCE=..\src\HybridBinarizer.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\ITFReader.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=..\src\MathUtils.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\OneDReader.c
# End Source File
# Begin Source File

SOURCE=..\src\PerspectiveTransform.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRAlignmentPattern.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRAlignmentPatternFinder.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRBitMatrixParser.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRCodeReader.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDataBlock.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDataMask.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDecode.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDecodedBitStreamParser.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDetector.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRErrorCorrectLevel.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPattern.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPatternFinder.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPatternInfo.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFormatInfo.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRMode.c
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRVersion.c
# End Source File
# Begin Source File

SOURCE=..\src\ReedSolomonDecoder.c
# End Source File
# Begin Source File

SOURCE=..\src\ResultPoint.c
# End Source File
# Begin Source File

SOURCE=..\src\StringUtils.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCAReader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCEANReader.c
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCEReader.c
# End Source File
# Begin Source File

SOURCE=..\src\iconv\utfswap.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\BarcodeFormat.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseTypeDef.h
# End Source File
# Begin Source File

SOURCE=..\src\BitArray.h
# End Source File
# Begin Source File

SOURCE=..\src\BitMatrix.h
# End Source File
# Begin Source File

SOURCE=..\src\BitSource.h
# End Source File
# Begin Source File

SOURCE=..\src\CharacterSetECI.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\CodaBarReader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code128Reader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code39Reader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\Code93Reader.h
# End Source File
# Begin Source File

SOURCE=..\src\DecodeHints.h
# End Source File
# Begin Source File

SOURCE=..\src\DetectorResult.h
# End Source File
# Begin Source File

SOURCE=..\src\DiyMalloc.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\EAN13Reader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\EAN8Reader.h
# End Source File
# Begin Source File

SOURCE=..\src\iconv\gb2312.h
# End Source File
# Begin Source File

SOURCE=..\src\GenericGF.h
# End Source File
# Begin Source File

SOURCE=..\src\GenericGFPoly.h
# End Source File
# Begin Source File

SOURCE=..\src\GlobalHistogramBinarizer.h
# End Source File
# Begin Source File

SOURCE=..\src\GreyscaleLuminanceSource.h
# End Source File
# Begin Source File

SOURCE=..\src\GridSampler.h
# End Source File
# Begin Source File

SOURCE=..\src\HybridBinarizer.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\ITFReader.h
# End Source File
# Begin Source File

SOURCE=..\src\MathUtils.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\OneDReader.h
# End Source File
# Begin Source File

SOURCE=..\src\PerspectiveTransform.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRAlignmentPattern.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRAlignmentPatternFinder.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRBitMatrixParser.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRCodeReader.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDataBlock.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDataMask.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDecode.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDecodedBitStreamParser.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRDetector.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRErrorCorrectLevel.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPattern.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPatternFinder.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFinderPatternInfo.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRFormatInfo.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRMode.h
# End Source File
# Begin Source File

SOURCE=..\src\QR\QRVersion.h
# End Source File
# Begin Source File

SOURCE=..\src\ReedSolomonDecoder.h
# End Source File
# Begin Source File

SOURCE=..\src\ResultPoint.h
# End Source File
# Begin Source File

SOURCE=..\src\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCAReader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCEANReader.h
# End Source File
# Begin Source File

SOURCE=..\src\oneD\UPCEReader.h
# End Source File
# Begin Source File

SOURCE=..\src\iconv\utfswap.h
# End Source File
# Begin Source File

SOURCE=..\src\iconv\wcmbswap.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
