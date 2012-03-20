REM -----------------------------------------------------------------------------
REM cmake execution driver script
REM -----------------------------------------------------------------------------

IF "%1" == "MAFMED_DEBUG" GOTO MAFMED_DEBUG
IF "%1" == "MAFMED_DOCUMENTATION" GOTO MAFMED_DOCUMENTATION
IF "%1" == "VAPP_DEBUG" GOTO VAPP_DEBUG
IF "%1" == "VAPP_RELEASE" GOTO VAPP_RELEASE
IF "%1" == "VAPP_RELEASE_2005" GOTO VAPP_RELEASE_2005
IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:MAFMED_DEBUG

cd Medical_Parabuild

cmake.exe  .  ^
  -G"Visual Studio 7 .NET 2003" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.0 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0/lib/cppunitd.lib" ^
  -DMAF_BINARY_PATH:PATH="%PARABUILD_CHECKOUT_DIR%/openMAF_ITK_Medical_Parabuild/openMAF_ITK_Parabuild" 

cd ..   

GOTO END

:MAFMED_DOCUMENTATION

cd Medical_Parabuild

cmake.exe  .  ^
  -G"Visual Studio 7 .NET 2003" ^
  -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.0 ^
  -DMAF_BINARY_PATH:PATH="%PARABUILD_CHECKOUT_DIR%/openMAF_ITK_Medical_Parabuild/openMAF_ITK_Parabuild" ^
  -DBUILD_DOCUMENTATION=ON

cd ..   

GOTO END

:VAPP_DEBUG

cd Medical_Parabuild

cmake.exe . ^
  -G"Visual Studio 7 .NET 2003" ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_TESTING:BOOL=OFF ^
  -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.0 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0/lib/cppunitd.lib" ^
  -DMAF_BINARY_PATH:PATH="%PARABUILD_CHECKOUT_DIR%/vapp/openMAF_ITK_Parabuild" 

  
cd ..   
   
GOTO END

:VAPP_RELEASE

cd Medical_Parabuild

cmake.exe     ^
  -G"Visual Studio 7 .NET 2003" ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_TESTING:BOOL=OFF ^
  -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.0 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0/lib/cppunitd.lib" ^
  -DMAF_BINARY_PATH:PATH="%PARABUILD_CHECKOUT_DIR%/vapp/openMAF_ITK_Parabuild" 
  
cd ..   

GOTO END

:VAPP_RELEASE_2005

cd Medical_Parabuild

cmake.exe     ^
  -G"Visual Studio 8 2005" ^
  -DBUILD_EXAMPLES:BOOL=OFF ^
  -DBUILD_TESTING:BOOL=OFF ^
  -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.0 ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.12.0/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.12.0/lib/cppunitd.lib" ^
  -DMAF_BINARY_PATH:PATH="%PARABUILD_CHECKOUT_DIR%/vapp/openMAF_ITK_Parabuild" 
  
cd ..   

GOTO END

:UNKNOWN_CONDITION

:END
