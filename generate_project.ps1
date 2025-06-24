# Check out https://premake.github.io/docs/Using-Premake for more builds
param (
	[string]$toolset = "gmake",
	[string]$target_abi = $null,
	[string]$device
)

# Let the environment override a toolset, if there is an environment variable set.
if ($env:toolset)
{
	$toolset = $env:toolset
}

$parameters = "--file=.\Builder\Project_Builder.lua"

if ($device)
{
	$parameters = "$parameters --target-device=$device"
}
if ($target_abi)
{
	$parameters = "$parameters --target-abi=$target_abi"
}

# Append the toolset.
$parameters = "$parameters $toolset"
$executable = ".\Builder\utils\premake5.exe"

"Building windows project..."
$process = Start-Process $executable $parameters -Wait -NoNewWindow -PassThru
if ($process.ExitCode -ne 0) { throw "Project generation error " + $process.ExitCode }

# Code for also making a cmake project, but turns out the converter is outdated...
# ""
# "Checking for cmake_converter (https://cmakeconverter.readthedocs.io)..."
# $process = Start-Process "pip" "install --pre cmake_converter" -Wait -PassThru
# if ($process.ExitCode -ne 0) {
# 	"ERROR: Something went wrong or python/pip is missing."
# 	throw "Project generation error " + $process.ExitCode
# }

# $python_pack_txt_path = ".\Build\Tmp\python_library_path.txt";

# New-Item -ItemType "Directory" -Path ".\Build\Tmp" -Force | Out-Null
# New-Item -ItemType "File" -Path $python_pack_txt_path -Force | Out-Null

# $process = Start-Process "python" "-m site --user-site" -Wait -PassThru -NoNewWindow -RedirectStandardOutput $python_pack_txt_path

# if ($process.ExitCode -ne 0) {
# 	throw "Project generation error " + $process.ExitCode
# }
# $cmake_converter_command = Get-Content $python_pack_txt_path
# $cmake_converter_command = $cmake_converter_command + "\cmake_converter\main.py" + " -s .\Skape_Playground.sln"

# Remove-Item $python_pack_txt_path

# "Converting to CMake"

# $cmake_converter_command
# $process = Start-Process "python" $cmake_converter_command -Wait -PassThru
# if ($process.ExitCode -ne 0) { throw "Project generation error " + $process.ExitCode }
