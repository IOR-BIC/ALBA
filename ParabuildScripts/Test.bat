REM -----------------------------------------------------------------------------
REM Unit tests driver script
REM -----------------------------------------------------------------------------
IF "%1" == "ALBA_MED_TEST" GOTO ALBA_MED_TEST
IF "%1" == "ALBA_TEST_2010" GOTO ALBA_TEST_2010
IF NOT "%1" == "ALBA_MED_TEST" GOTO DEFAULT

:ALBA_MED_TEST
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./ALBA_ITK_Parabuild/ALBA.sln  /project RUN_TESTS.vcproj  /build debug /out build_log
GOTO END

:ALBA_TEST_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ALBA.sln  /project RUN_TESTS.vcxproj  /build debug /out build_log
GOTO END
 
:DEFAULT
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ALBA.sln  /project RUN_TESTS.vcproj  /build debug /out build_log
GOTO END

:END