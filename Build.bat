cmake .  ^
  -G"Visual Studio 7 .NET 2003" ^
  -DBUILD_TESTING:BOOL=ON ^
  -DCPPUNIT_INCLUDE_DIR:PATH="C:/cppunit-1.11.6/include" ^
  -DCPPUNIT_LIBRARY:FILEPATH="C:/cppunit-1.11.6/lib/cppunitd.lib"
   
REM perform building
devenv MAF.sln /build debug /out build_log.txt