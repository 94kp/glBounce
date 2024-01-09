cls
cd %~dp0
MD build
MD build\intermediates
MD build\outputs
DEL build\outputs\OGL.exe
DEL build\intermediates\OGL.obj
DEL build\intermediates\OGL.res
cl.exe /Fobuild\intermediates\  /c /EHsc /I C:\glew\include src\main\OGL.cpp
rc.exe /fobuild\intermediates\OGL.res src\res\OGL.rc 
link.exe /OUT:build\outputs\OGL.exe build\intermediates\OGL.obj build\intermediates\OGL.res /LIBPATH:C:\glew\lib\Release\x64 src\lib\Sphere.lib gdi32.lib user32.lib /SUBSYSTEM:WINDOWS

build\outputs\OGL.exe

type Log.txt
