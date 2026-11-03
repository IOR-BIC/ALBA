param()

. "$PSScriptRoot\common.ps1"

Add-MSBuildToPath
$testCmd = 'set CTEST_OUTPUT_ON_FAILURE=true && MSBuild RUN_TESTS.vcxproj /verbosity:minimal /p:Configuration=Release'

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name test -Command $testCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
