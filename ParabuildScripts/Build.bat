REM -----------------------------------------------------------------------------
REM driver batch file for MAF building
REM -----------------------------------------------------------------------------

IF "%1" == "MAFMED_DEBUG" GOTO MAFMED_DEBUG
IF "%1" == "VAPP_DEBUG" GOTO VAPP_DEBUG
IF "%1" == "VAPP_RELEASE" GOTO VAPP_RELEASE
IF "%1" == "MAFMED_DEBUG_2005" GOTO MAFMED_DEBUG_2005
IF "%1" == "VAPP_DEBUG_2005" GOTO VAPP_DEBUG_2005
IF "%1" == "VAPP_RELEASE_2005" GOTO VAPP_RELEASE_2005
IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:MAFMED_DEBUG
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_DEBUG
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_RELEASE
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:MAFMED_DEBUG_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_DEBUG_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build debug /out build_log.txt
GOTO END

:VAPP_RELEASE_2005
CALL "C:/Programmi/Microsoft Visual Studio 8/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project ALL_BUILD.vcproj /build release /out build_log.txt
GOTO END

:UNKNOWN_CONDITION

:END
