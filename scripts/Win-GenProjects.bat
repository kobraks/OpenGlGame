@echo off
pushd %~dp0\..\
call vendor\premake\bin\premake5.exe --verbose vs2022
popd
PAUSE
