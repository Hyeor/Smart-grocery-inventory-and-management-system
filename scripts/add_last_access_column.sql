-- Migration script to add last_access column to User table
-- Run this script in MySQL to track last login time

USE smart_grocery;

-- Add last_access column to track when user last logged in
ALTER TABLE User 
ADD COLUMN IF NOT EXISTS last_access DATETIME DEFAULT NULL AFTER date_created;

-- Verify the changes
DESCRIBE User;

SELECT 'Migration completed successfully! last_access column added.' AS Status;
