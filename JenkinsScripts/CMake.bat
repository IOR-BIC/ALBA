REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------


IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG

IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:LIB_VS2010_DEBUG

cd ..

mkdir MAF2.Build
cd MAF2.Build

cmake.exe  ../MAF2.Source  -G"Visual Studio 10"

cd ../MAF2.Source
  
GOTO END


:UNKNOWN_CONDITION

:END
