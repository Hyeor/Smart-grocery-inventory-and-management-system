# Smart Grocery - Project Structure

## Organized File Layout

```
smart grocery/
├── .vscode/                    # VS Code configuration
│   ├── c_cpp_properties.json  # C++ IntelliSense settings
│   ├── launch.json            # Debugger configuration
│   └── settings.json          # Editor settings
│
├── source/                     # C++ Source Files
│   ├── main.cpp               # Application entry point
│   ├── Database.cpp           # Database connection implementation
│   ├── User.cpp               # User authentication implementation
│   ├── Product.cpp            # Inventory management implementation
│   └── Sales.cpp              # Sales transaction implementation
│
├── headers/                    # C++ Header Files
│   ├── Database.h             # Database class declaration
│   ├── User.h                 # UserManager class declaration
│   ├── Product.h              # InventoryManager class declaration
│   └── Sales.h                # SalesManager class declaration
│
├── build/                      # Compiled Output
│   ├── main.exe               # Executable application
│   ├── main.o                 # Object file (main)
│   ├── Database.o             # Object file (Database)
│   ├── User.o                 # Object file (User)
│   ├── Product.o              # Object file (Product)
│   └── Sales.o                # Object file (Sales)
│
├── libraries/                  # Runtime Libraries (DLLs)
│   ├── libmysql.dll           # MySQL client library
│   ├── libprotobuf.dll        # Protocol buffers library
│   ├── libssl-3-x64.dll       # OpenSSL/TLS library
│   └── libcrypto-3-x64.dll    # OpenSSL crypto library
│
├── scripts/                    # Build & Execution Scripts
│   ├── run.bat                # Main launcher (start MySQL + app)
│   ├── compile.bat            # Generic compilation script
│   ├── compile-mingw64.bat    # MinGW-w64 compilation
│   ├── compile-msvc.bat       # MSVC compilation
│   └── install-mingw64.ps1    # MinGW-w64 installer
│
├── docs/                       # Documentation & Configuration
│   ├── README.md              # Project overview and quickstart
│   ├── COMPILATION_GUIDE.md   # Detailed compilation instructions
│   ├── CMakeLists.txt         # CMake build configuration
│   └── Makefile               # GNU Make configuration
│
└── Root Files:
    ├── CMakeLists.txt         # CMake configuration (build system)
    └── Makefile               # Make configuration (build system)
```

## File Organization by Type

| Folder | Purpose | File Types |
|--------|---------|-----------|
| `source/` | Source code | `.cpp` |
| `headers/` | Header declarations | `.h` |
| `build/` | Compiled artifacts | `.exe`, `.o` |
| `libraries/` | Runtime dependencies | `.dll` |
| `scripts/` | Build & run scripts | `.bat`, `.ps1` |
| `docs/` | Documentation | `.md`, `.txt`, `.cmake` |

## How to Use

### To Run the Application
```cmd
cd scripts
run.bat
```

### To Recompile
```cmd
cd scripts
compile-mingw64.bat
```

### File Navigation

- **Edit source code**: `source/` folder
- **Review declarations**: `headers/` folder
- **Run application**: `build/main.exe`
- **View documentation**: `docs/` folder
- **Execute scripts**: `scripts/` folder

## Compilation Workflow

1. Source code in `source/` is compiled
2. Object files (`.o`) created in `build/`
3. Linker creates `main.exe` in `build/`
4. DLLs from `libraries/` are loaded at runtime
5. Scripts in `scripts/` automate the process

## Project Configuration Files

- **CMakeLists.txt** - CMake build system
- **Makefile** - GNU Make build system
- **.vscode/** - VS Code IDE configuration

All configuration files have been moved to `docs/` for organization.
