# Purchase Order Module - Screen Flow

## Main Purchase Order Menu
```
========================================
      PURCHASE ORDER MANAGEMENT         
========================================

Operations:
1. Create New Purchase Order
2. View All Purchase Orders
3. View Purchase Order Details
4. Back to Previous Menu
========================================
Select option: _
```

---

## Screen 1: Create Purchase Order - Start
```
========================================
      CREATE NEW PURCHASE ORDER         
========================================

P.O. Code: PO-0001

=== AVAILABLE SUPPLIERS ===
==========================================================
ID   | Supplier Name                | Contact Person      
==========================================================
1    | Fresh Foods Distribution     | John Smith          
2    | Dairy Supplies Co.           | Mary Johnson        
3    | Meat & Poultry Wholesale     | Robert Lee          
==========================================================

Select Supplier ID (0 to go back): _
```

---

## Screen 2: Add Items
```
========================================
      CREATE NEW PURCHASE ORDER         
========================================
P.O. Code: PO-0001
Supplier: Fresh Foods Distribution
========================================

=== ITEMS FROM SELECTED SUPPLIER ===
=========================================================================
ID   | Item Name                    | Unit      | Cost Price | Stock    
=========================================================================
101  | Fresh Tomatoes              | kg        | RM 5.50    | 150.00   
102  | Green Lettuce               | kg        | RM 3.20    | 200.00   
103  | Red Apples                  | kg        | RM 6.80    | 180.00   
104  | Bananas                     | kg        | RM 4.50    | 250.00   
=========================================================================

=== ADD ITEM TO LIST ===
Enter Item ID (0 to finish adding items, -1 to go back): 101
Enter Quantity (in kg): 50

[OK] Item added to purchase order!
Press Enter to continue...
```

---

## Screen 3: Items Added - Continue
```
========================================
      CREATE NEW PURCHASE ORDER         
========================================
P.O. Code: PO-0001
Supplier: Fresh Foods Distribution
========================================

=== ITEMS IN PURCHASE ORDER ===
=====================================================================================
No. | Qty      | Unit      | Item Name                    | Cost      | Total      
=====================================================================================
1   | 50.00    | kg        | Fresh Tomatoes              | RM 5.50   | RM 275.00  
2   | 30.00    | kg        | Green Lettuce               | RM 3.20   | RM 96.00   
=====================================================================================

=== ITEMS FROM SELECTED SUPPLIER ===
=========================================================================
ID   | Item Name                    | Unit      | Cost Price | Stock    
=========================================================================
101  | Fresh Tomatoes              | kg        | RM 5.50    | 150.00   
102  | Green Lettuce               | kg        | RM 3.20    | 200.00   
103  | Red Apples                  | kg        | RM 6.80    | 180.00   
104  | Bananas                     | kg        | RM 4.50    | 250.00   
=========================================================================

=== ADD ITEM TO LIST ===
Enter Item ID (0 to finish adding items, -1 to go back): 0
```

---

## Screen 4: Enter Discount and Tax
```
========================================
      CREATE NEW PURCHASE ORDER         
========================================
P.O. Code: PO-0001
Supplier: Fresh Foods Distribution
========================================

=== ITEMS IN PURCHASE ORDER ===
=====================================================================================
No. | Qty      | Unit      | Item Name                    | Cost      | Total      
=====================================================================================
1   | 50.00    | kg        | Fresh Tomatoes              | RM 5.50   | RM 275.00  
2   | 30.00    | kg        | Green Lettuce               | RM 3.20   | RM 96.00   
=====================================================================================

=== DISCOUNT & TAX ===
Enter discount percentage (0 for no discount, -1 to cancel): 5
Enter tax percentage (0 for no tax, -1 to cancel): 6
```

---

## Screen 5: Review and Confirm
```
========================================
      CREATE NEW PURCHASE ORDER         
========================================
P.O. Code: PO-0001
Supplier: Fresh Foods Distribution
========================================

=== ITEMS IN PURCHASE ORDER ===
=====================================================================================
No. | Qty      | Unit      | Item Name                    | Cost      | Total      
=====================================================================================
1   | 50.00    | kg        | Fresh Tomatoes              | RM 5.50   | RM 275.00  
2   | 30.00    | kg        | Green Lettuce               | RM 3.20   | RM 96.00   
=====================================================================================

=== PURCHASE ORDER SUMMARY ===
-----------------------------------------------------
Sub Total:                                  RM 371.00
Discount (5%):                               RM 18.55
Tax (6%):                                     RM 21.15
-----------------------------------------------------
Total:                                      RM 373.60
-----------------------------------------------------

Enter remarks (optional, press Enter to skip): Urgent delivery needed

Do you want to save this purchase order? (Y/N, B to go back): Y

[OK] Purchase Order Created Successfully!
     P.O. Code: PO-0001
     Total Amount: RM373.60

Press Enter to continue...
```

