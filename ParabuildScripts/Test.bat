REM -----------------------------------------------------------------------------
REM Unit tests driver script
REM -----------------------------------------------------------------------------
IF "%1" == "MAF_MED_TEST" GOTO MAF_MED_TEST
IF "%1" == "MAF_TEST_2010" GOTO MAF_TEST_2010
IF NOT "%1" == "MAF_MED_TEST" GOTO DEFAULT

:MAF_MED_TEST
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./openMAF_ITK_Parabuild/MAF.sln  /project RUN_TESTS.vcproj  /build debug /out build_log
GOTO END

:MAF_TEST_2010
CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv MAF.sln  /project RUN_TESTS.vcxproj  /build debug /out build_log
GOTO END
 
:DEFAULT
CALL "%PROGRAMFILES%/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv MAF.sln  /project RUN_TESTS.vcproj  /build debug /out build_log
GOTO END

:END