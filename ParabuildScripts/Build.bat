REM -----------------------------------------------------------------------------
REM driver batch file for MAF building
REM -----------------------------------------------------------------------------

IF "%1" == "MAF_DEBUG" GOTO BUILD_MAF_DEBUG
IF "%1" == "VAPP_DEBUG" GOTO BUILD_VAPP_DEBUG
IF "%1" == "VAPP_RELEASE" GOTO BUILD_VAPP_RELEASE
IF "%1" == "MAF_DEBUG_2005" GOTO BUILD_MAF_DEBUG_2005
IF "%1" == "VAPP_DEBUG_2005" GOTO BUILD_VAPP_DEBUG_2005
IF "%1" == "VAPP_RELEASE_2005" GOTO BUILD_VAPP_RELEASE_2005
IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:BUILD_MAF_DEBUG
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv MAF.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv  ./openMAF_ITK_Parabuild/MAF.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_RELEASE
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv  ./openMAF_ITK_Parabuild/MAF.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:BUILD_MAF_DEBUG_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv MAF.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_DEBUG_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv  ./openMAF_ITK_Parabuild/MAF.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:BUILD_VAPP_RELEASE_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv  ./openMAF_ITK_Parabuild/MAF.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:UNKNOWN_CONDITION

:END
