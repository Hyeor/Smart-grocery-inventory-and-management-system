-- Add timestamp column to Sales table for monthly report filtering
-- Run this script in phpMyAdmin or MySQL command line

USE smart_grocery;

-- Add sale_date column with default timestamp
ALTER TABLE Sales 
ADD COLUMN sale_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP;

-- Update existing records to have current timestamp
UPDATE Sales SET sale_date = CURRENT_TIMESTAMP WHERE sale_date IS NULL;

-- Verify the change
DESCRIBE Sales;

SELECT 'Sales table updated successfully with sale_date column!' AS Status;
