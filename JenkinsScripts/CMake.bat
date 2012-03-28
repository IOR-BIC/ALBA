REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------


IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG

IF NOT "%1" == "LIB_VS2010_DEBUG" GOTO UNKNOWN_CONDITION

:LIB_VS2010_DEBUG

mkdir Build
cd Build

cmake.exe  ../Source ^
     -G"Visual Studio 10" ^
     -DMAF_BINARY_PATH:PATH=d:\MAF2Libs\VS2010D\Build\

cd ..

GOTO END

:UNKNOWN_CONDITION

:END
