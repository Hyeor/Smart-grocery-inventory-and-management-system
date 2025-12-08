# Smart Grocery Management System - Complete Overview

## Project Information
- **Project Name:** Smart Grocery Management System
- **Language:** C++ (C++17 Standard)
- **Database:** MySQL Server 9.5
- **Compiler:** MinGW-w64 (64-bit) v15.2.0
- **Location:** `c:\Users\faris\OneDrive\Desktop\WORKSHOP 1\Project\smart grocery`
- **Build Output:** `build\main.exe`

---

## System Architecture

### 1. Database Layer
**Database Name:** `smart_grocery`

#### Tables

##### User Table
- **Columns:**
  - `user_id` (INT, Primary Key, Auto-increment)
  - `username` (VARCHAR 50, NOT NULL)
  - `password` (VARCHAR 255, NOT NULL)
  - `role` (VARCHAR 20, NOT NULL)
- **Roles:** Admin, Staff
- **Purpose:** System user authentication and authorization

##### Product Table
- **Columns:**
  - `product_id` (INT, Primary Key, Auto-increment)
  - `name` (VARCHAR, NOT NULL) - Product name (flexible: 1-3+ words)
  - `cost_price` (DECIMAL, NOT NULL) - Cost price in RM
  - `sell_price` (DECIMAL, NOT NULL) - Selling price in RM
  - `stock_quantity` (INT, NOT NULL) - Current stock level
  - `supplier_id` (INT, Foreign Key) - Links to Supplier
  - `date_created` (TIMESTAMP) - Creation date
- **Purpose:** Inventory management with supplier tracking

##### Supplier Table
- **Columns:**
  - `supplier_id` (INT, Primary Key, Auto-increment)
  - `supplier_name` (VARCHAR, NOT NULL)
  - `contact_person` (VARCHAR, NOT NULL)
  - `phone` (VARCHAR, NOT NULL)
  - `status` (VARCHAR, NOT NULL) - "Active" or "Inactive"
  - `date_created` (TIMESTAMP)
- **Purpose:** Supplier management and product sourcing

##### Sales Table
- **Columns:**
  - `sale_id` (INT, Primary Key, Auto-increment)
  - `user_id` (INT, Foreign Key) - Links to User
  - `total_amount` (DECIMAL, NOT NULL) - Total sale amount in RM
  - `profit` (DECIMAL) - Profit calculation in RM
- **Purpose:** Sales transactions and profit tracking

---

## Module Structure

### Core Components

#### 1. User Management Module (`User.cpp` / `user.h`)
**Responsible for:** Authentication and user administration

**Features:**
- **Authentication (Login)**
  - 3-attempt retry mechanism for failed login
  - Username/password validation
  - Role-based access (Admin/Staff)
  - Session establishment

- **Create User (Admin-only)**
  - Input fields: Username, Password, Role
  - Role options: Admin (1), Staff (2)
  - Validation: No empty fields
  - Database insert with auto-increment ID

- **Read User Operations**
  - View all users with table format: #, User ID, Username, User Type
  - View specific user by ID with full details (ID, Username, Password, Role)
  - Formatted output with line separators

- **Update User (Admin-only)**
  - Select user by ID
  - Update options:
    1. Username
    2. Password
    3. User Role (Admin/Staff)
  - Confirmation after update

- **Delete User (Admin-only)**
  - Select user by ID
  - Show user details before deletion
  - Confirmation prompt (y/n)
  - Removes user from system

**Menu Structure:**
```
USER MANAGEMENT
1. Create New User
2. View All Users
3. View Specific User
4. Update User
5. Delete User
6. Back to Main Menu
```

---

#### 2. Inventory Management Module (`Product.cpp` / `Product.h`)
**Responsible for:** Product CRUD operations and stock tracking

**Features:**
- **Create Product**
  - Input: Product Name, Cost Price, Sell Price, Stock Quantity, Supplier Selection
  - Supplier selection via dropdown (lists active suppliers)
  - Input validation: Positive numbers, non-empty names
  - Automatic date_created timestamp

- **Read Product Operations**
  - View all products in table format with:
    - Product ID
    - Date Created (YYYY-MM-DD HH:MM:SS)
    - Product Name
    - Supplier Name (from Supplier table)
    - Cost Price (RM prefix)
    - Sell Price (RM prefix)
    - Stock Quantity
    - Visual stock level bar chart: `[████----]` format
  - View specific product with full details including supplier and date created

- **Update Product**
  - Select product by ID
  - Update options:
    1. Product Name
    2. Cost Price
    3. Sell Price
    4. Stock Quantity
    5. Supplier Assignment
  - Can reassign to different supplier

- **Delete Product**
  - Select product by ID
  - Confirmation before deletion

- **Helper Functions**
  - `displaySuppliers()` - Shows list of active suppliers for selection
  - Stock level visualization with ASCII bar chart

