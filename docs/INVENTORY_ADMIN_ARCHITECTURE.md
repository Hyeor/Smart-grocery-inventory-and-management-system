# System Architecture - INVENTORY ADMIN Role

## Complete Role Hierarchy

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         SMART GROCERY IMS ROLES                         │
└─────────────────────────────────────────────────────────────────────────┘

                            LOGIN SCREEN
                                 │
                                 │ username + password
                                 ▼
                         ROLE VERIFICATION
                         (from database)
                                 │
                  ┌──────────────┼──────────────┐
                  │              │              │
                  ▼              ▼              ▼
              "ADMIN"         "STAFF"    "INVENTORY ADMIN" ◄── NEW
                  │              │              │
                  ▼              ▼              ▼
            ┌─────────┐   ┌────────────┐   ┌──────────────────────┐
            │  ADMIN  │   │   STAFF    │   │ INVENTORY ADMIN      │
            │DASHBOARD│   │ DASHBOARD  │   │   DASHBOARD          │
            └─────────┘   └────────────┘   └──────────────────────┘
                 │              │                   │
        ┌────────┼────────┐     │         ┌─────────┼─────────┬────────┐
        │        │        │     │         │         │         │        │
        ▼        ▼        ▼     ▼         ▼         ▼         ▼        ▼
     MONITOR SUPPLIERS INVENTORY USERS  MONITOR SUPPLIERS INVENTORY USERS
        │        │        │     │         │         │         │        │
        │        │        └─────┘         │         │         │        │
        │        │         STAFF ONLY     │         │         │        │
        │        │                        │         │         │        │
        ▼        ▼                        ▼         ▼         ▼        ▼
     (FULL)  (FULL)                    (FULL)   (FULL)   (LIMITED)  (LIMITED)
             SALES                                           │         │
             │                                               │         │
             ▼                                               ▼         ▼
          (LIMITED)                                    ┌─────────────┐ │
                                                       │ STOCKS      │ │
                                                       │ PURCHASE    │ │
                                                       │ RECEIVING   │ │
                                                       └─────────────┘ │
                                                                       │
                                                       (Profile update
                                                        Name, Email,
                                                        Phone only)
```

## INVENTORY ADMIN Dashboard Structure

```
┌────────────────────────────────────────────────────────────────────┐
│                      INVENTORY ADMIN MAIN MENU                     │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  1. [MONITOR]     Real-Time Monitoring Dashboard                 │
│  2. [SUPPLIERS]   Supplier Management                            │
│  3. [INVENTORY]   Inventory & Stock Management                   │
│  4. [USERS]       User Profile Management                        │
│  5. [EXIT]        Logout from System                             │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
         │              │              │              │
         ▼              ▼              ▼              ▼
    ┌─────────┐  ┌──────────────┐ ┌──────────────┐ ┌─────────┐
    │ MONITOR │  │  SUPPLIERS   │ │  INVENTORY   │ │  USERS  │
    │         │  │              │ │   (LIMITED)  │ │(LIMITED)│
    └─────────┘  └──────────────┘ └──────────────┘ └─────────┘
         │              │              │              │
         │              │        ┌─────┼──┐           │
         │              │        │     │  │           │
         ▼              ▼        ▼     ▼  ▼           ▼
    ┌─────────┐  ┌──────────────┐ ┌──────────────────┐
    │Dashboard│  │• View         │ │ 1. STOCKS        │
    │Metrics  │  │• Add          │ │    (Manage Inv)  │
    │ - Stock │  │• Update       │ │ 2. PURCHASE      │
    │ - Sales │  │• Delete       │ │    (PO Mgmt)     │
    │ - Profit│  │  Suppliers    │ │ 3. RECEIVING     │
    └─────────┘  │              │ │    (Shipments)   │
                 └──────────────┘ │ 4. BACK          │
                                  │    (Dashboard)   │
                                  └──────────────────┘
                                         │
                                  ┌──────┼──────┬──────────┐
                                  │      │      │          │
                                  ▼      ▼      ▼          ▼
                              ┌──────┐ ┌────┐ ┌────────┐  └──┐
                              │View/ │ │PO  │ │Receive │ Back│
                              │Add/  │ │Mgmt│ │Shipmts │    │
                              │Update│ │    │ │        │    │
                              │Prod. │ │    │ │        │    │
                              └──────┘ └────┘ └────────┘    │
                                                            │
                                   ┌────────────────────────┘
                                   │
                           Return to Dashboard
