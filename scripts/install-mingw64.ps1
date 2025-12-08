# PowerShell script to install and setup 64-bit MinGW
$winLibsUrl = "https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-13.0.0-ucrt-r4/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4.zip"
$installDir = "C:\mingw64"
$zipPath = "$env:TEMP\winlibs.zip"

Write-Host "Downloading 64-bit MinGW-w64 from WinLibs..."
try {
    # Download with retry
    for ($i = 0; $i -lt 3; $i++) {
        try {
            Invoke-WebRequest -Uri $winLibsUrl -OutFile $zipPath -UseBasicParsing -TimeoutSec 300
            break
        } catch {
            Write-Host "Attempt $($i+1) failed, retrying..."
            Start-Sleep -Seconds 5
        }
    }
    
    if (Test-Path $zipPath) {
        Write-Host "Download complete. Extracting..."
        Expand-Archive -Path $zipPath -DestinationPath $installDir -Force
        
        Write-Host "Adding to PATH..."
        $env:PATH = "$installDir\bin;$env:PATH"
        [Environment]::SetEnvironmentVariable("PATH", "$installDir\bin;$([Environment]::GetEnvironmentVariable('PATH', 'User'))", "User")
        
        Write-Host "Verifying installation..."
        & "$installDir\bin\gcc.exe" --version
        
        Write-Host "64-bit MinGW installed successfully!"
        Write-Host "Installation path: $installDir"
    } else {
        Write-Host "Download failed. Please download manually from:"
        Write-Host "https://github.com/brechtsanders/winlibs_mingw/releases"
    }
} catch {
    Write-Host "Error during installation: $_"
    Write-Host "Please install manually from: https://github.com/brechtsanders/winlibs_mingw/releases"
}
