# 🛒 Purchase Order Module - Quick Start

## ⚡ Quick Setup (3 Steps)

### Step 1: Create Database Tables
Open MySQL command line or MySQL Workbench and run:

```sql
USE smart_grocery;
source scripts/create_purchase_order_tables.sql;
```

Or in MySQL Workbench:
1. Open `scripts/create_purchase_order_tables.sql`
2. Execute the script

### Step 2: Compile Project
Run the compile script:
```bash
.\compile.bat
```

### Step 3: Run Application
```bash
.\main.exe
```

## 📋 How to Create a Purchase Order

1. Login as Admin or Staff
2. Select "Purchase Order" from main menu
3. Select "Create New Purchase Order"
4. Follow the prompts:
   - P.O. Code is auto-generated (e.g., PO-0001)
   - Select Supplier by ID
   - Add items:
     * Enter Item ID
     * Enter Quantity
     * Press 0 when done
   - Enter Discount % (or 0 to skip)
   - Enter Tax % (or 0 to skip)
   - Review summary
   - Enter remarks (optional)
   - Press Y to save

## 🎯 Key Features

✅ **Auto P.O. Code**: Automatically generates PO-0001, PO-0002, etc.
✅ **Supplier Filter**: Items filtered by selected supplier
✅ **Multiple Items**: Add as many items as needed
✅ **Smart Calculations**: Auto-calculates subtotal, discount, tax, and total
✅ **Optional Fields**: Discount, tax, and remarks are all optional
✅ **Navigation**: Go back at any step with 0, -1, or B

## 📁 New Files

```
headers/
  └─ PurchaseOrder.h          # Header file

source/
  └─ PurchaseOrder.cpp        # Implementation

scripts/
  ├─ create_purchase_order_tables.sql       # Database setup
  └─ verify_purchase_order_setup.sql        # Verification

docs/
  ├─ PURCHASE_ORDER_GUIDE.md               # User guide
  └─ PURCHASE_ORDER_IMPLEMENTATION.md      # Technical docs
```

## 🔍 Verify Setup

Run this in MySQL to verify tables were created:

```sql
USE smart_grocery;
source scripts/verify_purchase_order_setup.sql;
```

## 📊 Menu Changes

### Admin Dashboard (Updated)
```
1. Monitoring Dashboard
2. Inventory Management
3. User Management
4. Supplier Management
5. Purchase Order          ← NEW!
6. Process Sale
7. Logout
```

### Staff Dashboard (Updated)
```
1. Monitoring Dashboard
2. Inventory Management
3. Supplier Management
4. Purchase Order          ← NEW!
5. Process Sale
6. Logout
```

## ⚠️ Important Notes

1. **Database Required**: Run the SQL script before using the module
2. **Active Suppliers**: Only active suppliers appear in selection
3. **Supplier Items**: Only items from selected supplier can be added
4. **At Least One Item**: Must add at least one item to save P.O.
5. **Numeric Quantity**: Quantity must be numbers only

## 🆘 Troubleshooting

**Problem**: "Supplier ID not found"
**Solution**: Use ID from the displayed supplier list

**Problem**: "Item does not belong to selected supplier"
**Solution**: Only select items from the displayed list after choosing supplier

**Problem**: "Cannot create purchase order without items"
**Solution**: Add at least one item before proceeding

**Problem**: Tables don't exist
**Solution**: Run `scripts/create_purchase_order_tables.sql` in MySQL

## 📚 Documentation

- **User Guide**: `docs/PURCHASE_ORDER_GUIDE.md`
- **Implementation Details**: `docs/PURCHASE_ORDER_IMPLEMENTATION.md`

## ✅ Status

- [x] Database schema created
- [x] Header and source files implemented
- [x] Integrated into main menu
- [x] Compilation scripts updated
- [x] Documentation completed
- [x] Successfully compiled

**Ready to Use!** 🎉

---

**Need Help?** Check `docs/PURCHASE_ORDER_GUIDE.md` for detailed instructions.
