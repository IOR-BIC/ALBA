# -Configuration: solution configuration to build (default Release).
# -Target: optional project to build (with its dependencies) instead of the
#          whole solution, e.g. -Target albaOperations.
# -MaxCompilers: compiler processes running at once in the whole build
#          (default: the number of logical processors). Like the C++ build
#          limit of the Visual Studio IDE, it is shared by all the projects
#          built in parallel: without it each project used its own /MP and a
#          full build ran out of memory (C1060, LNK1102), leaving stale objects.
param(
    [ValidateSet('Release', 'Debug', 'RelWithDebInfo', 'MinSizeRel')][string]$Configuration = 'Release',
    [string]$Target = '',
    [ValidateRange(1, 256)][int]$MaxCompilers = [Environment]::ProcessorCount
)

. "$PSScriptRoot\common.ps1"

Add-MSBuildToPath
$buildCmd = "MSBuild ALBA.sln /m /p:UseMultiToolTask=true /p:EnforceProcessCountAcrossBuilds=true /p:MultiProcMaxCount=$MaxCompilers /verbosity:quiet /p:Configuration=$Configuration"
if ($Target)
    { $buildCmd += " /t:$Target" }

# Release keeps the historical log name, other configurations get their own.
$logName = 'build'
if ($Configuration -ne 'Release')
    { $logName = "build-$($Configuration.ToLower())" }

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name $logName -Command $buildCmd
    $exitCode = $LASTEXITCODE

    # Once the external libraries are built, stop rebuilding them on every
    # build (configure.ps1 -CompileLibraries turns it back on). Only a full
    # Release build turns it off, so that a Debug or single-target build
    # cannot leave the Release libraries unbuilt.
    if ($exitCode -eq 0 -and $Configuration -eq 'Release' -and -not $Target -and (Select-String -Path 'CMakeCache.txt' -Pattern '^COMPILE_LIBRARIES:BOOL=ON' -Quiet)) {
        Write-Output "Build succeeded: setting COMPILE_LIBRARIES=OFF"
        & "$PSScriptRoot\run-logged.ps1" -Name configure -Command 'cmake -DCOMPILE_LIBRARIES:BOOL=OFF .'
        $exitCode = $LASTEXITCODE
    }
    exit $exitCode
} finally {
    Pop-Location
}
