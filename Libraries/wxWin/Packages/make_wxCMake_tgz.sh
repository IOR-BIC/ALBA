 mkdir CMake
 mkdir CMake/include
 mkdir CMake/include/wx
 mkdir CMake/include/wx/msw
 mkdir CMake/src
 mkdir CMake/src/expat
 mkdir CMake/src/jpeg
 mkdir CMake/src/png
 mkdir CMake/src/regex
 mkdir CMake/src/tiff
 mkdir CMake/src/zlib
 mkdir CMake/src/projects
 mkdir CMake/src/projects/adv
 mkdir CMake/src/projects/base
 mkdir CMake/src/projects/core
 mkdir CMake/src/projects/dbgrid
 mkdir CMake/src/projects/gl
 mkdir CMake/src/projects/html
 mkdir CMake/src/projects/media
 mkdir CMake/src/projects/net
 mkdir CMake/src/projects/odbc
 mkdir CMake/src/projects/qa
 mkdir CMake/src/projects/xml
 mkdir CMake/src/projects/xrc
 mkdir CMake/samples
 mkdir CMake/samples/minimal

cp ../../../../Build/Libraries/wxWin/Sources/CMakeLists.txt 	 CMake
cp ../../../../Build/Libraries/wxWin/Sources/CMakeListsUnix.txt 	 CMake
cp ../../../../Build/Libraries/wxWin/Sources/CMakeListsWin32.txt 	 CMake
cp ../../../../Build/Libraries/wxWin/Sources/wxFiles.cmake           CMake
cp ../../../../Build/Libraries/wxWin/Sources/CMakeConfigFile.in      CMake
cp ../../../../Build/Libraries/wxWin/Sources/CMakeUseFile.in         CMake
cp ../../../../Build/Libraries/wxWin/Sources/configure.in            CMake 
cp ../../../../Build/Libraries/wxWin/Sources/include/wx/msw/setup.in CMake/include/wx/msw/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/CMakeLists.txt 	 CMake/src/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/expat/CMakeLists.txt 	 CMake/src/expat/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/jpeg/CMakeLists.txt 	 CMake/src/jpeg/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/png/CMakeLists.txt 	 CMake/src/png/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/CMakeLists.txt 	 CMake/src/projects/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/adv/CMakeLists.txt 	 CMake/src/projects/adv/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/base/CMakeLists.txt 	 CMake/src/projects/base/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/core/CMakeLists.txt 	 CMake/src/projects/core/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/dbgrid/CMakeLists.txt 	 CMake/src/projects/dbgrid/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/gl/CMakeLists.txt 	 CMake/src/projects/gl/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/html/CMakeLists.txt 	 CMake/src/projects/html/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/media/CMakeLists.txt 	 CMake/src/projects/media/ 
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/net/CMakeLists.txt 	 CMake/src/projects/net/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/odbc/CMakeLists.txt 	 CMake/src/projects/odbc/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/qa/CMakeLists.txt 	 CMake/src/projects/qa/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/xml/CMakeLists.txt 	 CMake/src/projects/xml/
cp ../../../../Build/Libraries/wxWin/Sources/src/projects/xrc/CMakeLists.txt 	 CMake/src/projects/xrc/
cp ../../../../Build/Libraries/wxWin/Sources/src/regex/CMakeLists.txt 	 CMake/src/regex/
cp ../../../../Build/Libraries/wxWin/Sources/src/tiff/CMakeLists.txt 	 CMake/src/tiff/
cp ../../../../Build/Libraries/wxWin/Sources/src/zlib/CMakeLists.txt 	 CMake/src/zlib/
cp ../../../../Build/Libraries/wxWin/Sources/samples/CMakeLists.txt 	 CMake/samples/ 
cp ../../../../Build/Libraries/wxWin/Sources/samples/minimal/CMakeLists.txt 	 CMake/samples/minimal 

chmod -R a+rwx CMake

cd CMake

tar  -cvf ../wxCMake.tar *

cd ..

gzip wxCMake.tar




