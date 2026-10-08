param(
    [Parameter(Mandatory = $true)][string]$Name,
    [Parameter(Mandatory = $true)][string]$Command,
    [int]$MaxErrorLines = 30,
    [int]$TailLines = 15
)

$logDir = Join-Path $PSScriptRoot '..\..\build-logs'
New-Item -ItemType Directory -Force -Path $logDir | Out-Null
$logDir = (Resolve-Path $logDir).Path

$outFile = Join-Path $logDir "$Name.stdout.tmp"
$errFile = Join-Path $logDir "$Name.stderr.tmp"
$logFile = Join-Path $logDir "$Name.log"

$process = Start-Process -FilePath 'cmd.exe' `
    -ArgumentList '/c', $Command `
    -NoNewWindow -Wait -PassThru `
    -RedirectStandardOutput $outFile `
    -RedirectStandardError $errFile

$exitCode = $process.ExitCode

Get-Content $outFile, $errFile -ErrorAction SilentlyContinue | Set-Content $logFile
Remove-Item $outFile, $errFile -ErrorAction SilentlyContinue

$errorPattern = 'error C\d+|error LNK\d+|fatal error|CMake Error|FAILED|Failures|Assertion|not ok'
$errorLines = Select-String -Path $logFile -Pattern $errorPattern |
    Select-Object -First $MaxErrorLines |
    ForEach-Object { $_.Line.Trim() }

Write-Output "STEP: $Name"
Write-Output "EXIT CODE: $exitCode"
Write-Output "FULL LOG: $logFile"

if ($errorLines) {
    Write-Output "--- FIRST ERRORS ---"
    $errorLines | ForEach-Object { Write-Output $_ }
}

Write-Output "--- LOG TAIL ---"
Get-Content $logFile -Tail $TailLines | ForEach-Object { Write-Output $_ }

exit $exitCode