# -CompileLibraries re-enables the build of the external libraries (VTK, GDCM,
# ...), which build.ps1 turns off after the first successful build.
param([switch]$CompileLibraries)

. "$PSScriptRoot\common.ps1"

New-Item -ItemType Directory -Force -Path $BldDir | Out-Null

$cmakeCmd = "cmake -G `"Visual Studio 17 2022`" -A x64 -D CMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=ON -DBUILD_BASE_TESTING:BOOL=ON -DBUILD_COMMON_TESTING:BOOL=ON -DBUILD_CORE_TESTING:BOOL=ON -DBUILD_DATAPIPES_TESTING:BOOL=ON -DBUILD_GUI_TESTING:BOOL=ON -DBUILD_INTERACTION_TESTING:BOOL=ON -DBUILD_IO_TESTING:BOOL=ON -DBUILD_OPERATIONS_TESTING:BOOL=ON -DBUILD_VIEWS_TESTING:BOOL=ON -DBUILD_VME_TESTING:BOOL=ON -DBUILD_VISUALPIPES_TESTING=ON -DBUILD_vtkALBA_TESTING:BOOL=ON -DDISABLE_GPU_VOLUME_RENDERING_TESTS=ON -DDART_TESTING_TIMEOUT:STRING=600 -DCPPUNIT_INCLUDE_DIR:PATH=C:\Alba\extras\cppunit-1.12.1-64bit\include -DCPPUNIT_DEBUG_LIBRARY:FILEPATH=C:\Alba\extras\cppunit-1.12.1-64bit\lib/cppunitd.lib -DCPPUNIT_RELEASE_LIBRARY:FILEPATH=C:\Alba\extras\cppunit-1.12.1-64bit\lib/cppunit.lib `"$SrcDir`""
if ($CompileLibraries)
    { $cmakeCmd += ' -DCOMPILE_LIBRARIES:BOOL=ON' }

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name configure -Command $cmakeCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
