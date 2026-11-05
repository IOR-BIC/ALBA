# Shared paths and tool lookup for the agent scripts. Dot-source it.

# Source tree is the repository root (e.g. C:\Alba\ALBA); build tree is the
# sibling folder with a "_B" suffix (e.g. C:\Alba\ALBA_B).
$SrcDir = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$BldDir = Join-Path (Split-Path $SrcDir -Parent) ((Split-Path $SrcDir -Leaf) + '_B')

# MSBuild is not on PATH unless VsDevCmd.bat was called first: in that case
# use it as is, otherwise locate it with vswhere (standard VS installer path)
# and prepend its folder to PATH, so that cmd.exe child processes find it.
function Add-MSBuildToPath {
    if (Get-Command MSBuild.exe -ErrorAction SilentlyContinue)
        { return }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere))
        { throw "vswhere.exe not found: is Visual Studio installed?" }
    $path = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
    if (-not $path)
        { throw "MSBuild.exe not found by vswhere" }
    $env:PATH = (Split-Path $path -Parent) + ';' + $env:PATH
}
