# Unit Feature Implementation

## Overview
The Product module now supports 17 different unit types for flexible inventory management. Units are attached to selling prices and stock quantities, allowing precise tracking of products measured in various ways.

## Available Units
1. **kg** (Kilogram) - For heavy items
2. **g** (Gram) - For lightweight items
3. **litre** - For liquid volumes
4. **ml** (Milliliter) - For small liquid amounts
5. **piece/pcs** - For individual items
6. **bunch** - For bundled items (e.g., bananas)
7. **bag** - For bagged products
8. **tray/container** - For packaged items
9. **cup/tub/pack** - For cup or tub packaging
10. **dozen** - For 12-item groups
11. **loaf** - For bread products
12. **pack/packet** - For packaged items
13. **can** - For canned goods
14. **box** - For boxed products
15. **bottle** - For bottled items
16. **jar** - For jarred products
17. **tube** - For tube packaging

## Features

### 1. Add Product with Unit
When adding a new product:
- Select from 13 product categories
- Select from 17 unit types
- Enter **Sell Price per [unit]** (e.g., RM5.50 per kg)
- Enter **Stock Quantity in [unit]** (e.g., 120 kg)
- Unit information is stored in the database

**Example Workflow:**
```
Product Name: Jasmine Rice
Category: 1 (Fresh Produce)
Unit: 1 (kg)
Cost Price: RM4.50
Sell Price per kg: RM5.50
Stock Quantity (in kg): 120
Supplier: Select from list
```

### 2. View Inventory with Units
The inventory list displays:
- **Sell Price/Unit**: Shows price with unit (e.g., RM5.50/kg)
- **Stock/Unit**: Shows quantity with unit (e.g., 120 kg)

**Table Format:**
```
ID | Item Name      | Category        | Cost(RM) | Sell Price/Unit | Stock/Unit | Supplier
1  | Jasmine Rice   | Fresh Produce   | RM4.50   | RM5.50/kg       | 120 kg     | ABC Supply
2  | Olive Oil      | Pantry Staples  | RM25.00  | RM28.00/litre   | 15 litre   | XYZ Foods
```

### 3. View Product Details with Unit
Product details view shows:
- **Unit**: The selected unit type
- **Sell Price**: RM[amount] per [unit]
- **Stock Quantity**: [amount] [unit]

**Example:**
```
Product ID:       1
Item Name:        Jasmine Rice
Category:         Fresh Produce
Unit:             kg
Cost Price:       RM4.50
Sell Price:       RM5.50 per kg
Stock Quantity:   120 kg
Supplier:         ABC Supply
Date Created:     2025-01-11 00:00:00
```

### 4. Update Product Unit
When updating a product, you can now change:
1. Product Name
2. Category
3. **Unit** (NEW - select from 17 options)
4. Cost Price
5. Sell Price
6. Stock Quantity
7. Supplier

## Database Schema

### Product Table Structure
```sql
product_id       INT(11) AUTO_INCREMENT PRIMARY KEY
name             VARCHAR(100)
category         VARCHAR(100) DEFAULT 'Others'
unit             VARCHAR(50) DEFAULT 'piece/pcs'
cost_price       DECIMAL(10,2)
sell_price       DECIMAL(10,2)
stock_quantity   DECIMAL(10,2)  -- Changed to support decimal for units like kg, g, litre
supplier_id      INT(11)
date_created     TIMESTAMP
```

## Migration Instructions

To add the unit column to existing databases:

1. Open MySQL command line or phpMyAdmin
2. Run the script: `scripts/update_product_schema.sql`

Or manually execute:
```sql
USE smart_grocery;

ALTER TABLE Product 
ADD COLUMN IF NOT EXISTS unit VARCHAR(50) DEFAULT 'piece/pcs' AFTER category;

UPDATE Product SET unit = 'piece/pcs' WHERE unit IS NULL OR unit = '';
```

## Use Cases

### Example 1: Fresh Produce
- **Product**: Jasmine Rice
- **Unit**: kg
- **Sell Price**: RM5.50 per kg
- **Stock**: 120 kg

### Example 2: Beverages
- **Product**: Olive Oil
- **Unit**: litre
- **Sell Price**: RM28.00 per litre
- **Stock**: 15 litre

### Example 3: Packaged Goods
- **Product**: Eggs
- **Unit**: dozen
- **Sell Price**: RM8.50 per dozen
- **Stock**: 30 dozen

### Example 4: Canned Goods
- **Product**: Tomato Paste
- **Unit**: can
- **Sell Price**: RM3.20 per can
- **Stock**: 50 can

## Benefits

1. **Accurate Pricing**: Per-unit pricing ensures correct calculations
2. **Flexible Inventory**: Support for various measurement types
3. **Clear Display**: Units shown alongside prices and stock
4. **Easy Updates**: Can change unit type if needed
5. **Professional**: Industry-standard unit selections

## Implementation Details

### Helper Functions
- `displayUnits()`: Shows all 17 unit options with descriptions
- `getUnitName(int)`: Converts user choice (1-17) to unit string

### Modified Functions
- `addProduct()`: Includes unit selection workflow
- `viewInventory()`: Displays units with prices and stock
- `viewProduct()`: Shows unit in product details
- `updateProduct()`: Allows unit changes (option 3)

## Compilation
```bash
C:\mingw64\bin\g++ -o build\main.exe source\*.cpp ^
  -I"headers" ^
  -I"C:/Program Files/MySQL/MySQL Server 9.5/include" ^
  -L"C:/Program Files/MySQL/MySQL Server 9.5/lib" ^
  -lmysql
```

**Compiled Size**: 259 KB  
**Last Compiled**: 2025-01-11 00:41:48

## Version History
- **v1.0** (2025-01-11): Initial unit feature implementation
  - 17 unit types
  - Per-unit pricing and stock
  - Unit display in all views
  - Unit update capability
