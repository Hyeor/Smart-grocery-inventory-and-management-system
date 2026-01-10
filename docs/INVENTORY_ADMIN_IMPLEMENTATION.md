# INVENTORY ADMIN Role Implementation

## Overview
A new "INVENTORY ADMIN" role has been successfully added to the system. This role provides specialized access for inventory and supplier management without full administrative privileges.

## Role Access Structure

### Role Selection
When registering new staff, administrators now see **3 role options**:
1. **ADMIN** - Full system access
2. **STAFF** - Limited access: Monitor, Sales, Staff Management
3. **INVENTORY ADMIN** - Inventory and Supplier Management

### INVENTORY ADMIN Permissions

The INVENTORY ADMIN dashboard displays 5 main modules:

```
INVENTORY ADMIN MENU
├── 1. MONITOR      → Real-Time Monitoring Dashboard
├── 2. SUPPLIERS    → Supplier Management
├── 3. INVENTORY    → Inventory & Stock Management
├── 4. USERS        → User Profile Management
└── 5. EXIT         → Logout from System
```

#### Module 1: MONITOR
- Access to real-time monitoring dashboard
- View stock levels, sales metrics, and system status

#### Module 2: SUPPLIERS
- Full supplier management capabilities
- View supplier details
- Add new suppliers
- Update supplier information
- Delete suppliers
- Process supplier-related transactions

#### Module 3: INVENTORY
The INVENTORY module has a restricted submenu with 4 sub-modules:
```
INVENTORY ADMIN SUBMENU
├── 1. STOCKS     → Manage Product Inventory
│   ├── View all products
│   ├── Add new products
│   ├── Update product details
│   └── Delete products
├── 2. PURCHASE   → Place Purchase Orders
│   ├── Create new purchase orders
│   ├── View existing orders
│   ├── Update orders
│   └── Receive order status
├── 3. RECEIVING  → Receive Order Items
│   ├── Log received items
│   ├── Update stock levels
│   └── Generate receiving reports
└── 4. BACK       → Return to Main Menu
```

**Note:** Unlike Admin, INVENTORY ADMIN cannot:
- Process sales transactions
- Access the SALES module
- Manage user accounts (only update own profile)

#### Module 4: USERS
- Update own user profile (limited fields):
  - Full Name
  - Email Address
  - Phone Number
- Cannot:
  - Change password
  - Modify role/access level
  - View other user profiles
  - Manage other staff accounts

## Implementation Details

### Files Modified

#### 1. **source/User.cpp**
- Updated `addUser()` function: Added INVENTORY ADMIN as role choice #3
- Added `inventoryAdminMenu()`: Manages INVENTORY ADMIN user profile operations
- Added `updateInventoryAdminProfile()`: Allows limited profile updates (Name, Email, Phone)

#### 2. **headers/user.h**
- Added function declarations:
  - `void inventoryAdminMenu(Database& db);`
  - `void updateInventoryAdminProfile(Database& db);`

#### 3. **source/Product.cpp**
- Added `inventoryAdminSubmenu()`: Specialized inventory menu for INVENTORY ADMIN role
- Restricted to: STOCKS, PURCHASE, RECEIVING, BACK
- Removed access to: SALES module

#### 4. **headers/Product.h**
- Added function declaration:
  - `void inventoryAdminSubmenu(Database& db, PurchaseOrderManager* poMgr, ReceivingManager* recvMgr);`

#### 5. **source/main.cpp**
- Added `showInventoryAdminDashboard()`: New dashboard function for INVENTORY ADMIN role
- Updated role-based routing in `main()`:
  - If role == "INVENTORY ADMIN": Call `showInventoryAdminDashboard()`
  - If role == "ADMIN": Call `showAdminDashboard()`
  - Else (STAFF): Call `showStaffDashboard()`

## User Workflow Example

### For INVENTORY ADMIN User:

1. **Login**: Enter credentials as INVENTORY ADMIN
2. **Dashboard**: See INVENTORY ADMIN menu with 5 options
3. **Monitor**: View real-time system metrics
4. **Suppliers**: Manage supplier information
5. **Inventory**: 
   - Access STOCKS submenu to manage products
   - Access PURCHASE submenu to create/view orders
   - Access RECEIVING submenu to process incoming items
6. **Users**: Update personal profile information
7. **Exit**: Logout to return to login screen

### Restrictions vs. Other Roles

| Action | ADMIN | STAFF | INVENTORY ADMIN |
|--------|-------|-------|-----------------|
| Monitor Dashboard | ✓ | ✓ | ✓ |
| Manage Suppliers | ✓ | ✗ | ✓ |
| Manage Inventory | ✓ | ✗ | ✓ |
| Purchase Orders | ✓ | ✗ | ✓ |
| Receiving Items | ✓ | ✗ | ✓ |
| Process Sales | ✓ | ✓ | ✗ |
| Manage Staff | ✓ | ✗ | ✗ |
| Update Own Profile | ✓ | ✓ | ✓ |
| Manage Other Users | ✓ | ✗ | ✗ |
| System Settings | ✓ | ✗ | ✗ |

## Build Status
✓ Successfully compiled with g++ -std=c++17
✓ No compilation errors
✓ All role-based routing implemented
✓ Database compatibility maintained

## Testing Recommendations

1. **Registration Test**: Verify INVENTORY ADMIN appears as option #3 when registering staff
2. **Login Test**: Test login with INVENTORY ADMIN credentials
3. **Dashboard Test**: Verify correct dashboard displays for INVENTORY ADMIN role
4. **Module Access Test**: 
   - Verify MONITOR, SUPPLIERS, INVENTORY, USERS, EXIT appear
   - Verify SALES module is NOT visible
5. **Inventory Submenu Test**: 
   - Verify STOCKS, PURCHASE, RECEIVING, BACK options appear
   - Verify functions work correctly for each sub-module
6. **Profile Update Test**: Verify INVENTORY ADMIN can only update Name, Email, Phone
7. **Permission Test**: Verify INVENTORY ADMIN cannot access admin-only functions

## Next Steps

- Update database documentation to reflect new role
- Create training materials for INVENTORY ADMIN users
- Consider adding audit logs for inventory admin actions
- Test with real supplier/inventory data
