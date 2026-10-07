param()

$bldDir = Join-Path $PSScriptRoot '..\..\..\bld'
$buildCmd = 'MSBuild ALBA.sln /verbosity:quiet /p:Configuration=Release'

Push-Location $bldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name build -Command $buildCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}