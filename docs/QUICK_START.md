# ⚡ QUICK START - User Management v2.0

## 5-Minute Setup

### 1️⃣ Run Database Migration
```sql
USE smart_grocery;
SOURCE scripts/migrate_user_schema.sql;
```

### 2️⃣ Compile Code
```bash
compile.bat
```

### 3️⃣ Run Application
```bash
main.exe
```

### 4️⃣ Login with Default Admin
```
Staff ID: STF001
Password: admin
```

### 5️⃣ Change Password (Forced)
```
Current password: admin
New password: (your choice, min 6 chars)
Confirm: (repeat)
```

**Done!** ✓ You're now logged in as admin.

---

## 📋 First Steps Checklist

- [ ] Database migrated
- [ ] Code compiled
- [ ] Logged in with STF001
- [ ] Password changed
- [ ] Created first staff account (STF002)
- [ ] Tested staff login
- [ ] Verified access restrictions

---

## 🎯 Most Common Tasks

### Create New Staff Account
```
Menu → User Management → Register New Staff
  Enter: Name, Email, Phone, Position
  Select: Role (ADMIN or STAFF)
  → System generates Staff ID (e.g., STF002)
  → Default password: admin
  → Staff must change on first login
```

### Change Your Password
```
During login:
  → If first login: Automatic password change dialog
  → If not first login: Contact admin to reset
```

### Deactivate Staff Account
```
Menu → User Management → Deactivate Staff Account
  Enter: Staff ID
  Confirm: Yes
  → Account status: INACTIVE
  → Staff cannot login
  → All data preserved
```

### Reactivate Staff Account
```
Menu → User Management → Reactivate Staff Account
  Enter: Staff ID
  Confirm: Yes
  → Account status: ACTIVE
  → Staff can login again
```

---

## 🔑 Login Guide

### Admin Login
```
Staff ID: STF001
Password: (your current password)
→ Full system access
→ Can manage staff, inventory, sales, suppliers
```

### Staff Login
```
Staff ID: STF002 (or any staff account)
Password: (their password)
→ Limited access
→ Can use: Dashboard, Inventory, Sales
→ Cannot use: User Management, Suppliers
```

---

## 📊 Staff ID Format

**Format**: `STF` + 3 digits

**Examples**:
```
STF001 ← First admin
STF002 ← Second staff
STF003 ← Third staff
STF100 ← 100th staff
```

**Rules**:
- Auto-generated (cannot edit)
- Unique (no duplicates)
- Used as login username

---

## ✅ Validation Rules

### Email
```
✓ Valid: user@company.com
✓ Valid: name.surname@domain.co.uk
✗ Invalid: usercompany.com (missing @)
✗ Invalid: user@.com (missing domain)
```

### Phone
```
✓ Valid: 0123456789 (10 digits)
✓ Valid: +60-12-3456789 (with country code)
✓ Valid: 011 2345 6789 (with spaces)
✗ Invalid: 123 (too short, less than 7 digits)
```

### Password
```
✓ Valid: MyPass123 (min 6 chars)
✓ Valid: Secure@2025 (with special chars)
✗ Invalid: pass (too short, less than 6 chars)
```

---

## 🎮 Menu Overview

### Admin Dashboard
```
1. Monitoring Dashboard
2. Inventory Management
3. User Management ← Admin features
4. Supplier Management
5. Process Sale
6. Logout
```

### Staff Dashboard
```
1. Monitoring Dashboard
2. Inventory Management
3. Process Sale
4. Logout
(No User Management or Suppliers)
```

---

## 🆘 Quick Troubleshooting

### Can't Login?
```
Check:
1. Staff ID correct (e.g., STF001)
2. Password correct
3. Account is ACTIVE (not INACTIVE)
4. Database is running
```

### Forgot Password?
```
Solution:
→ Contact admin
→ Admin uses: Update Staff → Reset Password
→ Password reset to "admin"
→ Must change on next login
```

