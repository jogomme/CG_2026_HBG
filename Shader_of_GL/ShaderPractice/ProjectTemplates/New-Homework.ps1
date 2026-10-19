<# Creates an assignment from the existing Visual Studio template; shares Common sources. #>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$Name,
    [string]$Title,
    [switch]$Build
)
$ErrorActionPreference = 'Stop'
if ($Name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$') { throw 'Use letters, digits and underscores only.' }
$practiceDir = Split-Path -Parent $PSScriptRoot
$templateDir = Join-Path $PSScriptRoot 'VS\OpenGL33Shader'
$targetDir = Join-Path $practiceDir $Name
if (Test-Path -LiteralPath $targetDir) { throw "Directory already exists: $targetDir" }
if (-not $Title) { $Title = $Name }
$enc = [Text.UTF8Encoding]::new($false)
$guid = '{' + [guid]::NewGuid().ToString().ToUpper() + '}'
# Read and validate all inputs before creating the project.
$project = [IO.File]::ReadAllText((Join-Path $templateDir 'OpenGL33Shader.vcxproj')).Replace('$safeprojectname$', $Name).Replace('$guid1$', $guid)
$filters = [IO.File]::ReadAllText((Join-Path $templateDir 'OpenGL33Shader.vcxproj.filters')).Replace('$safeprojectname$', $Name)
$escapedTitle = $Title.Replace('\', '\\').Replace('"', '\"').Replace("`r", '\r').Replace("`n", '\n')
$source = [IO.File]::ReadAllText((Join-Path $templateDir 'main.cpp')).Replace('__WINDOWTITLE__', $escapedTitle)
$slnxPath = Join-Path $practiceDir 'ShaderPractice.slnx'
[xml]$solution = [IO.File]::ReadAllText($slnxPath)
if ($solution.SelectSingleNode("/Solution/Project[@Path='$Name/$Name.vcxproj']")) { throw 'Project already registered in solution.' }
$entry = $solution.CreateElement('Project')
$entry.SetAttribute('Path', "$Name/$Name.vcxproj")
$mapping = $solution.CreateElement('Platform')
$mapping.SetAttribute('Project', 'x64')
$null = $entry.AppendChild($mapping)
$null = $solution.DocumentElement.AppendChild($entry)
$null = New-Item -ItemType Directory -Path $targetDir
[IO.File]::WriteAllText((Join-Path $targetDir "$Name.vcxproj"), $project, $enc)
[IO.File]::WriteAllText((Join-Path $targetDir "$Name.vcxproj.filters"), $filters, $enc)
[IO.File]::WriteAllText((Join-Path $targetDir "$Name.cpp"), $source, $enc)
$solution.Save($slnxPath)
Write-Host "Created $targetDir (Common sources are referenced, not copied)."
if ($Build) {
    $msbuild = 'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe'
    if (-not (Test-Path -LiteralPath $msbuild)) { throw "MSBuild not found: $msbuild" }
    & $msbuild (Join-Path $targetDir "$Name.vcxproj") /p:Configuration=Debug /p:Platform=x64 /v:minimal
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $LASTEXITCODE" }
}
