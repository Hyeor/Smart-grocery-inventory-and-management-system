#include <iostream>
#include <string>
#include <cstdio>
#include <mysql.h>
#include "Database.h"
#include "Product.h"

using namespace std;

// Display available suppliers for selection
void displaySuppliers(Database& db) {
    string query = "SELECT supplier_id, supplier_name FROM Supplier WHERE status = 'Active'";
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row;

    cout << "\n=== AVAILABLE SUPPLIERS ===" << endl;
    cout << "ID | Supplier Name" << endl;
    cout << "------------------------" << endl;
    
    while ((row = mysql_fetch_row(res))) {
        printf("%s  | %s\n", row[0], row[1]);
    }
    cout << "------------------------" << endl;
}

// CREATE - Add new product
void InventoryManager::addProduct(Database& db) {
    string name;
    double cost, sell;
    int qty, supplierID;

    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, name);
    
    // Validate that product name is not empty
    while (name.empty()) {
        cout << "Error: Product name cannot be empty!" << endl;
        cout << "Enter Product Name: ";
        getline(cin, name);
    }
    
    cout << "Enter Cost Price: ";
    while (!(cin >> cost) || cost < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid cost price: ";
    }
    
    cout << "Enter Sell Price: ";
    while (!(cin >> sell) || sell < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid sell price: ";
    }
    
    cout << "Enter Quantity: ";
    while (!(cin >> qty) || qty <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter positive quantity: ";
    }
    
    displaySuppliers(db);
    
    cout << "Select Supplier ID: ";
    while (!(cin >> supplierID) || supplierID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid Supplier ID: ";
    }
    cin.ignore();
    
    // Verify supplier exists
    string verifyQuery = "SELECT supplier_id FROM Supplier WHERE supplier_id = " + to_string(supplierID);
    mysql_query(db.conn, verifyQuery.c_str());
    MYSQL_RES* verifyRes = mysql_store_result(db.conn);
    
    if (!mysql_fetch_row(verifyRes)) {
        cout << "[ERROR] Supplier ID not found!" << endl;
        return;
    }

    // Query to insert product
    string query = "INSERT INTO Product (name, cost_price, sell_price, stock_quantity, supplier_id) VALUES ('" 
                   + name + "', " + to_string(cost) + ", " + to_string(sell) + ", " + to_string(qty) + ", " + to_string(supplierID) + ")";
    db.executeQuery(query);
    cout << "[OK] Product Added Successfully." << endl;
}

// READ - View all products
void InventoryManager::viewInventory(Database& db) {
    string query = "SELECT p.product_id, p.date_created, p.name, s.supplier_name, p.cost_price, p.sell_price, p.stock_quantity FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id ORDER BY p.product_id";
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row;

    cout << "\n--- Inventory List ---" << endl;
    cout << "=================================================================================" << endl;
    cout << "ID | Date Created       | Name             | Supplier         | Cost | Sell | Stock | Bar" << endl;
    cout << "=================================================================================" << endl;
    
    while ((row = mysql_fetch_row(res))) {
        int stock = stoi(row[6]);
        string stockBar = "";
        
        // Create visual bar based on stock percentage
        int barLength = (stock > 100) ? 10 : (stock / 10);
        if (barLength > 10) barLength = 10;
        
        // Build bar with # for filled, - for empty
        for (int i = 0; i < barLength; i++) {
            stockBar += "#";
        }
        for (int i = barLength; i < 10; i++) {
            stockBar += "-";
        }
        
        printf("%-2s | %-18s | %-16s | %-16s | RM%-3s | RM%-3s | %-5s | [%s]\n",
               row[0], row[1], row[2], row[3] ? row[3] : "N/A", row[4], row[5], row[6], stockBar.c_str());
    }
    cout << "=================================================================================" << endl;
}

// READ - View single product by ID
void InventoryManager::viewProduct(Database& db, int productID) {
    string query = "SELECT p.product_id, p.product_id, p.name, p.supplier_id, s.supplier_name, p.cost_price, p.sell_price, p.stock_quantity, p.date_created FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id WHERE p.product_id = " + to_string(productID);
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if (row) {
        cout << "\n--- Product Details ---" << endl;
        cout << "ID: " << row[0] << endl;
        cout << "Name: " << row[2] << endl;
        cout << "Supplier: " << (row[4] ? row[4] : "N/A") << endl;
        cout << "Cost Price: RM" << row[5] << endl;
        cout << "Sell Price: RM" << row[6] << endl;
        cout << "Stock Quantity: " << row[7] << endl;
        cout << "Date Created: " << row[8] << endl;
        cout << "------------------------" << endl;
    } else {
        cout << "[ERROR] Product not found." << endl;
    }
}

