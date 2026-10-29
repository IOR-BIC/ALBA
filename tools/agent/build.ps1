param()

. "$PSScriptRoot\common.ps1"

Add-MSBuildToPath
$buildCmd = 'MSBuild ALBA.sln /m /verbosity:quiet /p:Configuration=Release'

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name build -Command $buildCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
