#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

// Forward declarations
class PurchaseOrderManager;
class ReceivingManager;
class SalesManager;

class InventoryManager {
public:
    // CREATE - Add new product
    void addProduct(Database& db);
    
    // READ - View all products
    void viewInventory(Database& db);
    
    // READ - View single product by name
    void viewProduct(Database& db, string productName);
    
    // UPDATE - Update product details
    void updateProduct(Database& db);
    
    // DELETE - Delete product
    void deleteProduct(Database& db);
    
    // Admin menu for CRUD operations
    void adminCRUDMenu(Database& db);
    
    // Stocks submenu - Contains all product operations
    void stocksMenu(Database& db);
    
    // Transaction Products - Show PO and stock transactions
    void viewTransactionProducts(Database& db);
    
    // Print Barcode - Display and save barcode as image/document
    void printBarcode(Database& db);
    
    // Inventory page with all submodules
    void inventoryPage(Database& db, PurchaseOrderManager* poMgr = nullptr, 
                      ReceivingManager* recvMgr = nullptr, SalesManager* salesMgr = nullptr);
};

#endif
