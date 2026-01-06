@echo off
REM Smart Grocery Compilation Script
REM This script compiles the project with MySQL support

echo Compiling Smart Grocery Project...
echo.

REM Check if 64-bit MinGW is available
where x86_64-w64-mingw32-g++.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Found 64-bit MinGW compiler
    x86_64-w64-mingw32-g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp Supplier.cpp Dashboard.cpp PurchaseOrder.cpp Receiving.cpp ^
        -I"C:/Program Files/MySQL/MySQL Server 9.5/include" ^
        -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" ^
        -lmysql -static-libgcc -static-libstdc++
    goto :end
)

REM Try with current MinGW
where g++.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Warning: Using 32-bit MinGW (may have linking issues with 64-bit MySQL)
    echo Try installing 64-bit MinGW-w64 for better compatibility
    g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp Supplier.cpp Dashboard.cpp PurchaseOrder.cpp Receiving.cpp ^
        -I"C:/Program Files/MySQL/MySQL Server 9.5/include" ^
        -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" ^
        -lmysql
    goto :end
)

echo Error: No C++ compiler found!
echo Please install MinGW or Visual Studio C++ tools
goto :end

:end
if exist main.exe (
    echo.
    echo Compilation successful!
    echo Running main.exe...
    echo.
    main.exe
) else (
    echo Compilation failed. Check the errors above.
)
