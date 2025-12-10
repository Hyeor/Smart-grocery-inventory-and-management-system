#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class InventoryManager {
public:
    // CREATE - Add new product
    void addProduct(Database& db);
    
    // READ - View all products
    void viewInventory(Database& db);
    
    // READ - View single product by ID
    void viewProduct(Database& db, int productID);
    
    // UPDATE - Update product details
    void updateProduct(Database& db);
    
    // DELETE - Delete product
    void deleteProduct(Database& db);
    
    // Admin menu for CRUD operations
    void adminCRUDMenu(Database& db);
    
    // Inventory page with full CRUD menu
    void inventoryPage(Database& db);
};

#endif
