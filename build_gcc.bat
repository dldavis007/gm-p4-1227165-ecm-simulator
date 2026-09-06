@echo off
setlocal

if not exist build mkdir build

gcc -std=c89 -Wall -Wextra -pedantic main.c -o build\bua_step118.exe 2> build\compiler_warnings.txt
if errorlevel 1 (
    echo Build failed. See build\compiler_warnings.txt
    exit /b 1
)

for %%A in (build\compiler_warnings.txt) do if %%~zA==0 (
    echo No compiler warnings.> build\compiler_warnings.txt
)

echo Build complete: build\bua_step118.exe
type build\compiler_warnings.txt
endlocal