### Email Already Exists?
```
Solution:
→ Email must be unique
→ Choose different email
→ Or contact admin to change existing
```

### Access Denied to User Management?
```
Check:
→ Are you STAFF user?
→ Only ADMIN can access User Management
→ Login as admin (STF001)
```

---

## 📝 Account Lifecycle

```
Account Created
     ↓
Password: admin
     ↓
First Login
     ↓
FORCE: Change Password
     ↓
Normal Operation
     ↓
Can: Login, Use System
     ↓
Admin: Can Deactivate
     ↓
INACTIVE: Cannot Login
     ↓
Admin: Can Reactivate
     ↓
Back to Normal Operation
```

---

## 🔐 Security Points

1. **Passwords are hashed** - Cannot be read from database
2. **Password input masked** - Shows asterisks, not characters
3. **First login change** - Force staff to set own password
4. **Account status control** - Disable without deleting data
5. **Role restrictions** - Different access per role
6. **Validation** - Email and phone checked before saving

---

## 📚 Need More Help?

| Need | File |
|------|------|
| Quick answers | USER_MANAGEMENT_QUICK_REFERENCE.md |
| Step-by-step guide | USER_MANAGEMENT_GUIDE.md |
| Technical details | USER_MANAGEMENT_IMPLEMENTATION.md |
| Setup instructions | IMPLEMENTATION_COMPLETE.md |
| Visual diagrams | VISUAL_IMPLEMENTATION_SUMMARY.md |
| All topics | DOCUMENTATION_INDEX.md |

---

## ⚙️ Settings Summary

### Default Admin
```
Staff ID: STF001
Name: System Administrator
Role: ADMIN
Email: admin@smartgrocery.com
Phone: +60-12-3456789
Password: admin (change on first login!)
Status: ACTIVE
```

### First New Staff
```
Staff ID: STF002 (auto-generated)
Name: (you enter)
Email: (you enter)
Phone: (you enter)
Position: (you enter)
Role: ADMIN or STAFF (you select)
Password: admin (auto-set, must change on login)
Status: ACTIVE
```

---

## 🎓 Learning Path

**First Day:**
1. Read: [README_USER_MANAGEMENT.md](README_USER_MANAGEMENT.md) ← You are here
2. Do: Complete 5-minute setup
3. Test: Login and create staff

**First Week:**
1. Use: [USER_MANAGEMENT_QUICK_REFERENCE.md](USER_MANAGEMENT_QUICK_REFERENCE.md)
2. Learn: Common tasks and menus

**As Needed:**
1. Refer: [USER_MANAGEMENT_GUIDE.md](USER_MANAGEMENT_GUIDE.md)
2. Get: Detailed help

---

## 💡 Pro Tips

### For Admins
- Save default password somewhere safe
- Test with staff account first
- Keep admin account secure
- Document staff account creation
- Regular backups recommended

### For Staff
- Change password immediately after first login
- Don't share your password
- Report account issues to admin
- Can only change own password

### For All
- Email should be real and accessible
- Phone should be your actual number
- Position should match your job title
- Role determines your system access

---

## ✨ What's New vs Old System

| Feature | Before | After |
|---------|--------|-------|
| Username | Generic text | Staff ID (STF001) |
| Auto ID | No | Yes ✓ |
| Full Name | No | Yes ✓ |
| Email | No | Yes ✓ |
| Phone | No | Yes ✓ |
| Position | No | Yes ✓ |
| Force Pwd Change | No | Yes ✓ |
| Disable Account | Delete only | Deactivate ✓ |
| Account Status | No | Yes ✓ |
| Reactivate | No | Yes ✓ |

**Result**: Professional staff management system! ✓

---

## 🎯 Your Next Step

**→ Read**: [DOCUMENTATION_INDEX.md](DOCUMENTATION_INDEX.md)

This will guide you to exactly what you need!

---

**Created**: January 4, 2025
**Version**: 2.0
**Status**: Ready to Use ✓
