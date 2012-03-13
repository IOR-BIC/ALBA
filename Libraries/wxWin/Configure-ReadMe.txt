What Configure.cmake does
by Sil - 22-may2006

1)
  search and unpack
    - Packages/wxWidgets*.tar.gz
    - Packages/wxWidgets*Patch*.tar.gz
    - Packages/wxCmake*.tar.gz

2)
  if Build/Libraries/wxWin/Source/CMakeList.txt is found
    - run another session of CMake on it 
    - this will generate the CMakeCache.txt
    - from the CMakeCache.txt the variable WX_MAKECOMMAND will be loaded
      and used to configure Build.cmake.in  

3) Create the custom target BUILD_WX_LIBRARY
    - that will call, bacause of the Dependencies, the WX_CUSTOM_COMMAND
    - WX_CUSTOM_COMMAND will call Build.cmake
    - Build.cmake will execute the WXWIN_MAKECOMMAND
           