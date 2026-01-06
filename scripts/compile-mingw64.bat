@echo off
REM Smart Grocery - Compile with 64-bit MinGW-w64
setlocal enabledelayedexpansion

echo.
echo Smart Grocery - C++ Compilation Script
echo =====================================
echo.

REM Check if 64-bit MinGW is installed
if exist "C:\mingw64\bin\g++.exe" (
    echo Found 64-bit MinGW at: C:\mingw64
    echo.
    echo Compiling...
    echo.
    
    REM Set paths relative to project root
    set HEADERS_PATH=%CD%\headers
    set MYSQL_INCLUDE="C:\Program Files\MySQL\MySQL Server 9.5\include"
    set MYSQL_LIB="C:\Program Files\MySQL\MySQL Server 9.5\lib"
    
    C:\mingw64\bin\g++.exe -o build\main.exe source\Database.cpp source\User.cpp source\Product.cpp source\Sales.cpp source\Supplier.cpp source\Dashboard.cpp source\PurchaseOrder.cpp source\Receiving.cpp source\main.cpp ^
        -I!HEADERS_PATH! ^
        -I!MYSQL_INCLUDE! ^
        -L!MYSQL_LIB! ^
        -lmysql -static-libgcc -static-libstdc++
    
    if exist build\main.exe (
        echo.
        echo SUCCESS! Executable created: build\main.exe
        echo.
        echo To run: scripts\run.bat
        pause
    ) else (
        echo.
        echo COMPILATION FAILED
        echo Check errors above
        pause
    )
) else (
    echo.
    echo ERROR: 64-bit MinGW not found at C:\mingw64
    echo.
    echo Please install MinGW-w64 first
    echo.
    pause
)
