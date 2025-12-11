#include <iostream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <stdexcept>
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

// Display category options
void displayCategories() {
    cout << "\n=== PRODUCT CATEGORIES ===" << endl;
    cout << "1. Fresh Produce (Fruits & Vegetables)" << endl;
    cout << "2. Meat, Poultry, and Seafood" << endl;
    cout << "3. Dairy Products" << endl;
    cout << "4. Bakery" << endl;
    cout << "5. Beverages" << endl;
    cout << "6. Canned Goods" << endl;
    cout << "7. Dry Goods & Grains" << endl;
    cout << "8. Snacks & Confectionery" << endl;
    cout << "9. Frozen Foods" << endl;
    cout << "10. Condiments & Sauces" << endl;
    cout << "11. Personal Care" << endl;
    cout << "12. Household Items" << endl;
    cout << "13. Others" << endl;
    cout << "==========================" << endl;
}

// Display unit options
void displayUnits() {
    cout << "\n=== UNIT OPTIONS ===" << endl;
    cout << "1. kg (Kilogram)" << endl;
    cout << "2. g (Gram)" << endl;
    cout << "3. litre (Liter)" << endl;
    cout << "4. ml (Milliliter)" << endl;
    cout << "5. piece/pcs" << endl;
    cout << "6. bunch" << endl;
    cout << "7. bag" << endl;
    cout << "8. tray/container" << endl;
    cout << "9. cup/tub/pack" << endl;
    cout << "10. dozen" << endl;
    cout << "11. loaf" << endl;
    cout << "12. pack/packet" << endl;
    cout << "13. can" << endl;
    cout << "14. box" << endl;
    cout << "15. bottle" << endl;
    cout << "16. jar" << endl;
    cout << "17. tube" << endl;
    cout << "====================" << endl;
}

// Get category name from selection
string getCategoryName(int choice) {
    switch(choice) {
        case 1: return "Fresh Produce (Fruits & Vegetables)";
        case 2: return "Meat, Poultry, and Seafood";
        case 3: return "Dairy Products";
        case 4: return "Bakery";
        case 5: return "Beverages";
        case 6: return "Canned Goods";
        case 7: return "Dry Goods & Grains";
        case 8: return "Snacks & Confectionery";
        case 9: return "Frozen Foods";
        case 10: return "Condiments & Sauces";
        case 11: return "Personal Care";
        case 12: return "Household Items";
        case 13: return "Others";
        default: return "Others";
    }
}

// Get unit name from selection
string getUnitName(int choice) {
    switch(choice) {
        case 1: return "kg";
        case 2: return "g";
        case 3: return "litre";
        case 4: return "ml";
        case 5: return "piece/pcs";
        case 6: return "bunch";
        case 7: return "bag";
        case 8: return "tray/container";
        case 9: return "cup/tub/pack";
        case 10: return "dozen";
        case 11: return "loaf";
        case 12: return "pack/packet";
        case 13: return "can";
        case 14: return "box";
        case 15: return "bottle";
        case 16: return "jar";
        case 17: return "tube";
        default: return "piece/pcs";
    }
}

// CREATE - Add new product
void InventoryManager::addProduct(Database& db) {
    string name, category, unit;
    double cost, sellPerUnit, stockQty;
    int categoryChoice, unitChoice, supplierID;

    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, name);
    
    // Validate that product name is not empty
    while (name.empty()) {
        cout << "Error: Product name cannot be empty!" << endl;
        cout << "Enter Product Name: ";
        getline(cin, name);
    }
    
    // Select Category
    displayCategories();
    cout << "Select Category (1-13): ";
    while (!(cin >> categoryChoice) || categoryChoice < 1 || categoryChoice > 13) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Select category (1-13): ";
    }
    category = getCategoryName(categoryChoice);
    
    // Select Unit
    displayUnits();
    cout << "Select Unit (1-17): ";
    while (!(cin >> unitChoice) || unitChoice < 1 || unitChoice > 17) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Select unit (1-17): ";
    }
    unit = getUnitName(unitChoice);
    
    // Enter unit size per pack (e.g., 5 kg per pack)
    double unitSize = 1.0;
    cout << "\nEnter unit size per pack (e.g., 5 for '5 " << unit << " per pack'): ";
    while (!(cin >> unitSize) || unitSize <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid unit size per pack: ";
    }
    
    cout << "\nEnter Cost Price: RM";
    while (!(cin >> cost) || cost < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid cost price: RM";
    }
    
    cout << "Enter Sell Price per " << unit << ": RM";
    while (!(cin >> sellPerUnit) || sellPerUnit < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid sell price per " << unit << ": RM";
    }
    
    cout << "Enter Stock Quantity (in " << unit << "): ";
    while (!(cin >> stockQty) || stockQty < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid stock quantity: ";
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

    // Calculate number of packs
    double totalPacks = stockQty / unitSize;
    
    // Query to insert product with category, unit, and unit_size
    string query = "INSERT INTO Product (name, category, unit, unit_size, cost_price, sell_price, stock_quantity, supplier_id) VALUES ('" 
                   + name + "', '" + category + "', '" + unit + "', " + to_string(unitSize) + ", " + to_string(cost) + ", " + to_string(sellPerUnit) + ", " + to_string(stockQty) + ", " + to_string(supplierID) + ")";
    db.executeQuery(query);
    cout << "\n[OK] Product Added Successfully!" << endl;
    cout << "     Product: " << name << endl;
    cout << "     Category: " << category << endl;
    cout << "     Unit: " << unit << endl;
    cout << "     Pack Size: " << unitSize << " " << unit << " per pack" << endl;
    cout << "     Sell Price: RM" << sellPerUnit << " per " << unit << endl;
    cout << "     Stock: " << stockQty << " " << unit << " (" << totalPacks << " packs)" << endl;
}

