# Purchase Order Module - User Guide

## Overview
The Purchase Order module allows administrators and staff to create, view, and manage purchase orders for ordering inventory from suppliers.

## Features

### 1. Auto-Generated P.O. Code
- Format: `PO-0001`, `PO-0002`, etc.
- Automatically increments with each new purchase order
- Unique identifier for tracking orders

### 2. Supplier Selection
- Select from active suppliers only
- View supplier details including contact person
- Items filtered based on selected supplier

### 3. Item Management
- View all items/products from selected supplier
- See current stock levels and cost prices
- Add multiple items to a single purchase order

### 4. Unit and Quantity
- Each item displays its unit (kg, g, litre, etc.)
- Enter quantity in numeric format only
- Automatic cost calculation (quantity × unit price)

### 5. Discount and Tax (Optional)
- Enter discount as a percentage (0-100%)
- Enter tax as a percentage (0-100%)
- Option to skip discount/tax by entering 0
- Automatic calculation of final total

### 6. Navigation Options
- Go back at any step during creation
- Cancel purchase order creation before saving
- Return to previous menu

## How to Use

### Creating a New Purchase Order

1. **Access the Module**
   - From main menu, select "Purchase Order"
   - Choose option 1: "Create New Purchase Order"

2. **P.O. Code**
   - System automatically generates the P.O. Code
   - Example: `PO-0001`

3. **Select Supplier**
   - View list of active suppliers with ID, name, and contact person
   - Enter the Supplier ID
   - Enter `0` to go back

4. **Add Items to Order**
   - View all items from the selected supplier
   - For each item:
     - Enter Item ID (or `0` to finish adding items, `-1` to cancel)
     - Enter Quantity in numbers only
     - Item is added to the purchase order list
   - Repeat until all items are added

5. **Enter Discount and Tax (Optional)**
   - Enter discount percentage (0-100%, `-1` to cancel)
   - Enter tax percentage (0-100%, `-1` to cancel)
   - Enter `0` for either if not applicable

6. **Review and Save**
   - Review the complete purchase order summary
   - Items list with quantities and costs
   - Sub Total, Discount, Tax, and Total Amount
   - Enter remarks (optional)
   - Confirm save: `Y` to save, `N` to cancel, `B` to go back

### Viewing Purchase Orders

#### View All Purchase Orders
- Select option 2: "View All Purchase Orders"
- Displays table with:
  - P.O. Code
  - Supplier Name
  - Total Amount
  - Status (Pending, Approved, Received, Cancelled)
  - Order Date

#### View Purchase Order Details
- Select option 3: "View Purchase Order Details"
- Enter P.O. Code (e.g., `PO-0001`)
- Displays:
  - Complete order information
  - All items with quantities and prices
  - Sub total, discount, tax calculations
  - Final total amount
  - Remarks (if any)

## Purchase Order Flow

```
1. P.O. Code Auto-Generated
           ↓
2. Select Supplier (Active suppliers only)
           ↓
3. Add Items (from selected supplier)
   - Select Item ID
   - Enter Quantity
   - Add to List
   - Repeat or finish
           ↓
4. Enter Discount % (optional - can enter 0)
           ↓
5. Enter Tax % (optional - can enter 0)
           ↓
6. Review Summary
           ↓
7. Enter Remarks (optional)
           ↓
8. Confirm and Save
```

## Database Setup

Before using the Purchase Order module, you must set up the database tables:

1. Open MySQL command line or MySQL Workbench
2. Navigate to your project's scripts folder
3. Run the SQL migration script:
   ```sql
   source scripts/create_purchase_order_tables.sql
   ```
   Or manually execute the SQL file in MySQL Workbench

This creates two tables:
- `PurchaseOrder`: Stores purchase order header information
- `PurchaseOrderItem`: Stores individual items in each purchase order

## Menu Options

### Main Purchase Order Menu
1. **Create New Purchase Order** - Create a new P.O. with items
2. **View All Purchase Orders** - List all purchase orders
3. **View Purchase Order Details** - View specific P.O. by code
4. **Back to Previous Menu** - Return to main dashboard

## Access Control
- **Admin**: Full access to Purchase Order module
- **Staff**: Full access to Purchase Order module

Both roles can create and view purchase orders.

## Tips

### Best Practices
1. Always verify supplier is correct before adding items
2. Double-check quantities before adding to list
3. Review the summary carefully before saving
4. Use remarks field for special instructions or notes
5. Keep P.O. codes for reference when tracking orders

### Common Scenarios

**Scenario 1: No Discount or Tax**
- Enter `0` for both discount and tax percentage
- Final total equals sub total

**Scenario 2: With Discount Only**
- Enter discount percentage (e.g., 5 for 5%)
- Enter `0` for tax
- Total = Sub Total - Discount

**Scenario 3: With Tax Only**
- Enter `0` for discount
- Enter tax percentage (e.g., 8 for 8%)
- Total = Sub Total + Tax

**Scenario 4: With Both Discount and Tax**
- Enter discount percentage first
- Enter tax percentage
- Total = (Sub Total - Discount) + Tax

### Cancel or Go Back Options
- **During Supplier Selection**: Enter `0`
- **During Item Addition**: Enter `-1` to cancel, `0` to finish
- **During Discount/Tax**: Enter `-1` to cancel
- **Before Saving**: Select `N` to cancel, `B` to go back

## Troubleshooting

### Issue: "Supplier ID not found or not active"
**Solution**: Ensure you're entering a valid supplier ID from the displayed list

### Issue: "Item not found"
**Solution**: Make sure the item ID exists in the system and belongs to the selected supplier

### Issue: "Cannot create purchase order without items"
**Solution**: Add at least one item before proceeding to discount/tax entry

### Issue: "This item does not belong to the selected supplier"
**Solution**: Select items only from the displayed list for the chosen supplier

## Compilation

The Purchase Order module files are included in all compilation scripts:
- `compile.bat` (main compile script)
- `scripts/compile.bat`
- `scripts/compile-mingw64.bat`
- `Makefile`

Simply run your preferred compilation script:
```bash
compile.bat
```

## Support

For issues or questions:
1. Check this user guide
2. Review the troubleshooting section
3. Verify database tables are created correctly
4. Ensure supplier and product data exists before creating purchase orders

---

**Module Version**: 1.0  
**Last Updated**: January 2026  
**Compatible With**: Smart Grocery Management System v1.0+
