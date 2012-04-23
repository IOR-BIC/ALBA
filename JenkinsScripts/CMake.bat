REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------


IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG
IF "%1" == "LIB_VS2010_RELEASE" GOTO LIB_VS2010_RELEASE

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source ^
     -G"Visual Studio 10" ^
     -DMAF_BINARY_PATH:PATH=d:\MAF2Libs\VS2010D\Build\ ^
 	 -DBUILD_TESTING:BOOL=ON ^
 	 -DDART_TESTING_TIMEOUT:STRING=120 ^
 	 -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
   	 -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
	 
cd ..

GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_RELEASE
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source ^
     -G"Visual Studio 10" ^
     -DMAF_BINARY_PATH:PATH=d:\MAF2Libs\VS2010R\Build\

cd ..

GOTO END

REM -----------------------------------------------------------------------------

:END
