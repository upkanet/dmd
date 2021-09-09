echo BUILD use Visual Studio 2019 MS Build
msbuild .\axodmd.vcxproj /property:Configuration=Release
MOVE Release\axodmd.exe axodmd.exe