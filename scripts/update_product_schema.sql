-- SQL Script to add category and unit columns to Product table
-- Run this in your MySQL database (XAMPP phpMyAdmin or MySQL command line)

USE smart_grocery;

-- Add category column to Product table
ALTER TABLE Product 
ADD COLUMN IF NOT EXISTS category VARCHAR(100) DEFAULT 'Others' AFTER name;

-- Add unit column to Product table
ALTER TABLE Product 
ADD COLUMN IF NOT EXISTS unit VARCHAR(50) DEFAULT 'piece/pcs' AFTER category;

-- Update existing products with default values if needed
UPDATE Product SET category = 'Others' WHERE category IS NULL OR category = '';
UPDATE Product SET unit = 'piece/pcs' WHERE unit IS NULL OR unit = '';

-- Display the updated table structure
DESCRIBE Product;

-- Display current products with new columns
SELECT product_id, name, category, unit, cost_price, sell_price, stock_quantity FROM Product;
