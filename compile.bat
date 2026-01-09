@echo off
REM Quick Compile Script for GroceryFlow IMS
REM Compiles all source files into main.exe in project root

echo ========================================
echo   GroceryFlow IMS - Quick Compiler
echo ========================================
echo.

REM Stop any running instances
taskkill /F /IM main.exe >nul 2>&1

REM Change to source directory
cd source

REM Compile using 64-bit MinGW
echo Compiling project...
"C:\Users\faris\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\g++.exe" ^
    -Wall -Wextra -std=c++17 ^
    -o "..\main.exe" ^
    main.cpp Database.cpp User.cpp Product.cpp Sales.cpp Supplier.cpp Dashboard.cpp PurchaseOrder.cpp Receiving.cpp ^
    -I"../headers" ^
    -I"C:/Program Files/MySQL/MySQL Server 9.5/include" ^
    -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" ^
    -lmysql -lgdi32 -lcomdlg32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   SUCCESS! main.exe compiled
    echo ========================================
    echo.
    echo Location: %~dp0main.exe
    echo.
) else (
    echo.
    echo ========================================
    echo   COMPILATION FAILED!
    echo ========================================
    echo.
)

cd ..
pause
