@echo off
cls
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
REM mkdir build
pushd build
cl -Zi /EHsc ..\somecrap.cpp user32.lib gdi32.lib
popd
