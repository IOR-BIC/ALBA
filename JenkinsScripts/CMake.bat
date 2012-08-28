REM -----------------------------------------------------------------------------
REM CMake scripts 
REM -----------------------------------------------------------------------------

IF "%1" == "LIB_VS2003_RELEASE" GOTO LIB_VS2003_RELEASE
IF "%1" == "LIB_VS2010_DEBUG" GOTO LIB_VS2010_DEBUG
IF "%1" == "LIB_VS2010_RELEASE" GOTO LIB_VS2010_RELEASE
IF "%1" == "DLL_VS2010_DEBUG" GOTO DLL_VS2010_DEBUG
IF "%1" == "DLL_VS2010_RELEASE" GOTO DLL_VS2010_RELEASE
IF "%1" == "DOXYGEN_DOCUMENTATION" GOTO DOXYGEN_DOCUMENTATION
IF "%1" == "LIB_VS2010_DEBUG_BASE_TEST" GOTO LIB_VS2010_DEBUG_BASE_TEST
IF "%1" == "LIB_VS2010_DEBUG_CORE_TEST" GOTO LIB_VS2010_DEBUG_CORE_TEST
IF "%1" == "LIB_VS2010_DEBUG_OPERATIONS_TEST" GOTO LIB_VS2010_DEBUG_OPERATIONS_TEST
IF "%1" == "LIB_VS2010_DEBUG_VME_TEST" GOTO LIB_VS2010_DEBUG_VME_TEST
IF "%1" == "LIB_VS2010_DEBUG_INTERACTION_TEST" GOTO LIB_VS2010_DEBUG_INTERACTION_TEST
IF "%1" == "LIB_VS2010_DEBUG_DATAPIPES_TEST" GOTO LIB_VS2010_DEBUG_DATAPIPES_TEST
IF "%1" == "LIB_VS2010_DEBUG_VTKMAF_TEST" GOTO LIB_VS2010_DEBUG_VTKMAF_TEST

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_BASE_TESTING:BOOL=ON ^
  -DBUILD_CORE_TESTING:BOOL=ON ^
  -DBUILD_OPERATIONS_TESTING:BOOL=ON ^
  -DBUILD_VME_TESTING:BOOL=ON ^
  -DBUILD_INTERACTION_TESTING:BOOL=ON ^
  -DBUILD_DATAPIPES_TESTING:BOOL=ON ^
  -DBUILD_vtkMAF_TESTING:BOOL=ON ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_VTKMAF_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=ON ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_DATAPIPES_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=ON ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_INTERACTION_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=ON ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_BASE_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=ON ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_CORE_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=ON ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_OPERATIONS_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=ON ^
  -DBUILD_VME_TESTING:BOOL=OFF ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
  -DDART_TESTING_TIMEOUT:STRING=120 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0_VS2010_BUILD/cppunit-1.12.0/lib/cppunitd.lib"
  
cd ..
  
GOTO END

REM -----------------------------------------------------------------------------
:LIB_VS2010_DEBUG_VME_TEST
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 10" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_BASE_TESTING:BOOL=OFF ^
  -DBUILD_CORE_TESTING:BOOL=OFF ^
  -DBUILD_OPERATIONS_TESTING:BOOL=OFF ^
  -DBUILD_VME_TESTING:BOOL=ON ^
  -DBUILD_INTERACTION_TESTING:BOOL=OFF ^
  -DBUILD_DATAPIPES_TESTING:BOOL=OFF ^
  -DBUILD_vtkMAF_TESTING:BOOL=OFF ^
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
:LIB_VS2003_RELEASE
REM -----------------------------------------------------------------------------

mkdir Build
cd Build

cmake.exe  ../Source  -G"Visual Studio 7 .NET 2003" ^
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
  -DBUILD_BASE_TESTING:BOOL=ON ^
  -DBUILD_CORE_TESTING:BOOL=ON ^
  -DBUILD_OPERATIONS_TESTING:BOOL=ON ^
  -DBUILD_VME_TESTING:BOOL=ON ^
  -DBUILD_INTERACTION_TESTING:BOOL=ON ^
  -DBUILD_DATAPIPES_TESTING:BOOL=ON ^
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


