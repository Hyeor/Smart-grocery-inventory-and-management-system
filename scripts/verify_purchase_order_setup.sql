-- ========================================
-- Quick Database Setup Verification
-- ========================================
-- Run this script to verify Purchase Order tables are set up correctly

-- 1. Check if tables exist
SELECT 
    TABLE_NAME, 
    TABLE_ROWS,
    CREATE_TIME
FROM 
    information_schema.TABLES 
WHERE 
    TABLE_SCHEMA = DATABASE()
    AND TABLE_NAME IN ('PurchaseOrder', 'PurchaseOrderItem')
ORDER BY 
    TABLE_NAME;

-- 2. If tables don't exist, run the creation script first
-- source scripts/create_purchase_order_tables.sql;

-- 3. Verify PurchaseOrder table structure
DESCRIBE PurchaseOrder;

-- 4. Verify PurchaseOrderItem table structure
DESCRIBE PurchaseOrderItem;

-- 5. Check foreign key relationships
SELECT 
    TABLE_NAME,
    COLUMN_NAME,
    CONSTRAINT_NAME,
    REFERENCED_TABLE_NAME,
    REFERENCED_COLUMN_NAME
FROM
    information_schema.KEY_COLUMN_USAGE
WHERE
    TABLE_SCHEMA = DATABASE()
    AND TABLE_NAME IN ('PurchaseOrder', 'PurchaseOrderItem')
    AND REFERENCED_TABLE_NAME IS NOT NULL;

-- 6. Check indexes
SELECT 
    TABLE_NAME,
    INDEX_NAME,
    COLUMN_NAME,
    NON_UNIQUE
FROM
    information_schema.STATISTICS
WHERE
    TABLE_SCHEMA = DATABASE()
    AND TABLE_NAME IN ('PurchaseOrder', 'PurchaseOrderItem')
ORDER BY
    TABLE_NAME, INDEX_NAME, SEQ_IN_INDEX;

-- 7. Sample query to test (will be empty if no data yet)
SELECT 
    COUNT(*) as total_purchase_orders 
FROM 
    PurchaseOrder;

-- 8. Test join between tables (will be empty if no data yet)
SELECT 
    po.po_code,
    s.supplier_name,
    COUNT(poi.po_item_id) as item_count,
    po.total_amount,
    po.status
FROM 
    PurchaseOrder po
    LEFT JOIN Supplier s ON po.supplier_id = s.supplier_id
    LEFT JOIN PurchaseOrderItem poi ON po.po_id = poi.po_id
GROUP BY 
    po.po_id
ORDER BY 
    po.order_date DESC;

-- ========================================
-- If everything looks good, you're ready to use Purchase Orders!
-- ========================================
