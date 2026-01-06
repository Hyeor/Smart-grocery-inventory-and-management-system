-- ========================================
-- SQL Migration: Receiving Tables
-- ========================================
-- Creates Receiving and ReceivingItem tables to track received quantities for Purchase Orders

CREATE TABLE IF NOT EXISTS Receiving (
    receiving_id INT AUTO_INCREMENT PRIMARY KEY,
    po_id INT NOT NULL,
    sub_total DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    discount_percent DECIMAL(5,2) NOT NULL DEFAULT 0.00,
    discount_amount DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    tax_percent DECIMAL(5,2) NOT NULL DEFAULT 0.00,
    tax_amount DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    total_amount DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    status ENUM('Received') DEFAULT 'Received',
    received_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (po_id) REFERENCES PurchaseOrder(po_id) ON DELETE CASCADE,
    INDEX idx_po_id (po_id),
    INDEX idx_status (status)
);

CREATE TABLE IF NOT EXISTS ReceivingItem (
    receiving_item_id INT AUTO_INCREMENT PRIMARY KEY,
    receiving_id INT NOT NULL,
    product_id INT NOT NULL,
    received_qty DECIMAL(10,2) NOT NULL,
    unit_price DECIMAL(10,2) NOT NULL,
    total_price DECIMAL(10,2) NOT NULL,
    FOREIGN KEY (receiving_id) REFERENCES Receiving(receiving_id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES Product(product_id) ON DELETE RESTRICT,
    INDEX idx_receiving_id (receiving_id),
    INDEX idx_product_id (product_id)
);
