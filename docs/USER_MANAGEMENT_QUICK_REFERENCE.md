# User Management Module - Quick Reference Guide

## Quick Start

### Initial Setup
1. Run database migration: `migrate_user_schema.sql`
2. Compile project with updated code
3. Login with: **STF001** / **admin**
4. Change password when prompted

---

## Login Flow

```
Enter Staff ID (e.g., STF001)
↓
Enter Password (masked)
↓
System checks:
  ├─ Is account ACTIVE?
  ├─ Is password correct?
  └─ Do we need password change?
↓
If first login: Force password change
↓
Grant access based on role (ADMIN/STAFF)
```

---

## Staff ID Format

- **Pattern**: `STF` + 3 digits
- **Examples**: `STF001`, `STF002`, `STF003`, etc.
- **Auto-generated**: Cannot edit manually
- **Unique**: No duplicates allowed

---

## Staff Information Fields

When registering new staff, provide:
1. **Full Name** - Up to 30 words (e.g., Muhammad Faris bin Fazil)
2. **Email** - Valid format (e.g., faris@company.com)
3. **Phone** - Min 7 digits (e.g., +60-12-3456789)
4. **Position** - Job title (e.g., Manager, Clerk, Admin)
5. **Role** - ADMIN or STAFF
6. **Account Status** - ACTIVE (default) or INACTIVE

---

## Admin Operations (User Management)

### Register New Staff
```
Menu Option: 1
Steps:
  - System auto-generates Staff ID
  - Enter staff details
  - Select role (ADMIN/STAFF)
  - Default password set to "admin"
  - Staff must change on first login
```

### View All Staff
```
Menu Option: 2
Shows: Staff ID, Name, Email, Phone, Position, Role, Status, Date Created
```

### View Staff Details
```
Menu Option: 3
Shows: All fields for selected staff (by Staff ID)
```

### Update Staff
```
Menu Option: 4
Can update:
  - Full Name
  - Email
  - Phone
  - Position
  - Role (ADMIN/STAFF)
  - Reset Password to "admin"
```

### Deactivate Account
```
Menu Option: 5
Result: Sets account_status = INACTIVE
Data: All information preserved
Effect: Staff cannot login
```

### Reactivate Account
```
Menu Option: 6
Result: Sets account_status = ACTIVE
Effect: Staff can login again
```

---

## Staff Operations (Self-Service)

### Change Own Password (After Login)
```
Option: Available in login sequence when password_change_required = 1
Steps:
  1. Enter current password
  2. Enter new password (min 6 chars)
  3. Confirm new password
  4. Flag is reset to 0
```

### View Own Details
```
Via: Monitoring Dashboard (if available)
Shows: Own staff profile information
```

---

## Role Comparison

| Feature | ADMIN | STAFF |
|---------|-------|-------|
| Monitoring Dashboard | ✓ | ✓ |
| Inventory Management | ✓ | ✓ |
| Process Sale | ✓ | ✓ |
| User Management | ✓ | ✗ |
| Supplier Management | ✓ | ✗ |
| View Staff Accounts | ✓ | ✗ |
| Register Staff | ✓ | ✗ |
| Deactivate Account | ✓ | ✗ |

---

## Common Tasks

### Task: Create New Manager Account
```
1. Go to: User Management → Register New Staff
2. Fill details (system generates STF002)
3. Select Role: ADMIN
4. Share Staff ID and default password
5. Manager changes password on first login
```

### Task: Disable Account (Temporarily)
```
1. Go to: User Management → Deactivate Staff Account
2. Enter Staff ID
3. Confirm deactivation
4. Staff cannot login anymore
5. All data is preserved
```

### Task: Re-enable Account
```
1. Go to: User Management → Reactivate Staff Account
2. Enter Staff ID
3. Confirm reactivation
4. Staff can login again with same credentials
```

### Task: Reset Forgotten Password
```
1. Go to: User Management → Update Staff
2. Enter Staff ID
3. Select: Reset Password to 'admin'
4. Staff logs in with "admin"
5. Forced to change password
```

