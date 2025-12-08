@echo off
REM Compile with MSVC (Visual C++)
echo Compiling with Visual C++ Compiler...

REM Set up MSVC environment
call "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe" --norestart

REM Alternative: Use clang-cl if available
where clang-cl.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Using Clang-CL compiler
    clang-cl /EHsc /std:c++17 main.cpp Database.cpp User.cpp Product.cpp Sales.cpp ^
        /I"C:\Program Files\MySQL\MySQL Server 9.5\include" ^
        /link "C:\Program Files\MySQL\MySQL Server 9.5\lib\libmysql.lib"
    if exist main.exe (
        echo Compilation successful!
        main.exe
    )
)
