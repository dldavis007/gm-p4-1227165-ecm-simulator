@echo off
call build_gcc.bat
if errorlevel 1 exit /b 1

build\bua_step115.exe > build\bua_step115_output.txt
if errorlevel 1 (
    echo Program returned an error.
    exit /b 1
)

type build\bua_step115_output.txt
