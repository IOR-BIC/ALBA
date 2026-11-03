param()

. "$PSScriptRoot\common.ps1"

Add-MSBuildToPath
$buildCmd = 'MSBuild ALBA.sln /m /verbosity:quiet /p:Configuration=Release'

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name build -Command $buildCmd
    $exitCode = $LASTEXITCODE

    # Once the external libraries are built, stop rebuilding them on every
    # build (configure.ps1 -CompileLibraries turns it back on).
    if ($exitCode -eq 0 -and (Select-String -Path 'CMakeCache.txt' -Pattern '^COMPILE_LIBRARIES:BOOL=ON' -Quiet)) {
        Write-Output "Build succeeded: setting COMPILE_LIBRARIES=OFF"
        & "$PSScriptRoot\run-logged.ps1" -Name configure -Command 'cmake -DCOMPILE_LIBRARIES:BOOL=OFF .'
        $exitCode = $LASTEXITCODE
    }
    exit $exitCode
} finally {
    Pop-Location
}