```

## File Inclusion Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                    main.cpp (Entry Point)                        │
│                                                                   │
│  showAdminDashboard()           ◄── Existing (Admin role)       │
│  showStaffDashboard()           ◄── Existing (Staff role)       │
│  showInventoryAdminDashboard()  ◄── NEW (InventoryAdmin role)   │
└─────────────────────────────────────────────────────────────────┘
  │                      │                    │
  ├─► User.cpp ─────────┼────────────────────┤
  │                      │                    │
  │   addUser()          │   staffMgmt        │
  │   (Role selection)   │   inventoryAdmn    │
  │                      │   updateProfile    │
  │                      ▼                    │
  ├─► Supplier.cpp       UserManager         │
  │   supplierPage()                         │
  │                                          │
  ├─► Product.cpp ──────────────────────────┘
  │   stocksMenu()          (STOCKS)
  │   inventoryAdminSubmenu()  (LIMITED)
  │   inventoryPage()       (FULL - ADMIN only)
  │
  ├─► Dashboard.cpp
  │   showMonitoringDashboard()
  │
  ├─► Sales.cpp
  │   salesPage()   ◄── NOT accessible to InventoryAdmin
  │
  └─► PurchaseOrder.cpp & Receiving.cpp
      (Called from inventoryAdminSubmenu)
```

## Data Flow - INVENTORY ADMIN Login

```
┌─────────────┐
│   START     │
└──────┬──────┘
       │
       ▼
┌──────────────────────┐
│  Enter Credentials   │
├──────────────────────┤
│ Staff ID: STFxxx     │
│ Password: ****       │
└──────┬───────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│  Database Query:                     │
│  SELECT role, password FROM User     │
│  WHERE staff_id = 'STFxxx'           │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│  Verify Password Match               │
└──────┬───────────────────────────────┘
       │
       ├─ Valid? ──► YES
       │              │
       ▼              │
     NO ──► ┌─────────┴──────────────┐
           │ Show Error Message     │
           │ Return to Login        │
           └───────────────────────┘
           
           YES ──► ┌──────────────────────────────────┐
                   │ Check Role from Database         │
                   └──────┬───────────────────────────┘
                          │
                   ┌──────┼──────┬─────────────────┐
                   │      │      │                 │
              "ADMIN"  "STAFF"  "INVENTORY" (other)
                   │      │      │ ADMIN           │
                   ▼      ▼      ▼                 ▼
              showAdmin showStaff showInventory showStaff
              Dashboard Dashboard AdminDash     Dashboard
                   │      │      │
                   └──────┴──────┴─ (User in appropriate menu)
                        │
                        ▼
                 ┌──────────────────────┐
                 │  Display Menu        │
                 │  (Based on Role)     │
                 └──────┬───────────────┘
                        │
                        ▼
                 (User can select options)
```

## Permission Matrix

```
┌────────────────────┬───────┬───────┬──────────────────┐
│      ACTION        │ ADMIN │ STAFF │ INVENTORY ADMIN  │
├────────────────────┼───────┼───────┼──────────────────┤
│ View Dashboard     │  ✓    │  ✓    │       ✓          │
│ Monitor System     │  ✓    │  ✓    │       ✓          │
│ Manage Suppliers   │  ✓    │  ✗    │       ✓          │
│ Manage Products    │  ✓    │  ✗    │       ✓          │
│ Create PO          │  ✓    │  ✗    │       ✓          │
│ Receive Items      │  ✓    │  ✗    │       ✓          │
│ Process Sales      │  ✓    │  ✓    │       ✗          │
│ Manage Staff       │  ✓    │  ✗    │       ✗          │
│ Update Own Profile │  ✓    │  ✓    │       ✓          │
│ Update Others      │  ✓    │  ✗    │       ✗          │
│ Change Own Pass    │  ✓    │  ✓    │       ✓          │
│ Change Role        │  ✓    │  ✗    │       ✗          │
│ System Settings    │  ✓    │  ✗    │       ✗          │
└────────────────────┴───────┴───────┴──────────────────┘

Legend:
  ✓ = Allowed
  ✗ = Denied
```

## Code Locations Reference

```
NEW/MODIFIED FUNCTIONS:

[Source/User.cpp]
  ├─ addUser()                        (MODIFIED: +INVENTORY ADMIN option)
  ├─ inventoryAdminMenu()             (NEW)
  └─ updateInventoryAdminProfile()    (NEW)

[Source/Product.cpp]
  └─ inventoryAdminSubmenu()          (NEW)

[Source/main.cpp]
  ├─ showInventoryAdminDashboard()    (NEW)
  └─ main()                           (MODIFIED: +role routing)

[Headers/user.h]
  ├─ void inventoryAdminMenu()        (NEW declaration)
  └─ void updateInventoryAdminProfile() (NEW declaration)

[Headers/Product.h]
  └─ void inventoryAdminSubmenu()     (NEW declaration)
```

---

**Status**: ✓ Complete and Compiled
**Last Updated**: January 11, 2026
**Build Output**: main.exe (Ready for deployment)
