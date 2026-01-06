-- Migration script to add address column to Supplier table
-- Run this script in MySQL to update the database schema

USE grocery_db;

-- Add address column after supplier_name if it doesn't exist
ALTER TABLE Supplier 
ADD COLUMN IF NOT EXISTS address VARCHAR(255) DEFAULT '' AFTER supplier_name;

-- Update existing records with empty address (optional)
UPDATE Supplier 
SET address = 'Not specified' 
WHERE address = '' OR address IS NULL;

-- Verify the changes
DESCRIBE Supplier;

SELECT 'Migration completed successfully!' AS Status;
