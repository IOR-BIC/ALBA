REM -----------------------------------------------------------------------------
REM Test scripts 
REM -----------------------------------------------------------------------------

IF "%1" == "TEST_VS2010_DEBUG" GOTO TEST_VS2010_DEBUG

REM -----------------------------------------------------------------------------
:TEST_VS2010_DEBUG
REM -----------------------------------------------------------------------------

cd Build

CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project RUN_TESTS.vcproj  /build debug /out build_log

cd ..

REM -----------------------------------------------------------------------------