**Menu Structure:**
```
INVENTORY MANAGEMENT
1. Add Product (CREATE)
2. View All Products (READ)
3. View Specific Product (READ)
4. Update Product (UPDATE)
5. Delete Product (DELETE)
6. Back to Dashboard
```

---

#### 3. Supplier Management Module (`Supplier.cpp` / `Supplier.h`)
**Responsible for:** Supplier relationship management (Admin-only)

**Features:**
- **Create Supplier**
  - Input: Supplier Name, Contact Person, Phone Number
  - Default status: "Active"
  - Automatic date_created timestamp

- **Read Supplier Operations**
  - View all suppliers in table format with:
    - Supplier ID
    - Supplier Name
    - Contact Person
    - Phone
    - Status (Active/Inactive)
    - Date Created
  - View specific supplier by ID

- **Update Supplier**
  - Select supplier by ID
  - Update options:
    1. Supplier Name
    2. Contact Person
    3. Phone Number
    4. Status (Active/Inactive toggle)

- **Delete Supplier**
  - Select supplier by ID
  - Confirmation before deletion
  - Shows supplier details before removal

**Menu Structure:**
```
SUPPLIER MANAGEMENT (Admin-only)
1. Add Supplier
2. View All Suppliers
3. View Specific Supplier
4. Update Supplier
5. Delete Supplier
6. Back to Main Menu
```

---

#### 4. Sales Processing Module (`Sales.cpp` / `Sales.h`)
**Responsible for:** Sales transactions and profit calculation

**Features:**
- **Process Sale**
  - Select product from inventory
  - Enter quantity to sell
  - Automatic calculations:
    - Total Amount = Sell Price × Quantity
    - Profit = (Sell Price - Cost Price) × Quantity
  - Stock level update (automatic deduction)
  - Transaction recording with user_id, total_amount, profit

**Output Format:**
- Displays transaction details with RM currency formatting
- Shows profit calculation

---

#### 5. Database Connection Module (`Database.cpp` / `Database.h`)
**Responsible for:** MySQL connectivity and query execution

**Features:**
- MySQL connection initialization
- Connection validation
- Query execution (INSERT, SELECT, UPDATE, DELETE)
- Result handling
- Error management

---

## Dashboard & Access Control

### Main Dashboard
```
=== DASHBOARD (Role) ===
1. Inventory Management
[2. User Management]        <- Admin only
[2. Supplier Management]    <- Admin only
[3. Process Sale]           <- Staff/Admin
[4/5. Exit]                 <- Position varies by role
```

### Role-Based Access

#### Admin Role
- **Full system access:**
  1. Inventory Management - Full CRUD
  2. User Management - Full CRUD (Create/Read/Update/Delete users)
  3. Supplier Management - Full CRUD
  4. Process Sale - Can process transactions
  5. Exit

- **Permissions:**
  - Can create new users with Admin/Staff roles
  - Can manage all system data
  - Can access all modules
  - Can process sales transactions

#### Staff Role
- **Limited access:**
  1. Inventory Management - Full CRUD (can manage products)
  2. Supplier Management - Read-only (cannot create/edit)
  3. Process Sale - Can process transactions
  4. Exit

- **Permissions:**
  - Cannot access User Management module
  - Cannot create/modify suppliers (admin-only)
  - Can view and manage inventory
  - Can process sales transactions
  - Cannot see User Management option in dashboard

---

## Key Features

### 1. Authentication System
- **3-Attempt Retry Mechanism**
  - User gets 3 attempts to enter correct credentials
  - Failed 3 times = automatic exit
  - Prevents brute force attacks

### 2. Currency Formatting
- **Malaysian Ringgit (RM) Prefix**
  - All prices display with "RM" prefix
  - Applied to: Cost Price, Sell Price, Total Amount, Profit
  - Example: `RM 25.50`

### 3. Stock Level Visualization
- **ASCII Bar Chart**
  - Visual representation of stock levels
  - Format: `[████----]` (filled/empty ratio)
  - Helps quick identification of low-stock items

### 4. Input Validation
- **Numeric Input Protection**
  - `cin.clear()` - Clears error flags
  - `cin.ignore()` - Removes invalid characters
  - Range checking (1-5 for menu selections, > 0 for prices/quantities)
  - Empty field validation

### 5. Date Tracking
- **Automatic Timestamps**
  - Products: `date_created` stored in database
  - Suppliers: `date_created` stored in database
  - Format: YYYY-MM-DD HH:MM:SS

### 6. Flexible Product Naming
- **Accepts Multiple Word Names**
  - Single word: "Apple"
  - Multiple words: "Fresh Green Apple"
  - No artificial restrictions on product names

---

## Data Flow

