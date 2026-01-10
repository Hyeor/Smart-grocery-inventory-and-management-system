# INVENTORY ADMIN Role - Quick Reference

## Role Selection Screen (New Staff Registration)
```
Select Role/Access Level:
1. ADMIN (Full system access)
2. STAFF (Limited access: Dashboard, Inventory, Sales)
3. INVENTORY ADMIN (Inventory & Supplier Management)  ← NEW
Select: 
```

## INVENTORY ADMIN Dashboard
```
================================================================================
                        INVENTORY ADMIN MENU
================================================================================

1. [MONITOR]     Real-Time Monitoring Dashboard
2. [SUPPLIERS]   Supplier Management
3. [INVENTORY]   Inventory & Stock Management
4. [USERS]       User Profile Management
5. [EXIT]        Logout from System

Select option (1-5):
```

## INVENTORY Module Submenu (NEW)
```
================================================================================
                        INVENTORY MODULE
================================================================================

1. [STOCKS]      Manage Product Inventory
2. [PURCHASE]    Place Purchase Orders
3. [RECEIVING]   Receive Order Items
4. [BACK]        Return to Main Menu

Select option (1-4):
```

## Access Matrix

**INVENTORY ADMIN can access:**
- ✓ MONITOR (Dashboard)
- ✓ SUPPLIERS (Management)
- ✓ INVENTORY (Stocks, Purchase, Receiving)
- ✓ USERS (Update own profile: Name, Email, Phone)
- ✗ SALES (NOT available)
- ✗ STAFF Management (NOT available)

## Key Features

### 1. STOCKS Module
- View all products inventory
- Add new products
- Update product information
- Delete products
- Manage stock levels

### 2. PURCHASE Module
- Create purchase orders
- View purchase order history
- Manage supplier orders
- Track order status

### 3. RECEIVING Module
- Receive incoming items
- Update inventory from receiving
- Generate receiving reports
- Track received shipments

### 4. USERS Module (Limited)
- Update own profile:
  - Full Name
  - Email Address
  - Phone Number
- **Cannot modify:**
  - Password
  - Role/Access Level
  - Other user accounts

## Code Changes Summary

| File | Changes | Purpose |
|------|---------|---------|
| User.cpp | Added role selection #3, inventoryAdminMenu(), updateInventoryAdminProfile() | Handle INVENTORY ADMIN user management |
| user.h | Added 2 function declarations | Export new functions |
| Product.cpp | Added inventoryAdminSubmenu() | Restrict inventory access |
| Product.h | Added function declaration | Export new function |
| main.cpp | Added showInventoryAdminDashboard(), updated role routing | Main dashboard for INVENTORY ADMIN |

## Database Compatibility
- New role stored as "INVENTORY ADMIN" in User.role field
- Existing Admin and Staff roles unchanged
- All INVENTORY ADMIN users will have role = "INVENTORY ADMIN"

## Build Status
✓ Compiled successfully
✓ No errors or critical warnings
✓ All features integrated and tested
