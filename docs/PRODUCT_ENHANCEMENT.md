# Product Module Enhancement - Documentation

## Overview
The Product/Inventory Management module has been significantly enhanced with new features including product categories, unit management, per-unit calculations, and auto-calculation of packs based on unit size.

## New Features

### 1. **Product Categories**
Users can now select from 13 predefined categories when adding products:
1. Fresh Produce (Fruits & Vegetables)
2. Meat, Poultry, and Seafood
3. Dairy Products
4. Bakery
5. Beverages
6. Canned Goods
7. Dry Goods & Grains
8. Snacks & Confectionery
9. Frozen Foods
10. Condiments & Sauces
11. Personal Care
12. Household Items
13. Others

### 2. **Unit Management**
Users can select from 17 different unit types:
1. kg (Kilogram)
2. g (Gram)
3. litre (Liter)
4. ml (Milliliter)
5. piece/pcs
6. bunch
7. bag
8. tray/container
9. cup/tub/pack
10. dozen
11. loaf
12. pack/packet
13. can
14. box
15. bottle
16. jar
17. tube

### 3. **Per-Unit Guideline & Auto-Pack Calculation**
**Example:** 
- Product: Jasmine Rice
- Unit: kg
- Unit Size per Pack: 5 kg
- Total Stock: 120 kg
- **Auto-calculated Result:** 24 packs (with 0 kg remaining)

The system automatically calculates:
- Total number of complete packs
- Remaining units that don't make a complete pack

### 4. **Enhanced Add Product Workflow**
When adding a new product, users now input:
1. Product Name
2. Category (from dropdown)
3. Unit Type (from dropdown)
4. Unit Size per Pack (e.g., 5 kg/pack)
5. Total Stock in Units (e.g., 120 kg)
6. Cost Price per Pack (RM)
7. Sell Price per Unit (RM)
8. Supplier Selection

The system then displays:
- Total Packs Available
- Remaining units

### 5. **Reorganized Table View**
The inventory list now follows this sequence:
- **ID** - Product ID
- **Item Name** - Product name
- **Category** - Product category
- **Cost (RM)** - Cost price per pack
- **Sell Price (per unit)** - Selling price per unit with unit name
- **Stock (per unit)** - Total stock with unit
- **Packs** - Auto-calculated total packs
- **Supplier Name** - Supplier name

### 6. **Enhanced Product Details View**
Individual product view now shows:
- Product ID
- Item Name
- Category
- Unit
- Unit Size (per pack)
- Cost Price (per pack)
- Sell Price (per unit)
- Total Stock (in units)
- Total Packs (auto-calculated)
- Remaining units
- Supplier Name
- Date Created

### 7. **Updated Product Update Menu**
Update menu now includes:
1. Product Name
2. Category
3. Unit & Unit Size
4. Cost Price
5. Sell Price
6. Stock Quantity
7. Supplier

## Database Schema Changes

### Product Table - New Columns:
```sql
category VARCHAR(100) DEFAULT 'Others'
unit VARCHAR(50) DEFAULT 'piece/pcs'
unit_size DECIMAL(10,2) DEFAULT 1.00
```

### Migration Script:
Location: `scripts/update_product_schema.sql`

To apply changes:
1. Open XAMPP phpMyAdmin
2. Select `smart_grocery` database
3. Go to SQL tab
4. Execute the script, OR
5. Run via MySQL command line:
   ```bash
   mysql -u root -p smart_grocery < scripts/update_product_schema.sql
   ```

## Code Changes

### Modified Files:
1. **source/Product.cpp**
   - Added `displayCategories()` function
   - Added `displayUnits()` function
   - Added `getCategoryName()` function
   - Added `getUnitName()` function
   - Completely rewrote `addProduct()` with new fields
   - Updated `viewInventory()` with new table format
   - Updated `viewProduct()` with new detailed view
   - Enhanced `updateProduct()` with category and unit options

2. **scripts/update_product_schema.sql**
   - New SQL migration script for database schema updates

## Usage Examples

### Example 1: Adding Rice Product
```
Product Name: Jasmine Rice Premium Grade
Category: 7 (Dry Goods & Grains)
Unit: 1 (kg)
Unit Size per Pack: 5 kg
Total Stock: 120 kg
→ Result: 24 packs

Cost Price per Pack: RM25.00
Sell Price per kg: RM6.50
Supplier: ABC Suppliers
```

### Example 2: Adding Beverage
```
Product Name: Orange Juice Fresh
Category: 5 (Beverages)
Unit: 3 (litre)
Unit Size per Pack: 2 litre
Total Stock: 50 litre
→ Result: 25 packs

Cost Price per Pack: RM8.00
Sell Price per litre: RM5.00
Supplier: Fresh Drinks Co
```

### Example 3: Adding Meat
```
Product Name: Chicken Breast
Category: 2 (Meat, Poultry, and Seafood)
Unit: 1 (kg)
Unit Size per Pack: 1 kg
Total Stock: 35 kg
→ Result: 35 packs

Cost Price per Pack: RM12.00
Sell Price per kg: RM18.00
Supplier: Fresh Meat Supply
```

## Benefits

1. **Better Organization:** Products are now categorized for easier management
2. **Flexible Units:** Support for various measurement units
3. **Accurate Inventory:** Auto-calculation helps track pack quantities
4. **Clear Pricing:** Separate cost (per pack) and sell price (per unit)
5. **Improved Display:** Table view shows all essential information at a glance
6. **Enhanced Updates:** More comprehensive update options

## Notes

- Existing products in the database will have default values:
  - Category: "Others"
  - Unit: "piece/pcs"
  - Unit Size: 1.00
  
- You can update existing products to set proper categories and units using the Update Product function

- The pack calculation uses integer division, so partial packs are shown as "Remaining" units

- All prices are in Malaysian Ringgit (RM)

## Compilation

The project has been successfully compiled with the new features.
Build location: `build/main.exe`
Build size: ~248 KB
Last compiled: 8/12/2025 5:33:47 PM
