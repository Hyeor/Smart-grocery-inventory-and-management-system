# FTXUI Compilation Script for GroceryFlow IMS (PowerShell)
# This script compiles the FTXUI-based frontend

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "   GroceryFlow IMS - FTXUI Frontend Build" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Get the script directory
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir

Write-Host "[*] Project root: $projectRoot" -ForegroundColor Yellow

# Navigate to project root
Push-Location $projectRoot

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    Write-Host "[*] Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Name "build" | Out-Null
}

# Navigate to build directory
Push-Location build

# Check for vcpkg
$vcpkgPath = "C:\vcpkg"
$toolchainArg = ""

if (Test-Path $vcpkgPath) {
    Write-Host "[+] Found vcpkg at $vcpkgPath" -ForegroundColor Green
    $toolchainFile = "$vcpkgPath\scripts\buildsystems\vcpkg.cmake"
    $toolchainArg = "-DCMAKE_TOOLCHAIN_FILE=`"$toolchainFile`" -DVCPKG_TARGET_TRIPLET=x64-windows"
} else {
    Write-Host "[!] vcpkg not found at $vcpkgPath" -ForegroundColor Yellow
    Write-Host "[*] Proceeding without explicit toolchain..." -ForegroundColor Yellow
}

# Configure CMake
Write-Host ""
Write-Host "[*] Configuring CMake..." -ForegroundColor Yellow

$cmakeArgs = @(
    "..",
    "-G", "Visual Studio 17 2022"
)

if ($toolchainArg) {
    $cmakeArgs += $toolchainArg.Split(' ')
}

& cmake @cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[!] CMake configuration failed!" -ForegroundColor Red
    Write-Host "[*] Make sure you have:" -ForegroundColor Yellow
    Write-Host "    - CMake 3.20+ installed" -ForegroundColor Yellow
    Write-Host "    - Visual Studio 17 2022 (Build Tools)" -ForegroundColor Yellow
    Write-Host "    - FTXUI library installed (via vcpkg or manually)" -ForegroundColor Yellow
    Pop-Location
    Pop-Location
    exit 1
}

# Build the project
Write-Host ""
Write-Host "[*] Building project..." -ForegroundColor Yellow

& cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[!] Build failed!" -ForegroundColor Red
    Write-Host "[*] Check the error messages above" -ForegroundColor Yellow
    Pop-Location
    Pop-Location
    exit 1
}

# Success
Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "   Build Successful!" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host ""

$exePath = Join-Path (Get-Location) "Release\main_ui.exe"
Write-Host "[+] Executable location:" -ForegroundColor Green
Write-Host "    $exePath" -ForegroundColor Cyan
Write-Host ""
Write-Host "[*] To run the application:" -ForegroundColor Yellow
Write-Host "    .\Release\main_ui.exe" -ForegroundColor Cyan
Write-Host ""

Pop-Location
Pop-Location
