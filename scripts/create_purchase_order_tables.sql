-- ========================================
-- SQL Migration: Purchase Order Tables
-- ========================================
-- This script creates the necessary tables for the Purchase Order module
-- Run this script to add Purchase Order functionality to your database

-- Create PurchaseOrder table
CREATE TABLE IF NOT EXISTS PurchaseOrder (
    po_id INT AUTO_INCREMENT PRIMARY KEY,
    po_code VARCHAR(20) UNIQUE NOT NULL,
    supplier_id INT NOT NULL,
    sub_total DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    discount_percent DECIMAL(5, 2) NOT NULL DEFAULT 0.00,
    discount_amount DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    tax_percent DECIMAL(5, 2) NOT NULL DEFAULT 0.00,
    tax_amount DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    total_amount DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    remarks TEXT,
    status ENUM('Pending', 'Approved', 'Received', 'Cancelled') DEFAULT 'Pending',
    order_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    received_date TIMESTAMP NULL,
    FOREIGN KEY (supplier_id) REFERENCES Supplier(supplier_id) ON DELETE RESTRICT,
    INDEX idx_po_code (po_code),
    INDEX idx_supplier (supplier_id),
    INDEX idx_status (status),
    INDEX idx_order_date (order_date)
);

-- Create PurchaseOrderItem table (junction table for PO and Products)
CREATE TABLE IF NOT EXISTS PurchaseOrderItem (
    po_item_id INT AUTO_INCREMENT PRIMARY KEY,
    po_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity DECIMAL(10, 2) NOT NULL,
    unit_price DECIMAL(10, 2) NOT NULL,
    total_price DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (po_id) REFERENCES PurchaseOrder(po_id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES Product(product_id) ON DELETE RESTRICT,
    INDEX idx_po_id (po_id),
    INDEX idx_product_id (product_id)
);

-- ========================================
-- Sample data for testing (optional)
-- ========================================
-- Uncomment the following lines to insert sample data

-- INSERT INTO PurchaseOrder (po_code, supplier_id, sub_total, discount_percent, discount_amount, tax_percent, tax_amount, total_amount, remarks, status)
-- VALUES 
-- ('PO-0001', 1, 1000.00, 5.00, 50.00, 8.00, 76.00, 1026.00, 'Sample purchase order for testing', 'Pending');

-- INSERT INTO PurchaseOrderItem (po_id, product_id, quantity, unit_price, total_price)
-- VALUES
-- (1, 1, 5.00, 200.00, 1000.00);

-- ========================================
-- Verification Queries
-- ========================================
-- Run these queries to verify the tables were created successfully

-- Show PurchaseOrder table structure
-- DESCRIBE PurchaseOrder;

-- Show PurchaseOrderItem table structure
-- DESCRIBE PurchaseOrderItem;

-- Count records in PurchaseOrder table
-- SELECT COUNT(*) as total_orders FROM PurchaseOrder;

-- Count records in PurchaseOrderItem table
-- SELECT COUNT(*) as total_items FROM PurchaseOrderItem;
