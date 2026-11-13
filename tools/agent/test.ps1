# -Configuration: configuration of the test executables to run (default Release).
# -Suite: optional test suites to run instead of all of them. Accepts the CTest
#         name (albaVisualPipesTests) or the short one (VisualPipes, vtkALBA),
#         several can be given separated by commas.
param(
    [ValidateSet('Release', 'Debug', 'RelWithDebInfo', 'MinSizeRel')][string]$Configuration = 'Release',
    [string[]]$Suite = @()
)

. "$PSScriptRoot\common.ps1"

Add-MSBuildToPath

$logName = 'test'
if ($Suite.Count -eq 0)
{
    $testCmd = "set CTEST_OUTPUT_ON_FAILURE=true && MSBuild RUN_TESTS.vcxproj /verbosity:minimal /p:Configuration=$Configuration"
}
else
{
    # With -File a comma separated list arrives as a single string: split it here
    $names = foreach ($s in ($Suite -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ }))
    {
        $n = $s
        if ($n -notmatch 'Tests$')
            { $n += 'Tests' }
        if ($n -notmatch '^(alba|vtk)')
            { $n = 'alba' + $n }
        $n
    }
    $regex = '^(' + ($names -join '|') + ')$'
    $testCmd = "ctest -C $Configuration --output-on-failure --no-tests=error -R `"$regex`""
    $logName = 'test-' + (($names -join '-') -replace 'Tests', '')
}
if ($Configuration -ne 'Release')
    { $logName += "-$($Configuration.ToLower())" }

Push-Location $BldDir
try {
    & "$PSScriptRoot\run-logged.ps1" -Name $logName -Command $testCmd
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
