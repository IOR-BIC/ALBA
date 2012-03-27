REM -----------------------------------------------------------------------------
REM Unit tests driver script
REM -----------------------------------------------------------------------------
IF "%1" == "MAFMED_DEBUG_2010" GOTO MAFMED_DEBUG_2010
IF NOT "%1" == "MAFMED_DEBUG_2010" GOTO DEFAULT

:MAFMED_DEBUG_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project RUN_TESTS.vcxproj /build debug /out build_log
GOTO END

:DEFAULT
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project RUN_TESTS.vcproj  /build debug /out build_log
GOTO END

:END 