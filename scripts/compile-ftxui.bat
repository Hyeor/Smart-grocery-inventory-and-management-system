@echo off
REM FTXUI Compilation Script for GroceryFlow IMS
REM This script compiles the FTXUI-based frontend

setlocal enabledelayedexpansion

echo.
echo ============================================
echo   GroceryFlow IMS - FTXUI Frontend Build
echo ============================================
echo.

REM Get the project root directory (parent of scripts folder)
cd /d "%~dp0.."

REM Check if build directory exists
if not exist build (
    echo [*] Creating build directory...
    mkdir build
)

cd build

REM Detect vcpkg location
set VCPKG_PATH=C:\vcpkg
if not exist "%VCPKG_PATH%" (
    echo [!] vcpkg not found at %VCPKG_PATH%
    echo [*] Proceeding without explicit toolchain...
    set TOOLCHAIN_ARG=
) else (
    echo [*] Found vcpkg at %VCPKG_PATH%
    set TOOLCHAIN_ARG=-DCMAKE_TOOLCHAIN_FILE="%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
)

REM Configure CMake
echo [*] Configuring CMake...
cmake .. -G "Visual Studio 17 2022" %TOOLCHAIN_ARG%

if errorlevel 1 (
    echo [!] CMake configuration failed!
    echo [*] Make sure you have:
    echo     - CMake 3.20+ installed
    echo     - Visual Studio 17 2022 (Build Tools)
    echo     - FTXUI library installed (via vcpkg or manually)
    pause
    exit /b 1
)

REM Build the project
echo.
echo [*] Building project...
cmake --build . --config Release

if errorlevel 1 (
    echo [!] Build failed!
    echo [*] Check the error messages above
    pause
    exit /b 1
)

REM Success
echo.
echo ============================================
echo   Build Successful!
echo ============================================
echo.
echo [+] Executable location:
echo     %cd%\Release\main_ui.exe
echo.
echo [*] To run the application:
echo     Release\main_ui.exe
echo.
pause
