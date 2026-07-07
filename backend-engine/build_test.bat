@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
cd /d "C:\Users\xint2\CLionProjects\NombaCrypt Shell\backend-engine"
echo === Compiling new main.cpp ===
c++.exe -std=gnu++20 -I include -c main.cpp -o main_test.o 2>&1
echo Exit code: %ERRORLEVEL%
if %ERRORLEVEL% EQU 0 (
    echo === COMPILATION SUCCESSFUL ===
) else (
    echo === COMPILATION FAILED ===
)
