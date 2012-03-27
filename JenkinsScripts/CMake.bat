REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------


IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG

IF NOT "%1" == "LIB_VS2010_DEBUG" GOTO UNKNOWN_CONDITION

:LIB_VS2010_DEBUG

cd ..

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10"

cd ../Source
  
GOTO END

:UNKNOWN_CONDITION

:END
