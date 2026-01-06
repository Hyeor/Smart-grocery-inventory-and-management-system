-- SQL Migration Script: Update User Table Schema
-- Purpose: Modify User table to support new staff management requirements
-- Run this script in MySQL to upgrade the User table

USE smart_grocery;

-- Disable foreign key checks to allow dropping User table
SET FOREIGN_KEY_CHECKS=0;

-- Step 1: Create backup of existing User table (optional but recommended)
-- Uncomment if you want to backup existing data
-- CREATE TABLE User_Backup AS SELECT * FROM User;

-- Step 2: Drop the existing User table and recreate with new schema
-- WARNING: This will delete existing user data. Backup first if needed!
DROP TABLE IF EXISTS User;

-- Step 3: Create new User table with enhanced staff management features
CREATE TABLE User (
    user_id INT PRIMARY KEY AUTO_INCREMENT COMMENT 'Auto-increment ID for database',
    staff_id VARCHAR(10) UNIQUE NOT NULL COMMENT 'Auto-generated Staff ID (STF001, STF002, etc.)',
    full_name VARCHAR(255) NOT NULL COMMENT 'Full name of staff (max 30 words)',
    email VARCHAR(100) NOT NULL UNIQUE COMMENT 'Email address of staff',
    phone VARCHAR(20) NOT NULL COMMENT 'Phone number of staff',
    position VARCHAR(100) NOT NULL COMMENT 'Position/Job Title (e.g., Admin, Manager, Clerk)',
    role ENUM('ADMIN', 'STAFF') NOT NULL DEFAULT 'STAFF' COMMENT 'Role/Access Level: ADMIN or STAFF',
    password VARCHAR(255) NOT NULL COMMENT 'Hashed password (MD5)',
    account_status ENUM('ACTIVE', 'INACTIVE') NOT NULL DEFAULT 'ACTIVE' COMMENT 'Account status control',
    password_change_required TINYINT(1) NOT NULL DEFAULT 1 COMMENT 'Flag to force password change on first login',
    date_created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'Account creation date and time',
    
    INDEX idx_staff_id (staff_id),
    INDEX idx_email (email),
    INDEX idx_account_status (account_status),
    INDEX idx_role (role)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Staff user management table with role-based access control';

-- Step 4: Insert default admin account
-- Default credentials:
--   Staff ID: admin
--   Password: admin123 (hashed as MD5)
--   Must change password on first login
INSERT INTO User (staff_id, full_name, email, phone, position, role, password, account_status, password_change_required, date_created) 
VALUES (
    'admin',
    'System Administrator',
    'admin@smartgrocery.com',
    '+60-12-3456789',
    'Administrator',
    'ADMIN',
    MD5('admin123'),
    'ACTIVE',
    1,
    NOW()
);

-- Re-enable foreign key checks
SET FOREIGN_KEY_CHECKS=1;

-- Step 5: Verify the schema and data
DESCRIBE User;
SELECT user_id, staff_id, full_name, email, position, role, account_status, date_created FROM User;

-- Migration completed successfully!
-- Staff can now login using Staff ID (e.g., STF001) and default password 'admin'
-- They will be prompted to change their password on first login
