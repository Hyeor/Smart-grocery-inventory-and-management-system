-- SQL Script to add and backfill barcode column for Product table
-- Run this script in the smart_grocery database before using the barcode feature

USE smart_grocery;

-- Add barcode column to Product table
ALTER TABLE Product
ADD COLUMN IF NOT EXISTS barcode VARCHAR(32) NOT NULL DEFAULT '' AFTER product_id;

-- Backfill barcodes for existing rows using product_id to keep uniqueness and readability
UPDATE Product
SET barcode = CONCAT('88', LPAD(product_id, 10, '0'))
WHERE barcode IS NULL OR barcode = '';

-- Ensure barcode uniqueness
ALTER TABLE Product
ADD UNIQUE INDEX IF NOT EXISTS idx_product_barcode (barcode);

-- Preview updated data
SELECT product_id, barcode, name FROM Product ORDER BY product_id;
