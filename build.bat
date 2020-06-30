@echo off

if not exist Build mkdir Build

set ExternalIncludePath=../External
set ExternalSource=../External/stb_image/stb_image.cpp

REM TODO(achal): I'm not sure, should I disable incremental linking??
REM NOTE(achal): Silencing Warning C4201 may cause compilation issues in other compilers, not MSVC.
set CompilerFlags=-nologo -I%ExternalIncludePath% -FeSoftwareRenderer -Z7 -EHsc -W4 -WX -Oi -wd4201 -wd4100 -wd4189 -fp:fast
set LinkerFlags=user32.lib gdi32.lib

pushd Build
cl %CompilerFlags% ../Source/WinMain.cpp ../Source/Engine.cpp %ExternalSource% %LinkerFlags% && del *.obj
SoftwareRenderer
popd

echo -------------------------------------------------------------------------------------------------------