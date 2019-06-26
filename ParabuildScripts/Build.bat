REM -----------------------------------------------------------------------------
REM driver batch file for ALBA building
REM -----------------------------------------------------------------------------

IF "%1" == "ALBA_DEBUG" GOTO BUILD_ALBA_DEBUG
IF "%1" == "VAPP_DEBUG" GOTO BUILD_VAPP_DEBUG
IF "%1" == "VAPP_DEBUG_2010" GOTO BUILD_VAPP_DEBUG_2010
IF "%1" == "VAPP_RELEASE" GOTO BUILD_VAPP_RELEASE
IF "%1" == "ALBA_DEBUG_2005" GOTO BUILD_ALBA_DEBUG_2005
IF "%1" == "VAPP_DEBUG_2005" GOTO BUILD_VAPP_DEBUG_2005
IF "%1" == "VAPP_RELEASE_2005" GOTO BUILD_VAPP_RELEASE_2005
IF "%1" == "ALBA_DEBUG_2008" GOTO BUILD_ALBA_DEBUG_2008
IF "%1" == "VAPP_DEBUG_2008" GOTO BUILD_VAPP_DEBUG_2008
IF "%1" == "VAPP_RELEASE_2008" GOTO BUILD_VAPP_RELEASE_2008
IF "%1" == "ALBA_DEBUG_2010" GOTO BUILD_ALBA_DEBUG_2010
IF "%1" == "VAPP_RELEASE_2010" GOTO BUILD_VAPP_RELEASE_2010
IF NOT "%1" == "ALBA_DEBUG" GOTO UNKNOWN_CONDITION

:BUILD_ALBA_DEBUG
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
cd ALBA_ITK_Parabuild
devenv  ALBA.sln /project ALL_BUILD.vcxproj /build debug /out build_log.txt
cd ..
GOTO END

:BUILD_VAPP_RELEASE
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:BUILD_ALBA_DEBUG_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_RELEASE_2005
CALL "%PROGRAMFILES%/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:BUILD_ALBA_DEBUG_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_RELEASE_2008
CALL "%PROGRAMFILES%/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat"
devenv  ./ALBA_ITK_Parabuild/ALBA.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:BUILD_ALBA_DEBUG_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcxproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_RELEASE_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
cd ALBA_ITK_Parabuild
devenv  ALBA.sln /project ALL_BUILD.vcxproj /build release /out build_log.txt
cd ..
GOTO END

:UNKNOWN_CONDITION

:END
