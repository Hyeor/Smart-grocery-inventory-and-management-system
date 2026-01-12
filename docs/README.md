# GROCERYFLOW IMS

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
     -I"[MySQL_Install_Path]\include" `
     -L"[MySQL_Install_Path]\lib" `
     -lmysql
   ```

3. **Run the application**:
   ```
   .\main.exe
   ```

## Project Structure

```
smart-grocery/
├── main.cpp                Main program entry point
├── headers/
│   ├── Database.h          MySQL database connection handling
│   ├── User.h              User authentication system
│   ├── Product.h           Inventory management
│   ├── Sales.h             Sales transaction processing
│   └── UI_Helpers.h        Console UI utilities (ANSI colors, formatting)
├── src/
│   ├── Database.cpp        Database implementation
│   ├── User.cpp            User authentication implementation
│   ├── Product.cpp         Inventory management implementation
│   └── Sales.cpp           Sales processing implementation
├── docs/
│   └── README.md           This file
├── Makefile                Build configuration
├── CMakeLists.txt          CMake build system
└── compile-mingw64.bat     Automated compilation script
```

## Features

### User Management
- Login system with role-based access
- Roles: Admin, Staff
- Secure credential handling

### Inventory Management  
- Add/update products in database
- Track stock quantities
- View inventory list
- Real-time stock updates after sales
- Product categorization

### Sales Processing
- Record sales transactions
- Calculate profits automatically
- Update stock accordingly
- Transaction history tracking
- Report generation

### UI/UX
- ANSI color-coded console interface
- Professional banner and formatted menus
- Table-based data display
- Loading animations
- Cross-platform console support (Windows/Linux)

## System Flowchart

The following diagram illustrates the complete system architecture and workflow of GROCERYFLOW IMS, from application entry through role-based access to various functional modules.

```mermaid
flowchart TB
  A[GROCERYFLOW IMS] --> B[Register / Login]
  B --> C{Database Connected?}
  C -- No --> E[Show error and retry<br>(MySQL must be running)]
  C -- Yes --> R{Role}

  R --> ADM[Admin]
  R --> STF[Staff]

  subgraph "Admin Panel"
    direction TB
    ADM --> AD1[Dashboard]

    AD1 --> INV_A[Inventory Management]
    INV_A --> A_ADD[Add Product]
    INV_A --> A_UPD[Update Product]
    INV_A --> A_VIEW[View Inventory]
    INV_A --> A_CAT[Categorize Products]

    AD1 --> SALES_A[Sales Processing]
    SALES_A --> A_REC[Record Sale]
    SALES_A --> A_STK[Update Stock]
    SALES_A --> A_PROF[Calculate Profit]
    SALES_A --> A_TXN[Transaction History]

    AD1 --> RPT_A[Reports]
    RPT_A --> A_GEN[Generate Sales Reports]
    RPT_A --> A_SUM[Profit Summary]

    AD1 --> ACC_A[Account]
    ACC_A --> A_UPRO[Update Profile]
  end

  subgraph "Staff Panel"
    direction TB
    STF --> SD1[Dashboard]

    SD1 --> INV_S[Inventory Management]
    INV_S --> S_ADD[Add Product]
    INV_S --> S_UPD[Update Product]
    INV_S --> S_VIEW[View Inventory]
    INV_S --> S_CAT[Categorize Products]

    SD1 --> SALES_S[Sales Processing]
    SALES_S --> S_REC[Record Sale]
    SALES_S --> S_STK[Update Stock]
    SALES_S --> S_PROF[Calculate Profit]
    SALES_S --> S_TXN[Transaction History]

    SD1 --> RPT_S[Reports]
    RPT_S --> S_GEN[Generate Sales Reports]

    SD1 --> ACC_S[Account]
    ACC_S --> S_UPRO[Update Profile]
  end

  A_TXN --> END[(End)]
  S_TXN --> END
```

📊 **[View Full Flowchart](flowchart.md)** - Interactive version with detailed explanations

> **Note**: This diagram is rendered natively by GitHub's Mermaid support. No external rendering services are required, ensuring reliability and preventing 404 errors.

## Database Configuration

Configure your MySQL connection settings in `Database.h`:
- **Host:** localhost
- **User:** [your_username]
- **Password:** [your_password]
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
C:\mingw64\bin\g++ -o main.exe main.cpp src/Database.cpp src/User.cpp src/Product.cpp src/Sales.cpp `
  -I"headers" `
  -I"[MySQL_Install_Path]\include" `
  -L"[MySQL_Install_Path]\lib" `
  -lmysql
```

## Troubleshooting

### MySQL Connection Failed
- Ensure MySQL Server is running
- Verify credentials are correct
- Check that database "smart_grocery" exists
- Verify port 3306 is accessible

### Compilation Errors
- Install 64-bit MinGW-w64 (not 32-bit MinGW)
- Reset C++ IntelliSense: Ctrl+Shift+P → "C/C++: Reset IntelliSense Engine"
- Check include paths match your MySQL installation

### "mysql.h not found"
- Verify MySQL Server installation path
- Update include/library paths in build configuration
- Ensure MySQL development headers are installed

## Development Notes

- Uses MySQL C API for database operations
- Modular architecture with separate concerns
- Cross-platform console UI support
- Educational codebase with focus on core concepts
- Professional error handling and user feedback

## System Requirements

- **OS:** Windows 10+ / Linux
- **Compiler:** GCC 9.0+
- **Database:** MySQL 9.5+
- **RAM:** 2GB minimum
- **Disk:** 500MB for dependencies

## Getting Started

1. Install 64-bit MinGW-w64
2. Install MySQL Server and create database
3. Configure database credentials in Database.h
4. Start MySQL Server
5. Run compile-mingw64.bat
6. Execute main.exe

## Security Notes

- Update database credentials in configuration files
- Use strong passwords for database access
- Implement input validation for production use
- Review SQL query construction for injection prevention

See COMPILATION_GUIDE.md for detailed build instructions.