### Task: Change Staff Role
```
1. Go to: User Management → Update Staff
2. Enter Staff ID
3. Select: Role/Access Level
4. Choose new role (ADMIN/STAFF)
5. Confirm change
```

---

## Validation Rules

### Email
- Must contain `@` symbol
- Must contain `.` (dot)
- Example: `user@company.com` ✓

### Phone
- Minimum 7 digits
- Can include: digits (0-9), `-`, `+`, spaces
- Examples:
  - `0123456789` ✓
  - `+60-12-3456789` ✓
  - `011 2345 6789` ✓
  - `123` ✗ (too short)

### Password
- Minimum 6 characters
- Hashed using MD5 in database
- Masked during input
- Case-sensitive

### Full Name
- Maximum 30 words recommended
- No special validation (allows spaces, hyphens, etc.)

---

## Database Schema Quick Reference

```sql
User Table:
├── user_id (INT, Primary Key)
├── staff_id (VARCHAR, Unique) → STF001, STF002...
├── full_name (VARCHAR)
├── email (VARCHAR, Unique)
├── phone (VARCHAR)
├── position (VARCHAR)
├── role (ENUM: ADMIN, STAFF)
├── password (VARCHAR, hashed)
├── account_status (ENUM: ACTIVE, INACTIVE)
├── password_change_required (TINYINT: 0 or 1)
└── date_created (TIMESTAMP)
```

---

## Troubleshooting

### Can't Login
- Verify Staff ID (case-sensitive, e.g., STF001)
- Check if account is ACTIVE (not INACTIVE)
- Verify password is correct (case-sensitive)
- Check database connection

### Forgot Password
- Admin: Use "Reset Password" option → set to "admin"
- Staff: Contact admin to reset

### Can't See User Management Menu
- Check your role (STAFF users don't see it)
- Only ADMIN role has access
- Login as admin (STF001)

### Email Already Exists Error
- Each staff must have unique email
- Change email address to unique one
- Or ask admin to help resolve

### Phone Validation Failed
- Phone must have minimum 7 digits
- Can use format: +60-12-3456789
- Invalid: "123" (too short)

---

## Menu Paths

### For ADMIN Users
```
Main Dashboard
├─ 1. Monitoring Dashboard
├─ 2. Inventory Management
├─ 3. User Management ← (6 options)
│  ├─ 1. Register New Staff
│  ├─ 2. View All Staff
│  ├─ 3. View Staff Details
│  ├─ 4. Update Staff Information
│  ├─ 5. Deactivate Staff Account
│  ├─ 6. Reactivate Staff Account
│  └─ 7. Back to Dashboard
├─ 4. Supplier Management
├─ 5. Process Sale
└─ 6. Logout
```

### For STAFF Users
```
Main Dashboard
├─ 1. Monitoring Dashboard
├─ 2. Inventory Management
├─ 3. Process Sale
└─ 4. Logout
```

---

## Important Notes

1. **Staff ID is Login Username**: Not a separate field, it IS the username
2. **First Login Password Change**: Automatic and required
3. **Deactivation is Safe**: Doesn't delete data, just disables login
4. **Admin Account**: STF001 / admin (change on first login!)
5. **Email & Phone**: Both must be unique per staff member
6. **Account Status**: Only affects login ability, not data visibility

---

## File Locations

- **Database Script**: `scripts/migrate_user_schema.sql`
- **Full Guide**: `docs/USER_MANAGEMENT_GUIDE.md`
- **Implementation**: `docs/USER_MANAGEMENT_IMPLEMENTATION.md`
- **Header**: `headers/user.h`
- **Code**: `source/User.cpp`

---

## Version

**Current Version**: 2.0 (January 4, 2025)
**Previous**: 1.0 (Basic username/password system)

---

## Support

For detailed information, refer to:
- `USER_MANAGEMENT_GUIDE.md` - Complete documentation
- `SYSTEM_OVERVIEW.md` - System architecture
- Code comments in `headers/user.h` and `source/User.cpp`
