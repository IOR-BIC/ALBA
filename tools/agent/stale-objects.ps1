# Lists (and with -Delete removes) the object files of the ALBA projects that
# are older than their source or than a header the source includes, directly
# or through other headers. MSBuild should recompile them by itself, but a
# build that fails (e.g. out of memory: C1060, LNK1102) can leave objects that
# it never recompiles, and the programs then run with stale code. Deleting
# them makes the next build compile them again.
# -Configuration: configuration of the objects to check (default Release).
# -Delete: delete the stale objects.
# Only the git-tracked sources outside Libs/ are considered; an include is
# resolved by its file name, so headers with the same name all count.
param(
    [ValidateSet('Release', 'Debug', 'RelWithDebInfo', 'MinSizeRel')][string]$Configuration = 'Release',
    [switch]$Delete
)

. "$PSScriptRoot\common.ps1"

$srcExt = @('.h', '.hpp', '.hxx', '.txx', '.inl', '.c', '.cpp', '.cxx')
$libsDir = (Join-Path $SrcDir 'Libs') + '\'

# Tracked sources, by lower case file name (a name can be used by more files)
$byName = @{}
foreach ($rel in (& git -C $SrcDir ls-files)) {
    if ($rel.StartsWith('Libs/') -or $srcExt -notcontains [IO.Path]::GetExtension($rel).ToLower())
        { continue }
    $path = Join-Path $SrcDir ($rel -replace '/', '\')
    $name = [IO.Path]::GetFileName($path).ToLower()
    if (-not $byName.ContainsKey($name))
        { $byName[$name] = New-Object System.Collections.Generic.List[string] }
    $byName[$name].Add($path)
}

$includeRegex = [regex]'(?m)^\s*#\s*include\s*[<"]([^>"]+)[>"]'
$newest = @{}   # path -> newest write time of the file and of what it includes

function Get-NewestTime([string]$path) {
    if ($newest.ContainsKey($path))
        { return $newest[$path] }
    $time = [IO.File]::GetLastWriteTimeUtc($path)
    $newest[$path] = $time  # an include cycle stops here
    $text = [IO.File]::ReadAllText($path, [Text.Encoding]::GetEncoding(28591))
    foreach ($m in $includeRegex.Matches($text)) {
        $name = [IO.Path]::GetFileName($m.Groups[1].Value).ToLower()
        if (-not $byName.ContainsKey($name))
            { continue }
        foreach ($dep in $byName[$name]) {
            $depTime = Get-NewestTime $dep
            if ($depTime -gt $time)
                { $time = $depTime }
        }
    }
    $newest[$path] = $time
    return $time
}

# Objects of the sources of each project (CMake puts them in the IntDir of the configuration)
$intDirRegex = [regex]("<IntDir Condition=`"'\`$\(Configuration\)\|\`$\(Platform\)'=='" + $Configuration + "\|[^']*'`">([^<]+)</IntDir>")
$compileRegex = [regex]'<ClCompile Include="([^"]+)"'
$stale = @()
$checked = 0
foreach ($proj in Get-ChildItem -Path $BldDir -Recurse -Filter *.vcxproj -File) {
    if ($proj.FullName.StartsWith((Join-Path $BldDir 'Libs') + '\'))
        { continue }
    $xml = [IO.File]::ReadAllText($proj.FullName)
    $intDir = $intDirRegex.Match($xml)
    if (-not $intDir.Success)
        { continue }
    $objDir = Join-Path $proj.DirectoryName $intDir.Groups[1].Value
    foreach ($m in $compileRegex.Matches($xml)) {
        $source = $m.Groups[1].Value
        if (-not $source.StartsWith($SrcDir + '\') -or $source.StartsWith($libsDir) -or -not (Test-Path -LiteralPath $source))
            { continue }
        $obj = Join-Path $objDir ([IO.Path]::GetFileNameWithoutExtension($source) + '.obj')
        if (-not (Test-Path -LiteralPath $obj))
            { continue }
        $checked++
        if ([IO.File]::GetLastWriteTimeUtc($obj) -lt (Get-NewestTime $source))
            { $stale += $obj }
    }
}

Write-Output "Stale objects ($Configuration): $($stale.Count) of $checked"
foreach ($obj in $stale) {
    Write-Output ('  ' + $obj.Substring($BldDir.Length + 1))
    if ($Delete)
        { Remove-Item -LiteralPath $obj }
}
if ($Delete -and $stale.Count)
    { Write-Output "Deleted: the build compiles them again" }
exit 0