// UPDATE - Update product details
void InventoryManager::updateProduct(Database& db) {
    int productID;
    cout << "Enter Product ID to update: ";
    while (!(cin >> productID) || productID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid Product ID: ";
    }
    
    // View current product
    viewProduct(db, productID);
    
    cout << "\nWhat do you want to update?" << endl;
    cout << "1. Product Name" << endl;
    cout << "2. Cost Price" << endl;
    cout << "3. Sell Price" << endl;
    cout << "4. Stock Quantity" << endl;
    cout << "5. Supplier" << endl;
    cout << "Select: ";
    
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 5) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-5: ";
    }
    cin.ignore();
    
    string query;
    if (choice == 1) {
        string newName;
        cout << "Enter new Product Name: ";
        getline(cin, newName);
        query = "UPDATE Product SET name = '" + newName + "' WHERE product_id = " + to_string(productID);
    }
    else if (choice == 2) {
        double newCost;
        cout << "Enter new Cost Price: ";
        while (!(cin >> newCost) || newCost < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid cost: ";
        }
        query = "UPDATE Product SET cost_price = " + to_string(newCost) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 3) {
        double newSell;
        cout << "Enter new Sell Price: ";
        while (!(cin >> newSell) || newSell < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid sell price: ";
        }
        query = "UPDATE Product SET sell_price = " + to_string(newSell) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 4) {
        int newQty;
        cout << "Enter new Stock Quantity: ";
        while (!(cin >> newQty) || newQty < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid quantity: ";
        }
        query = "UPDATE Product SET stock_quantity = " + to_string(newQty) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 5) {
        displaySuppliers(db);
        int newSupplierID;
        cout << "Select new Supplier ID: ";
        while (!(cin >> newSupplierID) || newSupplierID <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid Supplier ID: ";
        }
        query = "UPDATE Product SET supplier_id = " + to_string(newSupplierID) + " WHERE product_id = " + to_string(productID);
    }
    
    db.executeQuery(query);
    cout << "[OK] Product Updated Successfully." << endl;
}

// DELETE - Delete product
void InventoryManager::deleteProduct(Database& db) {
    int productID;
    cout << "Enter Product ID to delete: ";
    while (!(cin >> productID) || productID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid Product ID: ";
    }
    
    // Show product before deletion
    viewProduct(db, productID);
    
    cout << "\nAre you sure you want to delete this product? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        string query = "DELETE FROM Product WHERE product_id = " + to_string(productID);
        db.executeQuery(query);
        cout << "[OK] Product Deleted Successfully." << endl;
    } else {
        cout << "Deletion cancelled." << endl;
    }
}

// Admin CRUD Menu
void InventoryManager::adminCRUDMenu(Database& db) {
    int choice;
    while (true) {
        cout << "\n=== ADMIN - PRODUCT MANAGEMENT (CRUD) ===" << endl;
        cout << "1. Create (Add New Product)" << endl;
        cout << "2. Read (View All Products)" << endl;
        cout << "3. Read (View Specific Product)" << endl;
        cout << "4. Update (Modify Product)" << endl;
        cout << "5. Delete (Remove Product)" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Select: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
        }
        
        if (choice == 1) {
            addProduct(db);
        }
        else if (choice == 2) {
            viewInventory(db);
        }
        else if (choice == 3) {
            int productID;
            cout << "Enter Product ID: ";
            while (!(cin >> productID) || productID <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid Product ID: ";
            }
            viewProduct(db, productID);
        }
        else if (choice == 4) {
            updateProduct(db);
        }
        else if (choice == 5) {
            deleteProduct(db);
        }
        else if (choice == 6) {
            break;
        }
    }
}

// Inventory Page - Full CRUD with clean page layout
void InventoryManager::inventoryPage(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "      INVENTORY MANAGEMENT SYSTEM       " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Add New Product" << endl;
        cout << "2. View All Products" << endl;
        cout << "3. View Product Details" << endl;
        cout << "4. Update Product" << endl;
        cout << "5. Delete Product" << endl;
        cout << "6. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          ADD NEW PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            addProduct(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          VIEW ALL PRODUCTS             " << endl;
            cout << "========================================\n" << endl;
            viewInventory(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        VIEW PRODUCT DETAILS            " << endl;
            cout << "========================================\n" << endl;
            int productID;
            cout << "Enter Product ID: ";
            while (!(cin >> productID) || productID <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid Product ID: ";
            }
            cin.ignore(10000, '\n');
            viewProduct(db, productID);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 4) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          UPDATE PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            updateProduct(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 5) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          DELETE PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            deleteProduct(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 6) {
            break;
        }
    }
}