### Sales Transaction Flow
```
1. Admin/Staff logs in
2. Selects "Process Sale"
3. System loads products from database
4. User selects product
5. User enters quantity
6. System calculates:
   - Total Amount = Sell Price × Quantity
   - Profit = (Sell Price - Cost Price) × Quantity
7. System updates:
   - Stock Quantity (deducts sale quantity)
   - Sales table (records transaction)
8. Displays confirmation with RM amounts
```

### Inventory Update Flow
```
1. Admin/Staff adds product with supplier
2. Product stored with:
   - supplier_id (FK to Supplier table)
   - date_created timestamp
   - All pricing in RM
3. When viewing inventory:
   - Product data joined with Supplier name
   - Date created displayed
   - Stock level shown with bar chart
```

---

## File Structure

```
smart grocery/
├── source/
│   ├── main.cpp           - Application entry point, dashboard, menu routing
│   ├── User.cpp           - User authentication and CRUD implementation
│   ├── Product.cpp        - Inventory management implementation
│   ├── Supplier.cpp       - Supplier CRUD implementation
│   ├── Sales.cpp          - Sales transaction processing
│   └── Database.cpp       - MySQL connection and query execution
├── headers/
│   ├── user.h             - User management class definition
│   ├── Product.h          - Inventory management class definition
│   ├── Supplier.h         - Supplier management class definition
│   ├── Sales.h            - Sales processing class definition
│   └── Database.h         - Database connection class definition
├── build/
│   └── main.exe           - Compiled executable
├── scripts/
│   ├── compile.bat        - Compilation batch script
│   ├── compile-mingw64.ps1 - MinGW64 PowerShell script
│   ├── compile-msvc.bat   - MSVC compilation script
│   ├── install-mingw64.ps1 - MinGW64 installation script
│   └── run.bat            - Application launcher
├── docs/
│   ├── README.md
│   ├── PROJECT_STRUCTURE.md
│   ├── COMPILATION_GUIDE.md
│   ├── INPUT_VALIDATION_FIXES.md
│   ├── PRODUCT_NAME_VALIDATION.md
│   └── SYSTEM_OVERVIEW.md (this file)
├── Makefile               - Build configuration
└── build/                 - Compiled output directory
```

---

## Compilation & Execution

### Compilation Command
```powershell
C:\mingw64\bin\g++.exe -o build\main.exe source\*.cpp `
  -I"headers" `
  -I"C:/Program Files/MySQL/MySQL Server 9.5/include" `
  -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" `
  -lmysql
```

### Running Application
```powershell
build\main.exe
```

### Requirements
- MySQL Server 9.5 running on localhost
- Database `smart_grocery` created
- Root user with no password (or update connection credentials)

---

## User Workflow Example

### Admin User Workflow
```
1. Execute build\main.exe
2. Login with admin credentials
3. View Dashboard with 5 options
4. Option 1: Manage inventory (add/view/update/delete products)
5. Option 2: Manage users (create Staff accounts, modify permissions)
6. Option 3: Manage suppliers (add suppliers, update contact info)
7. Option 4: Process sales
8. Option 5: Exit application
```

### Staff User Workflow
```
1. Execute build\main.exe
2. Login with staff credentials
3. View Dashboard with 4 options (User Management hidden)
4. Option 1: Manage inventory (add/view/update/delete products)
5. Option 2: Manage suppliers (READ-ONLY, cannot create/modify)
6. Option 3: Process sales
7. Option 4: Exit application
8. Cannot access: User Management module
```

---

## Security Features

1. **Role-Based Access Control (RBAC)**
   - Admin-only functions hidden from Staff
   - Dashboard dynamically shows available options per role
   - Access denial messages for unauthorized attempts

2. **Login Security**
   - 3-attempt retry limit
   - Automatic session exit after failures
   - Role-based dashboard customization

3. **Data Validation**
   - Input range checking
   - Empty field validation
   - Numeric input error handling

4. **Confirmation Prompts**
   - Delete operations require confirmation
   - Prevents accidental data loss

---

## Future Enhancement Possibilities

1. **Email Notifications** - Alert admin on low stock
2. **Advanced Reporting** - Sales reports, profit analysis
3. **User Password Hashing** - Encrypt stored passwords
4. **Audit Logging** - Track all data modifications
5. **Multi-location Support** - Branch/warehouse management
6. **Barcode Integration** - QR code scanning for products
7. **Export Functionality** - CSV/PDF report generation
8. **Backup/Restore** - Database backup automation

---

## System Status

**Last Updated:** December 8, 2025
**Version:** 1.0 (Stable)
**Build Status:** ✓ Compilation Successful
**Database:** ✓ Connected
**All Modules:** ✓ Operational

---

## Support & Documentation

For additional information, refer to:
- `README.md` - Quick start guide
- `COMPILATION_GUIDE.md` - Setup and compilation instructions
- `PROJECT_STRUCTURE.md` - Detailed file organization
- `INPUT_VALIDATION_FIXES.md` - Input handling details
- `PRODUCT_NAME_VALIDATION.md` - Product naming rules
