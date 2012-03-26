REM -----------------------------------------------------------------------------
REM Build scripts
REM -----------------------------------------------------------------------------

IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG

:LIB_VS2010_DEBUG

cd ../MAF2.Build

CALL "%PROGRAMFILES%/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat"
devenv MAF.sln /project ALL_BUILD.vcxproj /build debug /out build_log.txt

cd ../MAF2.Source
  
GOTO END

:END

