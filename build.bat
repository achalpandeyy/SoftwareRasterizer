@echo off

if not exist Build mkdir Build

set ExternalIncludePath=../External

REM TODO(achal): I'm not sure, should I disable incremental linking??
REM NOTE(achal): Silencing Warning C4201 may cause compilation issues in other compilers, not MSVC.
set CompilerFlags=-nologo -I%ExternalIncludePath% -FeSoftwareRenderer -Z7 -EHsc -W4 -WX -wd4201 -wd4100 -wd4189
set LinkerFlags=user32.lib gdi32.lib

pushd Build
cl %CompilerFlags% ../Source/WinMain.cpp ../Source/Engine.cpp %LinkerFlags% && del *.obj
SoftwareRenderer
popd

echo -------------------------------------------------------------------------------------------------------