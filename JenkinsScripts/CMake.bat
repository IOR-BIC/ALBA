REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------

IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG
IF "%1" == "LIB_VS2010_RELEASE" GOTO LIB_VS2010_RELEASE
IF "%1" == "DLL_VS2010_DEBUG" GOTO DLL_VS2010_DEBUG
IF "%1" == "DLL_VS2010_RELEASE" GOTO DLL_VS2010_RELEASE
IF "%1" == "DOXYGEN_DOCUMENTATION" GOTO DOXYGEN_DOCUMENTATION

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
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

cmake.exe  ../Source  -G"Visual Studio 10" ^
	-DBUILD_TESTING:BOOL=OFF

cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:DLL_VS2010_DEBUG
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DMAF_BUILD_MAFDLL:BOOL=ON ^
  -DBUILD_TESTING:BOOL=ON ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DBUILD_EXAMPLES:BOOL=ON ^
  -DMAF_USE_ITK:BOOL=ON ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..

GOTO END

REM -----------------------------------------------------------------------------
:DLL_VS2010_RELEASE
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DMAF_BUILD_MAFDLL:BOOL=ON ^
  -DBUILD_TESTING:BOOL=OFF

cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:DOXYGEN_DOCUMENTATION
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_DOCUMENTATION:BOOL=ON ^
  -DBUILD_TESTING:BOOL=OFF ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DDOXYGEN_DOT_EXECUTABLE="D:/Graphviz 2.28/bin/dot.exe" ^
  -DDOXYGEN_DOT_PATH="D:/Graphviz 2.28/bin" ^
  -DDOXYGEN_EXECUTABLE="D:/doxygen/bin/doxygen.exe"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:END
REM -----------------------------------------------------------------------------


