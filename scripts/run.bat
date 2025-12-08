@echo off
REM Smart Grocery - Complete Setup and Run with organized structure

echo.
echo ====================================
echo Smart Grocery Management System
echo ====================================
echo.

REM Add libraries folder to PATH
set PATH=%CD%\..\libraries;%PATH%

REM Ensure all required DLLs are in libraries folder
echo Checking MySQL libraries...

if not exist "..\libraries\libmysql.dll" (
    echo Copying libmysql.dll...
    copy "C:\Program Files\MySQL\MySQL Server 9.5\lib\libmysql.dll" ..\libraries\ >nul
    if exist "..\libraries\libmysql.dll" echo ✓ libmysql.dll copied
)

if not exist "..\libraries\libprotobuf.dll" (
    echo Copying libprotobuf.dll...
    copy "C:\Program Files\MySQL\MySQL Server 9.5\lib\libprotobuf.dll" ..\libraries\ >nul
    if exist "..\libraries\libprotobuf.dll" echo ✓ libprotobuf.dll copied
)

if not exist "..\libraries\libssl-3-x64.dll" (
    echo Copying libssl-3-x64.dll...
    copy "C:\Program Files\MySQL\MySQL Server 9.5\bin\libssl-3-x64.dll" ..\libraries\ >nul
    if exist "..\libraries\libssl-3-x64.dll" echo ✓ libssl-3-x64.dll copied
)

if not exist "..\libraries\libcrypto-3-x64.dll" (
    echo Copying libcrypto-3-x64.dll...
    copy "C:\Program Files\MySQL\MySQL Server 9.5\bin\libcrypto-3-x64.dll" ..\libraries\ >nul
    if exist "..\libraries\libcrypto-3-x64.dll" echo ✓ libcrypto-3-x64.dll copied
)

echo.
echo Starting MySQL Server 9.5...

REM Start MySQL in a new window
start "MySQL Server" "C:\Program Files\MySQL\MySQL Server 9.5\bin\mysqld.exe" --datadir="C:\Program Files\MySQL\MySQL Server 9.5\data"

echo Waiting for MySQL to start...
timeout /t 5 /nobreak

REM Try to run the application
echo.
echo Starting Smart Grocery Application...
echo.
..\build\main.exe

echo.
echo Application closed.
pause
