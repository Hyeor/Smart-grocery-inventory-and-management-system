# Smart Grocery Management System

A C++ project for managing grocery store inventory, sales, and user authentication using MySQL.

## Quick Start

### Prerequisites
- MySQL Server 9.5+ (must be running)
- MinGW-w64 (64-bit) compiler
- VS Code (with C++ extensions)

### Installation & Compilation

1. **Install 64-bit MinGW-w64** (if not already installed):
   ```powershell
   # Run in PowerShell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
   powershell -File install-mingw64.ps1
   ```

2. **Compile the project**:
   ```
   Double-click: compile-mingw64.bat
   ```
   Or manually:
   ```powershell
   C:\mingw64\bin\g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp `
     -I"C:\Program Files\MySQL\MySQL Server 9.5\include" `
     -L"C:\Program Files\MySQL\MySQL Server 9.5\lib" `
     -lmysql
   ```

3. **Run the application**:
   ```
   .\main.exe
   ```

## Project Structure

```
├── main.cpp              Main program entry point
├── Database.cpp/.h       MySQL database connection handling
├── User.cpp/.h           User authentication system
├── Product.cpp/.h        Inventory management
├── Sales.cpp/.h          Sales transaction processing
├── Makefile              Build configuration
├── CMakeLists.txt        CMake build system
└── compile-mingw64.bat   Automated compilation script
```

## Features

### User Management
- Login system with role-based access
- Roles: Admin, Staff

### Inventory Management  
- Add products to database
- Track stock quantities
- View inventory list
- Updates after sales

### Sales Processing
- Record sales transactions
- Calculate profits automatically
- Update stock accordingly
- SQL injection protection needed

## Database Connection
- **Host:** localhost
- **User:** root  
- **Password:** (none)
- **Database:** smart_grocery
- **Port:** 3306

**Important:** MySQL Server must be running before starting the application

## Build Options

### Option 1: Batch Script (Easiest)
```cmd
compile-mingw64.bat
```

### Option 2: Make
```powershell
make compile    # Create object files
make link       # Link to executable
make clean      # Remove build files
```

### Option 3: CMake
```powershell
cmake -B build -G "MinGW Makefiles"
cd build
mingw32-make
```

### Option 4: Manual g++
```powershell
C:\mingw64\bin\g++ -o main.exe main.cpp Database.cpp User.cpp Product.cpp Sales.cpp `
  -I"C:\Program Files\MySQL\MySQL Server 9.5\include" `
  -L"C:\Program Files\MySQL\MySQL Server 9.5\lib" `
  -lmysql
```

## Troubleshooting

### MySQL Connection Failed
- Ensure MySQL Server is running
- Check credentials (default: root, no password)
- Verify database "smart_grocery" exists

### Compilation Errors
- Install 64-bit MinGW-w64 (not 32-bit MinGW)
- Reset C++ IntelliSense: Ctrl+Shift+P → "C/C++: Reset IntelliSense Engine"
- Check MySQL include paths in `.vscode/c_cpp_properties.json`

### "mysql.h not found"
- Verify MySQL Server 9.5 installed at: `C:\Program Files\MySQL\MySQL Server 9.5`
- Update include path in c_cpp_properties.json if installed elsewhere

## Development Notes

- Uses MySQL C API for database operations
- Classes defined in header files for inline compilation
- No input validation (for educational purposes)
- SQL injection vulnerabilities present (for learning)

## Next Steps

1. Install 64-bit MinGW-w64
2. Start MySQL Server
3. Run compile-mingw64.bat
4. Execute main.exe

See COMPILATION_GUIDE.md for detailed build instructions.
