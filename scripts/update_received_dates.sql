-- Update existing received purchase orders with received_date
-- This script sets received_date for orders that were marked as 'Received' but have NULL received_date

-- Update received_date to match order_date for existing received orders
UPDATE PurchaseOrder 
SET received_date = order_date 
WHERE status = 'Received' 
AND received_date IS NULL;

-- Verify the update
SELECT po_code, 
       supplier_id, 
       order_date, 
       received_date, 
       status, 
       total_amount
FROM PurchaseOrder 
WHERE status = 'Received'
ORDER BY received_date DESC;
