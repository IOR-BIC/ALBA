REM -----------------------------------------------------------------------------
REM Build scripts
REM -----------------------------------------------------------------------------

IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG
IF "%1" == "LIB_VS2010_RELEASE" GOTO LIB_VS2010_RELEASE

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG
REM -----------------------------------------------------------------------------

cd Build

CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcxproj /build debug /out build_log.txt

cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_RELEASE
REM -----------------------------------------------------------------------------

cd Build

CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv ALBA.sln /project ALL_BUILD.vcxproj /build release /out build_log.txt

cd ..
  
GOTO END

REM -----------------------------------------------------------------------------


:END