---

## Screen 6: View All Purchase Orders
```
========================================
        VIEW ALL PURCHASE ORDERS        
========================================

=== LIST OF PURCHASE ORDERS ===
==================================================================================
PO Code  | Supplier                 | Total Amount | Status    | Order Date     
==================================================================================
PO-0003  | Meat & Poultry Wholesale | RM 1,245.00 | Pending   | 2026-01-06     
PO-0002  | Dairy Supplies Co.       | RM 856.50   | Pending   | 2026-01-05     
PO-0001  | Fresh Foods Distribution | RM 373.60   | Pending   | 2026-01-06     
==================================================================================
Showing 1 to 3 of 3 entries

Press Enter to continue...
```

---

## Screen 7: View Purchase Order Details
```
========================================
      VIEW PURCHASE ORDER DETAILS       
========================================

Enter P.O. Code (e.g., PO-0001): PO-0001

=== PURCHASE ORDER DETAILS ===
========================================
P.O. Code: PO-0001
Supplier: Fresh Foods Distribution
Contact Person: John Smith
Status: Pending
Order Date: 2026-01-06 14:35:22
========================================

=== ITEMS ===
=====================================================================================
Item Name                    | Qty      | Unit      | Unit Price | Total Price   
=====================================================================================
Fresh Tomatoes              | 50.00    | kg        | RM 5.50    | RM 275.00     
Green Lettuce               | 30.00    | kg        | RM 3.20    | RM 96.00      
=====================================================================================

=== SUMMARY ===
-----------------------------------------------------
Sub Total:                              RM 371.00
Discount (5%):                            RM 18.55
Tax (6%):                                 RM 21.15
-----------------------------------------------------
Total:                                  RM 373.60
-----------------------------------------------------

Remarks: Urgent delivery needed

Press Enter to continue...
```

---

## Navigation Flow Diagram

```
Main Menu
    ↓
Purchase Order Menu
    ↓
Create New Purchase Order
    ↓
[PO-0001] Auto-generated
    ↓
Select Supplier
    ↓ (0 = go back)
Add Items Loop
    ├─ View Items by Supplier
    ├─ Enter Item ID
    ├─ Enter Quantity
    ├─ Add to List
    └─ (0 = finish, -1 = cancel)
    ↓
Enter Discount % (optional)
    ↓ (-1 = cancel)
Enter Tax % (optional)
    ↓ (-1 = cancel)
Review Summary
    ├─ Items List
    ├─ Calculations
    └─ Enter Remarks
    ↓
Confirm Save
    ├─ Y = Save to Database
    ├─ N = Cancel
    └─ B = Go Back
    ↓
Success Message
    ↓
Return to Menu
```

---

## Key Input Options at Each Step

| Step | Valid Inputs | Action |
|------|--------------|--------|
| Supplier Selection | Number | Select supplier by ID |
| | 0 | Go back to menu |
| Item Addition | Number | Select item by ID |
| | 0 | Finish adding items |
| | -1 | Cancel purchase order |
| Quantity | Number > 0 | Enter quantity |
| Discount | 0-100 | Discount percentage |
| | 0 | No discount |
| | -1 | Cancel purchase order |
| Tax | 0-100 | Tax percentage |
| | 0 | No tax |
| | -1 | Cancel purchase order |
| Confirm Save | Y | Save purchase order |
| | N | Cancel without saving |
| | B | Go back to review |

---

## Color Coding (Conceptual)

- **Headers**: Blue background
- **Success Messages**: Green with [OK]
- **Error Messages**: Red with [ERROR]
- **Info Messages**: Yellow with [INFO]
- **Table Headers**: Dark blue
- **Input Prompts**: Cyan
- **Summary Sections**: Light gray background

---

## Example Calculation Flow

```
Items:
  Item 1: 50 kg × RM 5.50 = RM 275.00
  Item 2: 30 kg × RM 3.20 = RM 96.00
  
Sub Total:              RM 371.00

Discount 5%:
  371.00 × 5 ÷ 100 =    RM 18.55

After Discount:
  371.00 - 18.55 =      RM 352.45

Tax 6%:
  352.45 × 6 ÷ 100 =    RM 21.15

TOTAL:
  352.45 + 21.15 =      RM 373.60
```

---

## User Experience Highlights

✅ **Clean Layout**: Well-organized information
✅ **Clear Instructions**: Step-by-step guidance
✅ **Visual Feedback**: Success/error messages
✅ **Easy Navigation**: Back options at each step
✅ **Data Validation**: Input checking
✅ **Auto-calculations**: No manual math needed
✅ **Summary Review**: See everything before saving
✅ **Professional Tables**: Aligned columns with borders

---

*These screens show the text-based console interface. All formatting uses standard ASCII characters for maximum compatibility.*