// READ - View all products
void InventoryManager::viewInventory(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string query = "SELECT p.product_id, p.name, p.category, p.unit, p.unit_size, p.cost_price, p.sell_price, p.stock_quantity, s.supplier_name "
                       "FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id ORDER BY p.product_id";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch inventory: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n=========================================================================================================" << endl;
        cout << "                                          INVENTORY LIST                                             " << endl;
        cout << "=========================================================================================================" << endl;
        cout << "Item Name              | Category                      | Cost (RM) | Sell Price (per unit) | Stock (per unit) | Supplier Name   " << endl;
        cout << "=========================================================================================================" << endl;
        
        while ((row = mysql_fetch_row(res))) {
            string name = row[1] ? string(row[1]) : "N/A";
            string category = row[2] ? string(row[2]) : "Others";
            string unit = row[3] ? string(row[3]) : "pcs";
            double unitSize = row[4] ? atof(row[4]) : 1.0;
            string cost = row[5] ? string(row[5]) : "0";
            string sellPrice = row[6] ? string(row[6]) + "/" + unit : "0";
            double stockQty = row[7] ? atof(row[7]) : 0.0;
            string stock = row[7] ? string(row[7]) + " " + unit : "0";
            string supplier = row[8] ? string(row[8]) : "N/A";
            
            // Use cout instead of printf to handle special characters properly
            cout << left << setw(22) << name << " | "
                 << setw(29) << category << " | "
                 << setw(9) << cost << " | "
                 << setw(21) << sellPrice << " | "
                 << setw(16) << stock << " | "
                 << setw(15) << supplier << endl;
        }
        cout << "=========================================================================================================" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View single product by ID
void InventoryManager::viewProduct(Database& db, int productID) {
    string query = "SELECT p.product_id, p.name, p.category, p.unit, p.unit_size, p.cost_price, p.sell_price, "
                   "p.stock_quantity, s.supplier_name, p.date_created "
                   "FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id "
                   "WHERE p.product_id = " + to_string(productID);
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if (row) {
        string unit = row[3] ? string(row[3]) : "pcs";
        double unitSize = row[4] ? atof(row[4]) : 1.0;
        double stockQty = row[7] ? atof(row[7]) : 0.0;
        double packsLeft = stockQty / unitSize;
        
        cout << "\n========================================" << endl;
        cout << "         PRODUCT DETAILS                " << endl;
        cout << "========================================" << endl;
        cout << "Product ID:       " << row[0] << endl;
        cout << "Item Name:        " << row[1] << endl;
        cout << "Category:         " << (row[2] ? row[2] : "Others") << endl;
        cout << "Unit:             " << unit << endl;
        cout << "Pack Size:        " << unitSize << " " << unit << " per pack" << endl;
        cout << "Cost Price:       RM" << (row[5] ? row[5] : "0") << endl;
        cout << "Sell Price:       RM" << (row[6] ? row[6] : "0") << " per " << unit << endl;
        cout << "Stock Quantity:   " << stockQty << " " << unit << endl;
        cout << "Packs Left:       " << packsLeft << " packs" << endl;
        cout << "Supplier:         " << (row[8] ? row[8] : "N/A") << endl;
        cout << "Date Created:     " << (row[9] ? row[9] : "N/A") << endl;
        cout << "========================================" << endl;
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
    cout << "2. Category" << endl;
    cout << "3. Unit" << endl;
    cout << "4. Pack Size (Unit Size)" << endl;
    cout << "5. Cost Price" << endl;
    cout << "6. Sell Price" << endl;
    cout << "7. Stock Quantity" << endl;
    cout << "8. Supplier" << endl;
    cout << "Select: ";
    
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 8) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-8: ";
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
        int categoryChoice;
        displayCategories();
        cout << "Select new Category (1-13): ";
        while (!(cin >> categoryChoice) || categoryChoice < 1 || categoryChoice > 13) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Select category (1-13): ";
        }
        string newCategory = getCategoryName(categoryChoice);
        query = "UPDATE Product SET category = '" + newCategory + "' WHERE product_id = " + to_string(productID);
    }
    else if (choice == 3) {
        int unitChoice;
        displayUnits();
        cout << "Select new Unit (1-17): ";
        while (!(cin >> unitChoice) || unitChoice < 1 || unitChoice > 17) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Select unit (1-17): ";
        }
        string newUnit = getUnitName(unitChoice);
        query = "UPDATE Product SET unit = '" + newUnit + "' WHERE product_id = " + to_string(productID);
    }
    else if (choice == 4) {
        double newUnitSize;
        cout << "Enter new Pack Size (unit size per pack): ";
        while (!(cin >> newUnitSize) || newUnitSize <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid pack size: ";
        }
        query = "UPDATE Product SET unit_size = " + to_string(newUnitSize) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 5) {
        double newCost;
        cout << "Enter new Cost Price: RM";
        while (!(cin >> newCost) || newCost < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid cost: RM";
        }
        query = "UPDATE Product SET cost_price = " + to_string(newCost) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 6) {
        double newSell;
        cout << "Enter new Sell Price: RM";
        while (!(cin >> newSell) || newSell < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid sell price: RM";
        }
        query = "UPDATE Product SET sell_price = " + to_string(newSell) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 7) {
        double newQty;
        cout << "Enter new Stock Quantity: ";
        while (!(cin >> newQty) || newQty < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid quantity: ";
        }
        query = "UPDATE Product SET stock_quantity = " + to_string(newQty) + " WHERE product_id = " + to_string(productID);
    }
    else if (choice == 8) {
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