# Smart Grocery Project - Compilation Guide

## Current Status
✅ **Code compiles successfully** - All source files are syntactically correct
✅ **Includes configured** - MySQL headers are properly found
❌ **Linking issue** - 32-bit MinGW cannot link 64-bit MySQL libraries

## Quick Solution

### Option 1: Compile with 64-bit MinGW (RECOMMENDED)
After 64-bit MinGW is installed to `C:\mingw64`, run:

```powershell
C:\mingw64\bin\g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp `
  -I"C:/Program Files/MySQL/MySQL Server 9.5/include" `
  -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" `
  -lmysql
```

Or use the batch file:
```cmd
compile-mingw64.bat
```

### Option 2: Use MSVC Compiler
If you have Visual Studio installed, use:
```cmd
compile-msvc.bat
```

### Option 3: Install via WinGet
```powershell
winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT --accept-source-agreements --accept-package-agreements
```

## Files Modified
- **.vscode/c_cpp_properties.json** - Updated with MySQL and MinGW paths
- **Database.h** - Header file for Database class
- **User.h** - Header file for UserManager class  
- **Product.h** - Header file for InventoryManager class
- **Sales.h** - Header file for SalesManager class
- **Sales.cpp** - Added includes
- **User.cpp** - Added includes
- **Makefile** - Build configuration
- **CMakeLists.txt** - CMake configuration

## Database Connection Details
- **Host:** localhost
- **User:** root
- **Password:** (empty)
- **Database:** smart_grocery
- **Port:** 3306

Make sure MySQL Server 9.5 is running before executing the program!

## Compilation Steps

### Manual Step-by-Step:
```powershell
# 1. Navigate to project directory
cd "C:\Users\faris\OneDrive\Desktop\WORKSHOP 1\Project\smart grocery"

# 2. Compile with 64-bit MinGW
C:\mingw64\bin\g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp ^
  -I"C:\Program Files\MySQL\MySQL Server 9.5\include" ^
  -L"C:\Program Files\MySQL\MySQL Server 9.5\lib" ^
  -lmysql

# 3. Run the executable
.\main.exe
```

### Using Make:
```powershell
# Compile object files only
make compile

# Link after 64-bit MinGW installation  
make link

# Clean build
make clean
```

## Troubleshooting

### "mysql.h: No such file or directory"
- Check if MySQL is installed at: `C:\Program Files\MySQL\MySQL Server 9.5`
- Verify .vscode/c_cpp_properties.json includes path is correct

### "cannot open source file"
- Reset C++ IntelliSense in VS Code: Ctrl+Shift+P → "C/C++: Reset IntelliSense Engine"

### Linking errors
- Ensure 64-bit MinGW is installed (not 32-bit)
- Verify MySQL libraries exist in lib directory

## Project Structure
```
smart grocery/
├── main.cpp
├── Database.cpp         Database.h
├── User.cpp             User.h  
├── Product.cpp          Product.h
├── Sales.cpp            Sales.h
├── Makefile
├── CMakeLists.txt
├── compile.bat
├── compile-msvc.bat
└── install-mingw64.ps1
```

