param()

$bldDir = Join-Path $PSScriptRoot '..\..\..\bld'
$testCmd = 'set CTEST_OUTPUT_ON_FAILURE=true && MSBuild RUN_TESTS.vcxproj /verbosity:minimal /p:Configuration=Release'

Push-Location $bldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name test -Command $testCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}