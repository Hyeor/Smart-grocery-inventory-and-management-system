# User Management Module - New Requirements Implementation

## Overview
The User Management Module has been completely redesigned to implement proper staff identity management with auto-generated Staff IDs, forced password changes on first login, account status control, and role-based access restrictions.

---

## Table of Contents
1. [Username & Authentication](#username--authentication)
2. [Staff Identity Fields](#staff-identity-fields)
3. [Account Control Features](#account-control-features)
4. [Role-Based Access Control](#role-based-access-control)
5. [Database Schema](#database-schema)
6. [Migration Instructions](#migration-instructions)
7. [Usage Examples](#usage-examples)
8. [API Reference](#api-reference)

---

## Username & Authentication

### Login Method
- **Username**: Uses **Staff ID** (auto-generated, e.g., `STF001`, `STF002`)
- **Password**: Masked input during login
- **Storage**: Hashed using MySQL MD5 function
- **First Login**: Staff must change their password on first login
- **Attempts**: Maximum 3 failed login attempts before access denied

### Login Flow
```
1. User enters Staff ID (e.g., STF001)
2. User enters password (masked input with asterisks)
3. System validates against database
4. If credentials correct AND account ACTIVE:
   - Check if password_change_required flag is set
   - If YES: Force password change dialog
   - If NO: Grant access based on role
5. If credentials invalid or account INACTIVE: Show error and retry
```

### Default Credentials (Initial Admin)
- **Staff ID**: `STF001`
- **Default Password**: `admin`
- **Password Change Required**: YES (must change on first login)

### Password Change on First Login
```
[NOTICE] You must change your password on first login.
=== CHANGE PASSWORD ===
Enter current password: [masked input]
Enter new password: [masked input, min 6 chars]
Confirm new password: [masked input]
[OK] Password changed successfully!
```

---

## Staff Identity Fields

### Required Fields for Each Staff Member

| Field | Type | Details | Example |
|-------|------|---------|---------|
| **Staff ID** | VARCHAR(10) | Auto-generated, unique | `STF001` |
| **Full Name** | VARCHAR(255) | First, middle, last names | `Muhammad Faris bin Fazil` |
| **Email Address** | VARCHAR(100) | Valid email format | `faris@company.com` |
| **Phone Number** | VARCHAR(20) | With country code | `+60-12-3456789` |
| **Position/Job Title** | VARCHAR(100) | Role title | `Manager`, `Clerk`, `Admin` |
| **Role/Access Level** | ENUM | ADMIN or STAFF | `STAFF` |
| **Account Status** | ENUM | ACTIVE or INACTIVE | `ACTIVE` |
| **Date Created** | TIMESTAMP | Account creation date | `2025-01-04 14:30:45` |

### Staff ID Generation
- Format: `STF` + 3-digit number (auto-incremented)
- Examples: `STF001`, `STF002`, `STF003`, etc.
- Auto-generated when creating new staff account
- Cannot be manually edited

### Email & Phone Validation
- **Email**: Must be valid format (contains @ and .)
- **Phone**: Must contain at least 7 digits (can include -, +, spaces)
- Both are validated during staff registration and updates

---

## Account Control Features

### Account Status Management
Admin users can:
- **Deactivate**: Disable staff account without deleting data
- **Reactivate**: Re-enable a previously deactivated account
- **Preserve Data**: All staff information retained when deactivated

### Inactive Account Behavior
- Cannot login (even with correct password)
- All data preserved in system
- Can be reactivated later
- Better than deletion for compliance/auditing

### Password Reset
- Admin can reset staff password to default `admin`
- Password change required flag is automatically set to 1
- Staff must change password on next login

---

## Role-Based Access Control

### Two Access Levels

#### 1. ADMIN Role
**Full System Access:**
- Monitoring Dashboard
- Inventory Management (CRUD operations)
- **User Management** (register, update, deactivate/reactivate)
- Supplier Management
- Process Sale
- View all reports and analytics

**Admin-Only Operations:**
- Register new staff
- Update staff information
- Deactivate/Reactivate accounts
- Reset staff passwords
- View all staff accounts

#### 2. STAFF Role
**Limited Access:**
- Monitoring Dashboard (read-only)
- Inventory Management (view products, update stock)
- Process Sale (create and record transactions)

**Restricted Access:**
- ❌ User Management (no access)
- ❌ Supplier Management (no access)
- ❌ Cannot register or manage staff accounts
- ❌ Cannot view other staff information

### Dashboard Menu by Role

**Admin Dashboard:**
```
1. Monitoring Dashboard
2. Inventory Management
3. User Management ← Admin-only
4. Supplier Management
5. Process Sale
6. Logout
```

**Staff Dashboard:**
```
1. Monitoring Dashboard
2. Inventory Management
3. Process Sale ← Limited to sales operations only
4. Logout
```

---

## Database Schema

### New User Table Structure
```sql
CREATE TABLE User (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    staff_id VARCHAR(10) UNIQUE NOT NULL,
    full_name VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    phone VARCHAR(20) NOT NULL,
    position VARCHAR(100) NOT NULL,
    role ENUM('ADMIN', 'STAFF') NOT NULL DEFAULT 'STAFF',
    password VARCHAR(255) NOT NULL,
    account_status ENUM('ACTIVE', 'INACTIVE') NOT NULL DEFAULT 'ACTIVE',
    password_change_required TINYINT(1) NOT NULL DEFAULT 1,
    date_created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);
```

### Index Strategy
- `idx_staff_id`: Fast login lookups by Staff ID
- `idx_email`: Prevent duplicate emails
- `idx_account_status`: Quick filtering of active accounts
- `idx_role`: Fast role-based queries

---

## Migration Instructions

### Step 1: Backup Existing Data (if any)
```sql
CREATE TABLE User_Backup AS SELECT * FROM User;
```

### Step 2: Run Migration Script
```bash
cd scripts
mysql -u root -p smart_grocery < migrate_user_schema.sql
```

Or in phpMyAdmin:
1. Select `smart_grocery` database
2. Go to "SQL" tab
3. Copy content of `migrate_user_schema.sql`
4. Execute

### Step 3: Verify Migration
```sql
USE smart_grocery;
DESCRIBE User;
SELECT * FROM User;
```

### Step 4: Test Login
- Login with Staff ID: `STF001`
- Password: `admin`
- Follow prompt to change password

---

## Usage Examples

### Example 1: Register New Manager
```
=== ADD NEW STAFF ===
Auto-generated Staff ID: STF002
Enter Full Name: Ahmad bin Abdullah
Enter Email Address: ahmad@company.com
Enter Phone Number: +60-11-2345678
Enter Position/Job Title: Manager

Select Role/Access Level:
1. ADMIN (Full system access)
2. STAFF (Limited access: Dashboard, Inventory, Sales)
Select: 1

[OK] New staff account created successfully!
Staff ID: STF002
Default Password: admin
[NOTE] Staff must change password on first login.
```

### Example 2: Manager First Login
```
=== LOGIN ===
Staff ID: STF002
Password: [****]

[OK] Login Valid! Welcome STF002

[NOTICE] You must change your password on first login.
=== CHANGE PASSWORD ===
Enter current password: [****]
Enter new password: [MySecurePass123]
Confirm new password: [MySecurePass123]
[OK] Password changed successfully!
```

### Example 3: Update Staff Information
```
=== UPDATE STAFF INFORMATION ===
Enter Staff ID to update: STF002
--- STAFF DETAILS ---
Staff ID: STF002
Full Name: Ahmad bin Abdullah
...

What do you want to update?
1. Full Name
2. Email
3. Phone
4. Position
5. Role/Access Level
6. Reset Password to 'admin'
Select: 4

Select new Role:
1. ADMIN
2. STAFF
Select: 2

[OK] Staff updated successfully.
```

### Example 4: Deactivate Account
```
=== DEACTIVATE STAFF ACCOUNT ===
Enter Staff ID to deactivate: STF003

--- STAFF DETAILS ---
Staff ID: STF003
Full Name: Siti Nurhaliza
Account Status: ACTIVE
...

Are you sure you want to deactivate this account? (y/n): y
[OK] Staff account deactivated successfully.
```

---

## API Reference

### Class: UserManager

#### Public Methods

##### `string getPasswordInput()`
Reads password from user with masking (asterisks).
- **Returns**: User-entered password string
- **Features**: Handles spaces, backspace, Enter key

##### `bool isValidEmail(const string& email)`
Validates email format.
- **Parameter**: `email` - Email address to validate
- **Returns**: `true` if valid, `false` otherwise
- **Pattern**: Requires @ symbol and . (dot)

##### `bool isValidPhone(const string& phone)`
Validates phone number format.
- **Parameter**: `phone` - Phone number to validate
- **Returns**: `true` if valid (min 7 digits), `false` otherwise
- **Allows**: Digits, -, +, spaces

##### `string generateStaffID(Database& db)`
Auto-generates next Staff ID.
- **Parameter**: `db` - Database connection
- **Returns**: Next Staff ID (e.g., "STF001", "STF002")
- **Logic**: Queries max existing ID and increments

##### `string getCurrentTimestamp()`
Gets current date/time in database format.
- **Returns**: Timestamp string (YYYY-MM-DD HH:MM:SS)
- **Used**: For date_created field

##### `string login(Database& db)`
Authenticates user with Staff ID and password.
- **Parameter**: `db` - Database connection
- **Returns**: User's role ("ADMIN" or "STAFF") if success, "" if failed
- **Features**: 
  - 3-attempt retry limit
  - Checks account_status = "ACTIVE"
  - Forces password change if flag set

##### `void changePassword(Database& db, const string& staffID)`
Allows staff to change their password.
- **Parameters**: `db` - Database, `staffID` - Staff ID
- **Features**:
  - Verifies current password first
  - Requires min 6 character password
  - Requires confirmation
  - Resets password_change_required flag

##### `void addUser(Database& db)`
Registers new staff member (Admin-only).
- **Parameter**: `db` - Database connection
- **Prompts**:
  - Full Name
  - Email (validated)
  - Phone (validated)
  - Position
  - Role (ADMIN/STAFF)
- **Auto-generated**: Staff ID
- **Default**: Password "admin", must change on login

##### `void viewUsers(Database& db)`
Displays list of all staff members.
- **Parameter**: `db` - Database connection
- **Columns**: Staff ID, Name, Email, Phone, Position, Role, Status, Date Created

##### `void viewUser(Database& db, const string& staffID)`
Displays detailed info for one staff member.
- **Parameters**: `db` - Database, `staffID` - Staff ID to view
- **Shows**: All staff identity fields and account status

##### `void updateUser(Database& db)`
Updates staff information (Admin-only).
- **Parameter**: `db` - Database connection
- **Options**:
  1. Full Name
  2. Email
  3. Phone
  4. Position
  5. Role/Access Level
  6. Reset Password to 'admin'

##### `void deactivateUser(Database& db)`
Deactivates staff account (Admin-only).
- **Parameter**: `db` - Database connection
- **Result**: Sets account_status = 'INACTIVE'
- **Data**: All information preserved

##### `void reactivateUser(Database& db)`
Reactivates deactivated staff account (Admin-only).
- **Parameter**: `db` - Database connection
- **Result**: Sets account_status = 'ACTIVE'
- **Confirmation**: Required before reactivation

##### `void userManagementMenu(Database& db)`
Main menu for staff management (Admin-only).
- **Parameter**: `db` - Database connection
- **Options**:
  1. Register New Staff
  2. View All Staff
  3. View Staff Details
  4. Update Staff Information
  5. Deactivate Staff Account
  6. Reactivate Staff Account
  7. Back to Dashboard

---

## Security Features

1. **Password Hashing**: MD5 hashing via MySQL
2. **Masked Input**: Password input hidden with asterisks
3. **Login Attempts**: Limited to 3 attempts before lockout
4. **Email Validation**: Prevents invalid email addresses
5. **Account Status**: Disable without deletion
6. **First Login Change**: Force password change on initial login
7. **Role-Based Access**: Restricts STAFF from management functions

---

## Compilation Notes

Include the updated header file:
```cpp
#include "user.h"  // Now includes ctime for timestamps
```

Compiled size: ~270 KB (slightly larger due to new features)

---

## Troubleshooting

### Q: User can't login
**A:** Check:
- Staff ID is correct (case-sensitive)
- Account status is ACTIVE (not INACTIVE)
- Password is correct (remember it's case-sensitive)
- Database connection is valid

### Q: Password change dialog not appearing
**A:** Admin may not have set `password_change_required = 1`. Use reset password option.

### Q: Can't reset password
**A:** Only ADMIN role can reset passwords. STAFF can only change their own.

### Q: Email already exists error
**A:** Email field is unique. Choose a different email address.

### Q: Phone validation fails
**A:** Phone must have minimum 7 digits. Can include -, +, spaces.

---

## Version History

- **v2.0** (2025-01-04): Complete redesign
  - Staff ID auto-generation (STF001 format)
  - Full staff identity fields
  - Forced password change on first login
  - Account status control (ACTIVE/INACTIVE)
  - Role-based access control
  - Improved validation
  
- **v1.0** (previous): Basic username/password system

---

## Related Files

- **Header**: [headers/user.h](../headers/user.h)
- **Implementation**: [source/User.cpp](../source/User.cpp)
- **Database**: [scripts/migrate_user_schema.sql](../scripts/migrate_user_schema.sql)
- **Main**: [source/main.cpp](../source/main.cpp)
- **Dashboard**: [headers/Dashboard.h](../headers/Dashboard.h)

---

## Support & Feedback

For issues or questions about the user management system, please refer to the main README or contact the development team.
