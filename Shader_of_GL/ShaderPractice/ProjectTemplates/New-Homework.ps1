<#
    새 실습 과제 프로젝트를 생성합니다.

    사용법
      .\New-Homework.ps1 -Name homework11
      .\New-Homework.ps1 -Name homework11 -Title "OpenGL Practice 11" -Build

    생성되는 것
      ShaderPractice\homework11\homework11.vcxproj   ( GL.props 자동 import )
      ShaderPractice\homework11\homework11.vcxproj.filters
      ShaderPractice\homework11\homework11.cpp
      ShaderPractice.slnx 에 자동 등록
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Name,

    [string]$Title,

    [switch]$Build
)

$ErrorActionPreference = 'Stop'

$scriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$practiceDir = Split-Path -Parent $scriptDir
$templateDir = Join-Path $scriptDir 'OpenGL33Shader'

if (-not $Title) {
    $Title = $Name
}

if ($Name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$') {
    throw "이름에는 영문, 숫자, _ 만 쓸 수 있습니다: $Name"
}

$targetDir = Join-Path $practiceDir $Name

if (Test-Path $targetDir) {
    throw "이미 존재하는 폴더입니다: $targetDir"
}

#------------------------------------------------------------------------------------------
# 1. 템플릿 복사
#------------------------------------------------------------------------------------------

Copy-Item -LiteralPath $templateDir -Destination $targetDir -Recurse

Get-ChildItem -LiteralPath $targetDir -File | Where-Object {
    $_.Name -like 'OpenGL33Shader.*'
} | Remove-Item -Force

Rename-Item -LiteralPath (Join-Path $targetDir 'main.cpp') -NewName "$Name.cpp"

#------------------------------------------------------------------------------------------
# 2. vcxproj / filters 생성 ( placeholders 치환 )
#------------------------------------------------------------------------------------------

$guid = [guid]::NewGuid().ToString().ToUpper()

$vcxproj = @"
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{$guid}</ProjectGuid>
    <RootNamespace>$Name</RootNamespace>
    <WindowsTargetPlatformVersion>10.0.26100.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="`$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="`$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'`$(Configuration)|`$(Platform)'=='Debug|x64'">
    <Import Project="`$(UserRootDir)\Microsoft.Cpp.`$(Platform).user.props" Condition="exists('`$(UserRootDir)\Microsoft.Cpp.`$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="..\GL.props" Label="SharedProps" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'`$(Configuration)|`$(Platform)'=='Release|x64'">
    <Import Project="`$(UserRootDir)\Microsoft.Cpp.`$(Platform).user.props" Condition="exists('`$(UserRootDir)\Microsoft.Cpp.`$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="..\GL.props" Label="SharedProps" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <ItemDefinitionGroup Condition="'`$(Configuration)|`$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'`$(Configuration)|`$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="$Name.cpp" />
  </ItemGroup>
  <Import Project="`$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>
"@

$filters = @"
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>
      <Extensions>cpp;c;cc;cxx</Extensions>
    </Filter>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="$Name.cpp">
      <Filter>Source Files</Filter>
    </ClCompile>
  </ItemGroup>
</Project>
"@

$enc = New-Object System.Text.UTF8Encoding($false)

[System.IO.File]::WriteAllText((Join-Path $targetDir "$Name.vcxproj"), $vcxproj, $enc)
[System.IO.File]::WriteAllText((Join-Path $targetDir "$Name.vcxproj.filters"), $filters, $enc)

#------------------------------------------------------------------------------------------
# 3. .cpp 의 창 제목 치환
#------------------------------------------------------------------------------------------

$cppPath = Join-Path $targetDir "$Name.cpp"
[System.IO.File]::WriteAllText(
    $cppPath,
    ([System.IO.File]::ReadAllText($cppPath)).Replace('__WINDOWTITLE__', $Title),
    $enc
)

#------------------------------------------------------------------------------------------
# 4. .slnx 에 등록
#------------------------------------------------------------------------------------------

$slnxPath = Join-Path $practiceDir 'ShaderPractice.slnx'
$slnx = [System.IO.File]::ReadAllText($slnxPath)

if ($slnx -match [regex]::Escape("$Name/$Name.vcxproj")) {
    Write-Host "이미 slnx 에 등록되어 있습니다: $Name"
}
else {
    $entry = "  <Project Path=`"$Name/$Name.vcxproj`" Id=`"$guid`" />`r`n"
    $slnx = $slnx.Replace("</Solution>", "$entry</Solution>")
    [System.IO.File]::WriteAllText($slnxPath, $slnx, $enc)
    Write-Host "slnx 등록 완료: $Name"
}

Write-Host ""
Write-Host "생성 완료: $targetDir"
Write-Host "  링커 / C++20 / /utf-8 은 GL.props 에서 자동으로 적용됩니다."
Write-Host "  빌드: x64 만 ( glew32.lib, glfw3dll.lib 이 SDK 에 x64 로만 존재 )"
Write-Host ""

#------------------------------------------------------------------------------------------
# 5. 선택적 빌드
#------------------------------------------------------------------------------------------

if ($Build) {
    $msbuild = 'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe'

    if (-not (Test-Path $msbuild)) {
        throw "MSBuild 를 찾지 못했습니다: $msbuild"
    }

    & $msbuild (Join-Path $targetDir "$Name.vcxproj") /p:Configuration=Debug /p:Platform=x64 /v:m /t:Rebuild

    if ($LASTEXITCODE -ne 0) {
        throw "빌드 실패 ( exit=$LASTEXITCODE )"
    }

    Write-Host "빌드 성공"
}
