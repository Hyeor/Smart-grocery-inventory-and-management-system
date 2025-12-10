@echo off
REM Smart Grocery - Complete Setup and Run with organized structure

REM Add libraries folder to PATH
set PATH=%CD%\..\libraries;%PATH%

REM Ensure all required DLLs are in libraries folder (silently)
if not exist "..\libraries\libmysql.dll" (
    copy "C:\Program Files\MySQL\MySQL Server 9.5\lib\libmysql.dll" ..\libraries\ >nul 2>&1
)

if not exist "..\libraries\libprotobuf.dll" (
    copy "C:\Program Files\MySQL\MySQL Server 9.5\lib\libprotobuf.dll" ..\libraries\ >nul 2>&1
)

if not exist "..\libraries\libssl-3-x64.dll" (
    copy "C:\Program Files\MySQL\MySQL Server 9.5\bin\libssl-3-x64.dll" ..\libraries\ >nul 2>&1
)

if not exist "..\libraries\libcrypto-3-x64.dll" (
    copy "C:\Program Files\MySQL\MySQL Server 9.5\bin\libcrypto-3-x64.dll" ..\libraries\ >nul 2>&1
)

REM Start MySQL in background (silently)
start /B "" "C:\Program Files\MySQL\MySQL Server 9.5\bin\mysqld.exe" --datadir="C:\Program Files\MySQL\MySQL Server 9.5\data" >nul 2>&1

REM Wait for MySQL to start (silently)
timeout /t 5 /nobreak >nul 2>&1

REM Clear screen and run the application
cls
..\build\main.exe
