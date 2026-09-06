@echo off
call build_gcc.bat
if errorlevel 1 exit /b 1

build\bua_step116.exe > build\bua_step116_output.txt
if errorlevel 1 (
    echo Program returned an error.
    exit /b 1
)

type build\bua_step116_output.txt
