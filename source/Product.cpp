#include <iostream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <mysql.h>
#include "Database.h"
#include "Product.h"
#include "PurchaseOrder.h"
#include "Receiving.h"
#include "Sales.h"

using namespace std;

// Display available suppliers for selection
void displaySuppliers(Database& db) {
    try {
        string query = "SELECT supplier_id, supplier_name FROM Supplier WHERE status = 'Active'";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch suppliers: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n=== AVAILABLE SUPPLIERS ===" << endl;
        cout << "ID | Supplier Name" << endl;
        cout << "------------------------" << endl;
        
        while ((row = mysql_fetch_row(res))) {
            printf("%s  | %s\n", row[0], row[1]);
        }
        cout << "------------------------" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Display category options
void displayCategories() {
    cout << "\n=== PRODUCT CATEGORIES ===" << endl;
    cout << "1. Fruits and Vegetables" << endl;
    cout << "2. Meat, Poultry and Seafood" << endl;
    cout << "3. Dairy Products" << endl;
    cout << "4. Bakery" << endl;
    cout << "5. Beverages" << endl;
    cout << "6. Packaged and Canned Goods" << endl;
    cout << "7. Dry Goods and Grains" << endl;
    cout << "8. Snacks and Confectionery" << endl;
    cout << "9. Frozen Foods" << endl;
    cout << "10. Oils, Condiments and Sauces" << endl;
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
        case 1: return "Fruits and Vegetables";
        case 2: return "Meat, Poultry and Seafood";
        case 3: return "Dairy Products";
        case 4: return "Bakery";
        case 5: return "Beverages";
        case 6: return "Packaged and Canned Goods";
        case 7: return "Dry Goods and Grains";
        case 8: return "Snacks and Confectionery";
        case 9: return "Frozen Foods";
        case 10: return "Oils, Condiments and Sauces";
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

// Generate a unique numeric barcode for products
string generateUniqueBarcode(Database& db) {
    // Use timestamp + milliseconds for guaranteed uniqueness
    auto now = chrono::system_clock::now();
    auto millis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    
    // Format: last 10 digits of milliseconds + 2 digit counter
    string basecode = to_string(millis % 10000000000LL);
    if (basecode.length() < 10) {
        basecode = string(10 - basecode.length(), '0') + basecode;
    }
    
    // Try with counter to ensure uniqueness (00-99)
    for (int counter = 0; counter < 100; ++counter) {
        string barcode = basecode + to_string(counter / 10) + to_string(counter % 10);
        
        // Validate uniqueness
        if (db.conn) {
            string checkQuery = "SELECT COUNT(*) FROM Product WHERE barcode = '" + barcode + "'";
            if (mysql_query(db.conn, checkQuery.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(db.conn);
                if (res) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if (row && row[0] && atoi(row[0]) == 0) {
                        mysql_free_result(res);
                        return barcode;  // Found unique barcode
                    }
                    mysql_free_result(res);
                }
            }
        }
    }
    
    // Fallback: Use full timestamp if counter approach fails
    string fallback = to_string(millis);
    if (fallback.length() > 12) {
        fallback = fallback.substr(fallback.length() - 12);
    } else if (fallback.length() < 12) {
        fallback = string(12 - fallback.length(), '0') + fallback;
    }
    
    return fallback;
}

// Validate and clean product name - remove special characters
string validateProductName(string name) {
    string cleaned;
    for (char c : name) {
        // Allow alphanumeric, spaces, hyphens, and commas
        if (isalnum(c) || c == ' ' || c == '-' || c == ',') {
            cleaned += c;
        }
    }
    // Trim leading/trailing spaces
    size_t start = cleaned.find_first_not_of(" ");
    size_t end = cleaned.find_last_not_of(" ");
    if (start == string::npos) return "";
    return cleaned.substr(start, end - start + 1);
}

// CREATE - Add new product
void InventoryManager::addProduct(Database& db) {
    string name, category, unit;
    double cost, sellPerUnit;
    int categoryChoice, unitChoice, supplierID;

    cout << "Enter Product Name (0 to cancel): ";
    getline(cin >> ws, name);
    
    // Validate and clean product name
    name = validateProductName(name);
    
    // Allow immediate cancel before any validation
    if (name == "0") {
        return; // cancel adding
    }
    
    // Validate that product name is not empty (still allow cancel)
    while (name.empty()) {
        cout << "Error: Product name cannot be empty! Use only letters, numbers, spaces, hyphens, and commas." << endl;
        cout << "Enter Product Name (0 to cancel): ";
        getline(cin, name);
        name = validateProductName(name);
        if (name == "0") {
            return;
        }
    }
    
    // Check for duplicate product name
    string escapedName = name;
    size_t pos = 0;
    while ((pos = escapedName.find("'", pos)) != string::npos) {
        escapedName.replace(pos, 1, "''");
        pos += 2;
    }
    
    string checkDuplicateQuery = "SELECT COUNT(*) FROM Product WHERE LOWER(name) = LOWER('" + escapedName + "')";
    if (mysql_query(db.conn, checkDuplicateQuery.c_str()) == 0) {
        MYSQL_RES* dupRes = mysql_store_result(db.conn);
        if (dupRes) {
            MYSQL_ROW dupRow = mysql_fetch_row(dupRes);
            if (dupRow && atoi(dupRow[0]) > 0) {
                cout << "\n[ERROR] A product with the name '" << name << "' already exists!" << endl;
                cout << "        Please use a different product name." << endl;
                mysql_free_result(dupRes);
                return;
            }
            mysql_free_result(dupRes);
        }
    }
    
    // Select Category
    displayCategories();
    cout << "Select Category (1-13, 0 to cancel): ";
    while (!(cin >> categoryChoice) || categoryChoice < 0 || categoryChoice > 13) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Select category (1-13, 0 to cancel): ";
    }
    if (categoryChoice == 0) return;
    category = getCategoryName(categoryChoice);
    
    // Select Unit
    displayUnits();
    cout << "Select Unit (1-17, 0 to cancel): ";
    while (!(cin >> unitChoice) || unitChoice < 0 || unitChoice > 17) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Select unit (1-17, 0 to cancel): ";
    }
    if (unitChoice == 0) return;
    unit = getUnitName(unitChoice);
    
    // Enter unit size per pack - only for weight/volume units
    double unitSize = 1.0;
    string unitLower = unit;
    transform(unitLower.begin(), unitLower.end(), unitLower.begin(), ::tolower);
    
    // Check if unit is weight/volume based (kg, g, litre, ml)
    bool isWeightVolume = (unitLower == "kg" || unitLower == "g" || 
                           unitLower == "litre" || unitLower == "ml");
    
    if (isWeightVolume) {
        cout << "\nEnter Unit size (number only): ";
        while (!(cin >> unitSize) || unitSize <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid unit size: ";
        }
    }
    
    cout << "\nEnter Cost Price: RM";
    while (!(cin >> cost) || cost < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid cost price: RM";
    }
    
    cout << "Enter Sell Price: RM";
    while (!(cin >> sellPerUnit) || sellPerUnit < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid sell price: RM";
    }
    
    // Basic stock quantity input (single entry)
    double stockQty = 0;
    cout << "\nEnter Stock Quantity: ";
    while (!(cin >> stockQty) || stockQty < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid stock quantity: ";
    }
    cin.ignore();
    
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
    
    if (mysql_query(db.conn, verifyQuery.c_str())) {
        cout << "[ERROR] Database query failed: " << mysql_error(db.conn) << endl;
        return;
    }
    
    MYSQL_RES* verifyRes = mysql_store_result(db.conn);
    if (!verifyRes) {
        cout << "[ERROR] Database query failed: Unable to store result set" << endl;
        return;
    }
    
    if (!mysql_fetch_row(verifyRes)) {
        cout << "[ERROR] Supplier ID not found!" << endl;
        mysql_free_result(verifyRes);
        return;
    }
    mysql_free_result(verifyRes);

    // Barcode input - user choice
    string barcode;
    int barcodeChoice;
    
    cout << "\nDo you have a barcode number?" << endl;
    cout << "1. Yes, I have a barcode" << endl;
    cout << "2. No, I don't have a barcode" << endl;
    cout << "Select option (1 or 2): ";
    while (!(cin >> barcodeChoice) || (barcodeChoice != 1 && barcodeChoice != 2)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1 or 2: ";
    }
    cin.ignore();
    
    if (barcodeChoice == 1) {
        // User has a barcode
        cout << "Enter Barcode Number: ";
        getline(cin, barcode);
        
        // Validate barcode uniqueness if user provided one
        if (!barcode.empty()) {
            string checkQuery = "SELECT COUNT(*) FROM Product WHERE barcode = '" + barcode + "'";
            if (mysql_query(db.conn, checkQuery.c_str()) == 0) {
                MYSQL_RES* checkRes = mysql_store_result(db.conn);
                if (checkRes) {
                    MYSQL_ROW checkRow = mysql_fetch_row(checkRes);
                    if (checkRow && atoi(checkRow[0]) > 0) {
                        cout << "[WARNING] Barcode already exists! Auto-generating new barcode." << endl;
                        barcode = generateUniqueBarcode(db);
                        cout << "[INFO] New auto-generated barcode: " << barcode << endl;
                    }
                    mysql_free_result(checkRes);
                }
            }
        } else {
            cout << "[WARNING] Barcode is empty! Auto-generating barcode." << endl;
            barcode = generateUniqueBarcode(db);
            cout << "[INFO] Auto-generated barcode: " << barcode << endl;
        }
    } else {
        // User doesn't have a barcode - auto-generate
        barcode = generateUniqueBarcode(db);
        cout << "[INFO] Auto-generated barcode: " << barcode << endl;
    }

    // Calculate number of packs (unused for display)
    // double totalPacks = stockQty / unitSize;
    
    // Query to insert product with category, unit, and unit_size
    string query = "INSERT INTO Product (barcode, name, category, unit, unit_size, cost_price, sell_price, stock_quantity, supplier_id) VALUES ('" 
                   + barcode + "', '" + name + "', '" + category + "', '" + unit + "', " + to_string(unitSize) + ", " + to_string(cost) + ", " + to_string(sellPerUnit) + ", " + to_string(stockQty) + ", " + to_string(supplierID) + ")";
    db.executeQuery(query);
    cout << "\n[OK] Product Added Successfully!" << endl;
    cout << "     Product: " << name << endl;
    cout << "     Barcode: " << barcode << endl;
    cout << "     Category: " << category << endl;
    cout << "     Unit: " << unit << endl;
    cout << "     Sell Price: RM" << sellPerUnit << " per " << unit << endl;
    cout << "     Subtotal in unit: " << stockQty << " " << unit << endl;
}

// READ - View all products
void InventoryManager::viewInventory(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string query = "SELECT p.product_id, p.barcode, p.name, p.category, p.unit, p.unit_size, p.cost_price, p.sell_price, p.stock_quantity, s.supplier_name "
                       "FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id ORDER BY p.product_id";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch inventory: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n" << string(150, '=') << endl;
        cout << setw(75) << "INVENTORY LIST" << setw(76) << "" << endl;
        cout << string(150, '=') << endl;
           cout << left << setw(25) << "Item Name" << " | "
               << setw(30) << "Category" << " | "
               << setw(10) << "Cost" << " | "
               << setw(15) << "Sell Price" << " | "
               << setw(25) << "Stock quantity" << " | "
               << setw(20) << "Supplier" << endl;
        cout << string(150, '=') << endl;
        
        while ((row = mysql_fetch_row(res))) {
            string name = row[2] ? string(row[2]) : "N/A";
            string category = row[3] ? string(row[3]) : "Others";
            string unit = row[4] ? string(row[4]) : "pcs";
            string cost = row[6] ? string(row[6]) : "0";
            string unitLower = unit;
            transform(unitLower.begin(), unitLower.end(), unitLower.begin(), ::tolower);

            // Sell price formatting: hide unit for litre/ml
            string sellPrice;
            if (row[7]) {
                if (unitLower == "litre" || unitLower == "ml") {
                    sellPrice = string(row[7]);
                } else {
                    sellPrice = string(row[7]) + "/" + unit;
                }
            } else {
                sellPrice = "0";
            }

            // Helper to format numbers with up to 2 decimals
            auto formatQty = [](double value) {
                stringstream ss;
                ss << fixed << setprecision(2) << value;
                string out = ss.str();
                // Trim trailing zeros and dot
                while (!out.empty() && out.back() == '0') out.pop_back();
                if (!out.empty() && out.back() == '.') out.pop_back();
                return out.empty() ? string("0") : out;
            };

            // Stock display rules: calculate and show packs (like product details)
            string stockDisplay;
            if (row[8]) {
                double quantity = stod(string(row[8]));
                double unitSizeVal = row[5] ? stod(string(row[5])) : 1.0;
                double packs = unitSizeVal > 0 ? quantity / unitSizeVal : quantity;
                
                string packsStr = formatQty(packs);
                
                if (unitLower == "kg" || unitLower == "g" || unitLower == "litre" || unitLower == "ml") {
                    stockDisplay = packsStr; // show number only for weight/volume units
                } else {
                    stockDisplay = packsStr + " " + unit; // show quantity with unit for other options
                }
            } else {
                stockDisplay = "0";
            }
            
            string supplier = row[9] ? string(row[9]) : "N/A";
            
            // Truncate long strings to fit column widths
            if (name.length() > 24) name = name.substr(0, 21) + "...";
            if (category.length() > 29) category = category.substr(0, 26) + "...";
            if (supplier.length() > 19) supplier = supplier.substr(0, 16) + "...";
            if (stockDisplay.length() > 24) stockDisplay = stockDisplay.substr(0, 21) + "...";
            
            // Use cout with consistent column widths for perfect alignment
            cout << left << setw(25) << name << " | "
                 << setw(30) << category << " | "
                 << setw(10) << cost << " | "
                 << setw(15) << sellPrice << " | "
                 << setw(25) << stockDisplay << " | "
                 << setw(20) << supplier << endl;
        }
        cout << string(150, '=') << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View single product by name
void InventoryManager::viewProduct(Database& db, string productName) {
    try {
        // Escape single quotes in product name for SQL query
        string escapedName = productName;
        size_t pos = 0;
        while ((pos = escapedName.find("'", pos)) != string::npos) {
            escapedName.replace(pos, 1, "''");
            pos += 2;
        }
        
        string query = "SELECT p.product_id, p.barcode, p.name, p.category, p.unit, p.unit_size, p.cost_price, p.sell_price, "
                       "p.stock_quantity, s.supplier_name, p.date_created "
                       "FROM Product p LEFT JOIN Supplier s ON p.supplier_id = s.supplier_id "
                       "WHERE p.name LIKE '%" + escapedName + "%'";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch product: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row) {
            string unit = row[4] ? string(row[4]) : "pcs";
            double unitSize = row[5] ? atof(row[5]) : 1.0;
            double stockQty = row[8] ? atof(row[8]) : 0.0;
            double packsLeft = stockQty / unitSize;
            
            // Convert unit to lowercase for comparison
            string unitLower = unit;
            transform(unitLower.begin(), unitLower.end(), unitLower.begin(), ::tolower);
            
            // Determine sell price format: only kg and g show unit
            string sellPriceDisplay;
            if (unitLower == "kg" || unitLower == "g") {
                sellPriceDisplay = "RM" + string(row[7] ? row[7] : "0") + "/" + unit;
            } else {
                sellPriceDisplay = "RM" + string(row[7] ? row[7] : "0");
            }
            
            // Determine stock quantity format: show unit except for kg, g, litre, ml
            string stockQtyDisplay;
            if (unitLower == "kg" || unitLower == "g" || unitLower == "litre" || unitLower == "ml") {
                stockQtyDisplay = to_string((int)packsLeft); // show number only
            } else {
                stockQtyDisplay = to_string((int)packsLeft) + " " + unit;
            }
            
            cout << "\n========================================" << endl;
            cout << "         PRODUCT DETAILS                " << endl;
            cout << "========================================" << endl;
            cout << "Item Name:        " << row[2] << endl;
            cout << "Barcode:          " << (row[1] ? row[1] : "N/A") << endl;
            cout << "Category:         " << (row[3] ? row[3] : "Others") << endl;
            cout << "Unit:             " << unit << endl;
            cout << "Cost Price:       RM" << (row[6] ? row[6] : "0") << endl;
            cout << "Sell Price:       " << sellPriceDisplay << endl;
            cout << "Subtotal in unit: " << stockQty << " " << unit << endl;
            cout << "Stock quantity:   " << stockQtyDisplay << endl;
            cout << "Supplier:         " << (row[9] ? row[9] : "N/A") << endl;
            cout << "Date Created:     " << (row[10] ? row[10] : "N/A") << endl;
            cout << "========================================" << endl;
        } else {
            cout << "[ERROR] Product not found." << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// UPDATE - Update product details
void InventoryManager::updateProduct(Database& db) {
    string productName;
    cout << "Enter Product Name to update (0 to go back): ";
    getline(cin >> ws, productName);
    
    if (productName == "0") {
        return;
    }
    
    if (productName.empty()) {
        cout << "[ERROR] Product name cannot be empty." << endl;
        return;
    }
    
    // View current product
    viewProduct(db, productName);
    
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
    
    // Escape single quotes in product name for SQL query
    string escapedName = productName;
    size_t pos = 0;
    while ((pos = escapedName.find("'", pos)) != string::npos) {
        escapedName.replace(pos, 1, "''");
        pos += 2;
    }
    
    string query;
    if (choice == 1) {
        string newName;
        cout << "Enter new Product Name (0 to cancel): ";
        getline(cin, newName);
        if (newName == "0") return;
        query = "UPDATE Product SET name = '" + newName + "' WHERE name = '" + escapedName + "'";
    }
    else if (choice == 2) {
        int categoryChoice;
        displayCategories();
        cout << "Select new Category (1-13, 0 to cancel): ";
        while (!(cin >> categoryChoice) || categoryChoice < 0 || categoryChoice > 13) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Select category (1-13, 0 to cancel): ";
        }
        if (categoryChoice == 0) return;
        string newCategory = getCategoryName(categoryChoice);
        query = "UPDATE Product SET category = '" + newCategory + "' WHERE name = '" + escapedName + "'";
    }
    else if (choice == 3) {
        int unitChoice;
        displayUnits();
        cout << "Select new Unit (1-17, 0 to cancel): ";
        while (!(cin >> unitChoice) || unitChoice < 0 || unitChoice > 17) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Select unit (1-17, 0 to cancel): ";
        }
        if (unitChoice == 0) return;
        string newUnit = getUnitName(unitChoice);
        query = "UPDATE Product SET unit = '" + newUnit + "' WHERE name = '" + escapedName + "'";
    }
    else if (choice == 4) {
        double newUnitSize;
        cout << "Enter new Pack Size (unit size per pack): ";
        while (!(cin >> newUnitSize) || newUnitSize <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid pack size: ";
        }
        query = "UPDATE Product SET unit_size = " + to_string(newUnitSize) + " WHERE name = '" + escapedName + "'";
    }
    else if (choice == 5) {
        double newCost;
        cout << "Enter new Cost Price: RM";
        while (!(cin >> newCost) || newCost < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid cost: RM";
        }
        query = "UPDATE Product SET cost_price = " + to_string(newCost) + " WHERE name = '" + escapedName + "'";
    }
    else if (choice == 6) {
        double newSell;
        cout << "Enter new Sell Price: RM";
        while (!(cin >> newSell) || newSell < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid sell price: RM";
        }
        query = "UPDATE Product SET sell_price = " + to_string(newSell) + " WHERE name = '" + escapedName + "'";
    }
    else if (choice == 7) {
        double newQty;
        cout << "Enter new Stock Quantity: ";
        while (!(cin >> newQty) || newQty < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid quantity: ";
        }
        query = "UPDATE Product SET stock_quantity = " + to_string(newQty) + " WHERE name = '" + escapedName + "'";
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
        query = "UPDATE Product SET supplier_id = " + to_string(newSupplierID) + " WHERE name = '" + escapedName + "'";
    }
    
    db.executeQuery(query);
    cout << "[OK] Product Updated Successfully." << endl;
}

// DELETE - Delete product
void InventoryManager::deleteProduct(Database& db) {
    // Display all products first for reference
    cout << "\n=== ALL PRODUCTS ===" << endl;
    viewInventory(db);
    
    string productName;
    cout << "\nEnter Product Name to delete (0 to go back): ";
    getline(cin >> ws, productName);
    
    if (productName == "0") {
        return;
    }
    
    if (productName.empty()) {
        cout << "[ERROR] Product name cannot be empty." << endl;
        return;
    }
    
    // Show product before deletion
    viewProduct(db, productName);
    
    cout << "\nEnter exact product name to confirm deletion: ";
    string confirmName;
    getline(cin, confirmName);
    
    if (confirmName == productName) {
        // Reconfirmation before proceeding with deletion
        cout << "\n" << string(50, '=') << endl;
        cout << "Are you sure you want to delete? Once you delete,\n";
        cout << "it cannot be reverted back.\n";
        cout << string(50, '=') << endl;
        cout << "\nEnter YES to confirm or NO to cancel: ";
        string finalConfirm;
        getline(cin, finalConfirm);
        
        if (finalConfirm != "YES") {
            cout << "\n[INFO] Deletion cancelled." << endl;
            return;
        }
        // Escape single quotes in product name
        string escapedName = productName;
        size_t pos = 0;
        while ((pos = escapedName.find("'", pos)) != string::npos) {
            escapedName.replace(pos, 1, "''");
            pos += 2;
        }
        
        // First, get the product_id
        string getIdQuery = "SELECT product_id FROM Product WHERE name = '" + escapedName + "'";
        if (mysql_query(db.conn, getIdQuery.c_str())) {
            cout << "[ERROR] Failed to fetch product details: " << mysql_error(db.conn) << endl;
            return;
        }
        
        MYSQL_RES* idRes = mysql_store_result(db.conn);
        if (!idRes) {
            cout << "[ERROR] Product not found." << endl;
            return;
        }
        
        MYSQL_ROW idRow = mysql_fetch_row(idRes);
        if (!idRow) {
            cout << "[ERROR] Product not found." << endl;
            mysql_free_result(idRes);
            return;
        }
        
        string productId = idRow[0];
        mysql_free_result(idRes);
        
        // Check if product has related purchase order items
        string checkPOQuery = "SELECT COUNT(*) FROM PurchaseOrderItem WHERE product_id = " + productId;
        if (mysql_query(db.conn, checkPOQuery.c_str())) {
            cout << "[ERROR] Failed to check purchase order items: " << mysql_error(db.conn) << endl;
            return;
        }
        
        MYSQL_RES* poRes = mysql_store_result(db.conn);
        MYSQL_ROW poRow = mysql_fetch_row(poRes);
        int poCount = stoi(poRow[0]);
        mysql_free_result(poRes);
        
        // Check if product has related receiving records
        string checkReceivingQuery = "SELECT COUNT(*) FROM ReceivingItem WHERE product_id = " + productId;
        if (mysql_query(db.conn, checkReceivingQuery.c_str())) {
            cout << "[ERROR] Failed to check receiving records: " << mysql_error(db.conn) << endl;
            return;
        }
        
        MYSQL_RES* receivingRes = mysql_store_result(db.conn);
        MYSQL_ROW receivingRow = mysql_fetch_row(receivingRes);
        int receivingCount = stoi(receivingRow[0]);
        mysql_free_result(receivingRes);
        
        // If product has related records, ask user
        if (poCount > 0 || receivingCount > 0) {
            cout << "\n[WARNING] This product has related records:" << endl;
            if (poCount > 0) {
                cout << "        - " << poCount << " purchase order item(s)" << endl;
            }
            if (receivingCount > 0) {
                cout << "        - " << receivingCount << " receiving item(s)" << endl;
            }
            cout << "\nOptions:" << endl;
            cout << "1. Force Delete (removes related records)" << endl;
            cout << "2. Cancel Deletion" << endl;
            cout << "Enter choice (1 or 2): ";
            int choice;
            while (!(cin >> choice) || (choice != 1 && choice != 2)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter 1 or 2: ";
            }
            cin.ignore();
            
            if (choice == 2) {
                cout << "\n[INFO] Deletion cancelled." << endl;
                return;
            }
            
            // User chose Force Delete
            cout << "\n[WARNING] Are you sure? This will delete:" << endl;
            cout << "        - The product: " << productName << endl;
            cout << "        - All related purchase order items" << endl;
            cout << "        - All related receiving items" << endl;
            cout << "\nType 'YES' to confirm force deletion: ";
            string forceConfirm;
            getline(cin, forceConfirm);
            
            if (forceConfirm != "YES") {
                cout << "\n[INFO] Force deletion cancelled." << endl;
                return;
            }
            
            // Delete related records using transaction
            try {
                // Start transaction
                mysql_query(db.conn, "START TRANSACTION");
                
                // Delete related receiving items first
                string deleteReceivingQuery = "DELETE FROM ReceivingItem WHERE product_id = " + productId;
                if (mysql_query(db.conn, deleteReceivingQuery.c_str())) {
                    mysql_query(db.conn, "ROLLBACK");
                    throw runtime_error("Failed to delete receiving items: " + string(mysql_error(db.conn)));
                }
                
                // Delete related purchase order items
                string deletePOQuery = "DELETE FROM PurchaseOrderItem WHERE product_id = " + productId;
                if (mysql_query(db.conn, deletePOQuery.c_str())) {
                    mysql_query(db.conn, "ROLLBACK");
                    throw runtime_error("Failed to delete purchase order items: " + string(mysql_error(db.conn)));
                }
                
                // Delete the product
                string deleteProductQuery = "DELETE FROM Product WHERE product_id = " + productId;
                if (mysql_query(db.conn, deleteProductQuery.c_str())) {
                    mysql_query(db.conn, "ROLLBACK");
                    throw runtime_error("Failed to delete product: " + string(mysql_error(db.conn)));
                }
                
                // Commit transaction
                mysql_query(db.conn, "COMMIT");
                cout << "\n[INFO] Related records deleted." << endl;
                cout << "[OK] Product '" << productName << "' Deleted Successfully." << endl;
                
            } catch (const exception& e) {
                cout << "[ERROR] " << e.what() << endl;
                return;
            }
        } else {
            // No related records, just delete the product
            string query = "DELETE FROM Product WHERE name = '" + escapedName + "'";
            db.executeQuery(query);
            cout << "[OK] Product '" << productName << "' Deleted Successfully." << endl;
        }
    } else {
        cout << "Deletion cancelled - name does not match." << endl;
    }
}

// Print Barcode - Display and save barcode as document
void InventoryManager::printBarcode(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        // Show all products first for reference
        cout << "\n=== AVAILABLE PRODUCTS ===" << endl;
        viewInventory(db);
        
        string productName;
        cout << "\nEnter Product Name to print barcode (0 to go back): ";
        getline(cin >> ws, productName);
        
        if (productName == "0") {
            return;
        }
        
        if (productName.empty()) {
            cout << "[ERROR] Product name cannot be empty." << endl;
            return;
        }
        
        // Escape single quotes
        string escapedName = productName;
        size_t pos = 0;
        while ((pos = escapedName.find("'", pos)) != string::npos) {
            escapedName.replace(pos, 1, "''");
            pos += 2;
        }
        
        // Get product details
        string query = "SELECT product_id, name, barcode, category, unit, sell_price FROM Product WHERE LOWER(name) = LOWER('" + escapedName + "')";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch product: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) {
            cout << "[ERROR] Product '" << productName << "' not found." << endl;
            mysql_free_result(res);
            return;
        }
        
        string name = row[1];
        string barcode = row[2];
        string category = row[3];
        string unit = row[4];
        double price = stod(row[5]);
        
        mysql_free_result(res);
        
        // Create reports directory if it doesn't exist
        #ifdef _WIN32
        system("if not exist reports mkdir reports");
        #else
        system("mkdir -p reports");
        #endif
        
        // Generate barcode label document (HTML format)
        string filename = "reports/Barcode_" + barcode + ".html";
        
        ofstream htmlFile(filename);
        if (!htmlFile.is_open()) {
            throw runtime_error("Failed to create barcode file");
        }
        
        // Generate HTML with barcode display
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html lang=\"en\">\n";
        htmlFile << "<head>\n";
        htmlFile << "    <meta charset=\"UTF-8\">\n";
        htmlFile << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        // Blank title to avoid showing in printed headers
        htmlFile << "    <title></title>\n";
        htmlFile << "    <script src=\"https://cdn.jsdelivr.net/npm/jsbarcode@3.11.5/dist/JsBarcode.all.min.js\"></script>\n";
        htmlFile << "    <style>\n";
        htmlFile << "        @media print {\n";
        htmlFile << "            body { margin: 10mm; }\n";
        htmlFile << "            .download-button { display: none; }\n";
        htmlFile << "        }\n";
        htmlFile << "        body {\n";
        htmlFile << "            font-family: Arial, sans-serif;\n";
        htmlFile << "            display: flex;\n";
        htmlFile << "            justify-content: center;\n";
        htmlFile << "            align-items: center;\n";
        htmlFile << "            min-height: 100vh;\n";
        htmlFile << "            margin: 0;\n";
        htmlFile << "            background: #f5f5f5;\n";
        htmlFile << "        }\n";
        htmlFile << "        .barcode-container {\n";
        htmlFile << "            background: white;\n";
        htmlFile << "            padding: 30px;\n";
        htmlFile << "            border: 2px solid #333;\n";
        htmlFile << "            border-radius: 8px;\n";
        htmlFile << "            text-align: center;\n";
        htmlFile << "            box-shadow: 0 4px 6px rgba(0,0,0,0.1);\n";
        htmlFile << "            max-width: 400px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .product-info {\n";
        htmlFile << "            margin-bottom: 20px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .product-name {\n";
        htmlFile << "            font-size: 18px;\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            color: #2c3e50;\n";
        htmlFile << "            margin-bottom: 5px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .product-detail {\n";
        htmlFile << "            font-size: 12px;\n";
        htmlFile << "            color: #7f8c8d;\n";
        htmlFile << "            margin: 3px 0;\n";
        htmlFile << "        }\n";
        htmlFile << "        .barcode-display {\n";
        htmlFile << "            margin: 20px 0;\n";
        htmlFile << "            padding: 20px;\n";
        htmlFile << "            background: white;\n";
        htmlFile << "            border: 1px solid #e0e0e0;\n";
        htmlFile << "            border-radius: 4px;\n";
        htmlFile << "        }\n";
        htmlFile << "        #barcode {\n";
        htmlFile << "            max-width: 100%;\n";
        htmlFile << "        }\n";
        htmlFile << "        #barcodeCanvas {\n";
        htmlFile << "            display: none;\n";
        htmlFile << "        }\n";
        htmlFile << "        .barcode-label {\n";
        htmlFile << "            font-size: 10px;\n";
        htmlFile << "            color: #95a5a6;\n";
        htmlFile << "            margin-top: 10px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .price {\n";
        htmlFile << "            font-size: 20px;\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            color: #27ae60;\n";
        htmlFile << "            margin: 15px 0;\n";
        htmlFile << "        }\n";
        htmlFile << "        .download-button {\n";
        htmlFile << "            margin: 10px 5px;\n";
        htmlFile << "            padding: 10px 20px;\n";
        htmlFile << "            background: #27ae60;\n";
        htmlFile << "            color: white;\n";
        htmlFile << "            border: none;\n";
        htmlFile << "            border-radius: 4px;\n";
        htmlFile << "            cursor: pointer;\n";
        htmlFile << "            font-size: 14px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .download-button:hover {\n";
        htmlFile << "            background: #229954;\n";
        htmlFile << "        }\n";
        htmlFile << "        .download-button.png {\n";
        htmlFile << "            background: #3498db;\n";
        htmlFile << "        }\n";
        htmlFile << "        .download-button.png:hover {\n";
        htmlFile << "            background: #2980b9;\n";
        htmlFile << "        }\n";
        htmlFile << "    </style>\n";
        htmlFile << "</head>\n";
        htmlFile << "<body>\n";
        htmlFile << "    <div class=\"barcode-container\">\n";
        htmlFile << "        <div class=\"product-info\">\n";
        htmlFile << "            <div class=\"product-name\">" << name << "</div>\n";
        htmlFile << "        </div>\n";
        htmlFile << "\n";
        htmlFile << "        <div class=\"barcode-display\">\n";
        htmlFile << "            <svg id=\"barcode\"></svg>\n";
        htmlFile << "            <canvas id=\"barcodeCanvas\"></canvas>\n";
        htmlFile << "            <div class=\"barcode-label\">SCANNABLE BARCODE</div>\n";
        htmlFile << "        </div>\n";
        htmlFile << "\n";
        htmlFile << "        <div class=\"price\">RM " << fixed << setprecision(2) << price << "</div>\n";
        htmlFile << "\n";
        htmlFile << "        <button class=\"download-button png\" onclick=\"downloadBarcode('png')\">Download as PNG</button>\n";
        htmlFile << "        <button class=\"download-button\" onclick=\"downloadBarcode('jpg')\">Download as JPG</button>\n";
        htmlFile << "    </div>\n";
        htmlFile << "\n";
        htmlFile << "    <script>\n";
        htmlFile << "        // Generate barcode using JsBarcode\n";
        htmlFile << "        JsBarcode('#barcode', '" << barcode << "', {\n";
        htmlFile << "            format: 'CODE128',\n";
        htmlFile << "            width: 2,\n";
        htmlFile << "            height: 80,\n";
        htmlFile << "            displayValue: true,\n";
        htmlFile << "            fontSize: 16,\n";
        htmlFile << "            margin: 10,\n";
        htmlFile << "            background: '#ffffff'\n";
        htmlFile << "        });\n";
        htmlFile << "\n";
        htmlFile << "        function downloadBarcode(format) {\n";
        htmlFile << "            const canvas = document.getElementById('barcodeCanvas');\n";
        htmlFile << "            const ctx = canvas.getContext('2d');\n";
        htmlFile << "            const container = document.querySelector('.barcode-container');\n";
        htmlFile << "            \n";
        htmlFile << "            // Set canvas size\n";
        htmlFile << "            canvas.width = 400;\n";
        htmlFile << "            canvas.height = 300;\n";
        htmlFile << "            \n";
        htmlFile << "            // Fill white background\n";
        htmlFile << "            ctx.fillStyle = 'white';\n";
        htmlFile << "            ctx.fillRect(0, 0, canvas.width, canvas.height);\n";
        htmlFile << "            \n";
        htmlFile << "            // Draw product name\n";
        htmlFile << "            ctx.fillStyle = '#2c3e50';\n";
        htmlFile << "            ctx.font = 'bold 18px Arial';\n";
        htmlFile << "            ctx.textAlign = 'center';\n";
        htmlFile << "            ctx.fillText('" << name << "', 200, 30);\n";
        htmlFile << "            \n";
        // intentionally removed category and unit from the downloadable image
        htmlFile << "            \n";
        htmlFile << "            // Get barcode SVG and convert to image\n";
        htmlFile << "            const svg = document.getElementById('barcode');\n";
        htmlFile << "            const svgData = new XMLSerializer().serializeToString(svg);\n";
        htmlFile << "            const img = new Image();\n";
        htmlFile << "            \n";
        htmlFile << "            img.onload = function() {\n";
        htmlFile << "                // Draw barcode image centered\n";
        htmlFile << "                const barcodeX = (canvas.width - img.width) / 2;\n";
        htmlFile << "                ctx.drawImage(img, barcodeX, 90);\n";
        htmlFile << "                \n";
        htmlFile << "                // Draw label\n";
        htmlFile << "                ctx.fillStyle = '#95a5a6';\n";
        htmlFile << "                ctx.font = '10px Arial';\n";
        htmlFile << "                ctx.fillText('SCANNABLE BARCODE', 200, 200);\n";
        htmlFile << "                \n";
        htmlFile << "                // Draw price\n";
        htmlFile << "                ctx.fillStyle = '#27ae60';\n";
        htmlFile << "                ctx.font = 'bold 20px Arial';\n";
        htmlFile << "                ctx.fillText('RM " << fixed << setprecision(2) << price << "', 200, 240);\n";
        htmlFile << "                \n";
        htmlFile << "                // Download image\n";
        htmlFile << "                const mimeType = format === 'png' ? 'image/png' : 'image/jpeg';\n";
        htmlFile << "                const link = document.createElement('a');\n";
        htmlFile << "                link.download = 'Barcode_" << barcode << ".' + format;\n";
        htmlFile << "                link.href = canvas.toDataURL(mimeType, 0.95);\n";
        htmlFile << "                link.click();\n";
        htmlFile << "            };\n";
        htmlFile << "            \n";
        htmlFile << "            img.src = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(svgData)));\n";
        htmlFile << "        }\n";
        htmlFile << "    </script>\n";
        htmlFile << "</body>\n";
        htmlFile << "</html>\n";
        
        htmlFile.close();
        
        cout << "\n[OK] Barcode label generated successfully!" << endl;
        cout << "     Product: " << name << endl;
        cout << "     Barcode: " << barcode << endl;
        cout << "     File: " << filename << endl;
        cout << "\nOpening in browser..." << endl;
        
        #ifdef _WIN32
        char currentPath[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentPath);
        string fullPath = string(currentPath) + "\\reports\\Barcode_" + barcode + ".html";
        
        string command = "start \"\" \"" + fullPath + "\"";
        int result = system(command.c_str());
        
        if (result != 0) {
            cout << "[WARNING] Could not open browser automatically." << endl;
            cout << "You can manually open: " << fullPath << endl;
        }
        #else
        char currentPath[PATH_MAX];
        getcwd(currentPath, sizeof(currentPath));
        string fullPath = string(currentPath) + "/reports/Barcode_" + barcode + ".html";
        
        string command = "xdg-open \"" + fullPath + "\" &";
        int ret = system(command.c_str());
        if (ret != 0) {
            cout << "[WARNING] Could not open browser automatically." << endl;
            cout << "File saved at: " << fullPath << endl;
        }
        #endif
        
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
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
            string productName;
            cout << "Enter Product Name: ";
            getline(cin >> ws, productName);
            if (!productName.empty()) {
                viewProduct(db, productName);
            } else {
                cout << "[ERROR] Product name cannot be empty." << endl;
            }
        }
        else if (choice == 4) {
            // Show current products first for easier selection
            viewInventory(db);
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

// READ - View Transaction Products (PO and Stock Transactions)
void InventoryManager::viewTransactionProducts(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        // Query to get products with their current stock and latest PO information
        string query = "SELECT DISTINCT p.product_id, p.name, p.stock_quantity, p.unit, p.unit_size, "
                       "COALESCE(SUM(ri.received_qty), 0) as total_po_received, "
                       "COALESCE(MAX(po.received_date), 'N/A') as latest_transaction_date "
                       "FROM Product p "
                       "LEFT JOIN ReceivingItem ri ON p.product_id = ri.product_id "
                       "LEFT JOIN Receiving r ON ri.receiving_id = r.receiving_id "
                       "LEFT JOIN PurchaseOrder po ON r.po_id = po.po_id "
                       "GROUP BY p.product_id, p.name, p.stock_quantity, p.unit, p.unit_size "
                       "ORDER BY p.product_id";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch transaction data: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n" << string(150, '=') << endl;
        cout << setw(75) << "TRANSACTION PRODUCTS" << setw(76) << "" << endl;
        cout << string(150, '=') << endl;
        cout << left << setw(25) << "Product Name" << " | "
             << setw(15) << "Unit" << " | "
             << setw(20) << "Current Stock" << " | "
             << setw(20) << "PO Received Qty" << " | "
             << setw(40) << "Latest Transaction" << endl;
        cout << string(150, '=') << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            string name = row[1] ? string(row[1]) : "N/A";
            string unit = row[3] ? string(row[3]) : "pcs";
            double stockQty = row[2] ? stod(string(row[2])) : 0.0;
            double unitSize = row[4] ? stod(string(row[4])) : 1.0;
            double packs = unitSize > 0 ? stockQty / unitSize : stockQty;
            string poQty = row[5] ? string(row[5]) : "0";
            string latestTx = row[6] ? string(row[6]) : "No transactions";
            
            // Format stock like inventory
            stringstream ss;
            ss << fixed << setprecision(2) << packs;
            string packsStr = ss.str();
            while (!packsStr.empty() && packsStr.back() == '0') packsStr.pop_back();
            if (!packsStr.empty() && packsStr.back() == '.') packsStr.pop_back();
            
            string unitLower = unit;
            transform(unitLower.begin(), unitLower.end(), unitLower.begin(), ::tolower);
            
            string stockDisplay;
            if (unitLower == "kg" || unitLower == "g" || unitLower == "litre" || unitLower == "ml") {
                stockDisplay = packsStr;
            } else {
                stockDisplay = packsStr + " " + unit;
            }
            
            // Truncate long strings
            if (name.length() > 24) name = name.substr(0, 21) + "...";
            if (unit.length() > 14) unit = unit.substr(0, 11) + "...";
            if (latestTx.length() > 39) latestTx = latestTx.substr(0, 36) + "...";
            
            // Format PO Received Qty like Current Stock (hide unit for kg/g/litre/ml)
            string poQtyDisplay;
            if (unitLower == "kg" || unitLower == "g" || unitLower == "litre" || unitLower == "ml") {
                poQtyDisplay = poQty; // number only
            } else {
                poQtyDisplay = poQty + " " + unit; // with unit
            }
            
            cout << left << setw(25) << name << " | "
                 << setw(15) << unit << " | "
                 << setw(20) << stockDisplay << " | "
                 << setw(20) << poQtyDisplay << " | "
                 << setw(40) << latestTx << endl;
        }
        cout << string(150, '=') << endl;
        cout << "\nTotal products: " << count << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Stocks Submenu - Contains all product operations
void InventoryManager::stocksMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "              STOCKS                    " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Add New Product" << endl;
        cout << "2. View All Products" << endl;
        cout << "3. View Product Details" << endl;
        cout << "4. Update Product" << endl;
        cout << "5. Delete Product" << endl;
        cout << "6. Transaction Products" << endl;
        cout << "7. Print Barcode Products" << endl;
        cout << "8. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 8) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-8: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          ADD NEW PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            addProduct(db);
            cout << "\n";
            viewInventory(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          VIEW ALL PRODUCTS             " << endl;
            cout << "========================================\n" << endl;
            viewInventory(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        VIEW PRODUCT DETAILS            " << endl;
            cout << "========================================\n" << endl;
            
            // Show all products first so user can reference names
            cout << "Available Products:\n" << endl;
            viewInventory(db);
            cout << "\n" << endl;
            
            string productName;
            cout << "Enter Product Name (or part of name, 0 to go back): ";
            getline(cin >> ws, productName);
            
            if (productName == "0") {
                continue;
            }
            
            if (productName.empty()) {
                cout << "[ERROR] Product name cannot be empty." << endl;
            } else {
                viewProduct(db, productName);
            }
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 4) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          UPDATE PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            // Show current products first for easier selection
            viewInventory(db);
            cout << "\n";
            updateProduct(db);
            cout << "\n";
            viewInventory(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 5) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          DELETE PRODUCT               " << endl;
            cout << "========================================\n" << endl;
            deleteProduct(db);
            cout << "\n";
            viewInventory(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 6) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        TRANSACTION PRODUCTS            " << endl;
            cout << "========================================\n" << endl;
            viewTransactionProducts(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 7) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "           PRINT BARCODE               " << endl;
            cout << "========================================\n" << endl;
            printBarcode(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 8) {
            cout << "\nReturning to previous menu..." << endl;
            break;
        }
    }
}
// Inventory Page - Main menu with submenus
void InventoryManager::inventoryPage(Database& db, PurchaseOrderManager* poMgr, 
                                      ReceivingManager* recvMgr, SalesManager* salesMgr) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "+" << string(62, '=') << "+" << endl;
        cout << "|" << setw(41) << "INVENTORY MANAGEMENT" << setw(22) << "|" << endl;
        cout << "|" << setw(42) << "Comprehensive Stockroom" << setw(21) << "|" << endl;
        cout << "+" << string(62, '=') << "+" << endl;
        cout << "\n+ OPERATIONS" << string(51, ' ') << "+" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "|  1. [STOCKS]           Manage Product Inventory" << setw(14) << "|" << endl;
        cout << "|  2. [PURCHASE]         Place Purchase Orders" << setw(18) << "|" << endl;
        cout << "|  3. [RECEIVING]        Receive Order Items" << setw(20) << "|" << endl;
        cout << "|  4. [SALES]            Process Sales Transactions" << setw(11) << "|" << endl;
        cout << "|  5. [BACK]             Return to Main Menu" << setw(20) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "+" << string(62, '-') << "+" << endl;
        cout << "\nSelect option (1-5): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            stocksMenu(db);
        }
        else if (choice == 2) {
            // Purchase Order module
            if (poMgr != nullptr) {
                poMgr->purchaseOrderPage(db);
            } else {
                cout << "[ERROR] Purchase Order manager not available." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
        else if (choice == 3) {
            // Receiving module
            if (recvMgr != nullptr) {
                recvMgr->receivingPage(db);
            } else {
                cout << "[ERROR] Receiving manager not available." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
        else if (choice == 4) {
            // Process Sale module
            if (salesMgr != nullptr) {
                salesMgr->salesPage(db, 1);
            } else {
                cout << "[ERROR] Sales manager not available." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
        else if (choice == 5) {
            break;
        }
    }
}

// Inventory Admin Submenu - For INVENTORY ADMIN role
void InventoryManager::inventoryAdminSubmenu(Database& db, PurchaseOrderManager* poMgr, 
                                             ReceivingManager* recvMgr) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "+" << string(62, '=') << "+" << endl;
        cout << "|" << setw(32) << "INVENTORY MODULE" << setw(31) << "|" << endl;
        cout << "+" << string(62, '=') << "+" << endl;
        cout << "\n+ INVENTORY ADMIN OPERATIONS" << string(35, ' ') << "+" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "|  1. [STOCKS]           Manage Product Inventory" << setw(14) << "|" << endl;
        cout << "|  2. [PURCHASE]         Place Purchase Orders" << setw(18) << "|" << endl;
        cout << "|  3. [RECEIVING]        Receive Order Items" << setw(20) << "|" << endl;
        cout << "|  4. [BACK]             Return to Main Menu" << setw(20) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "+" << string(62, '-') << "+" << endl;
        cout << "\nSelect option (1-4): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-4: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            stocksMenu(db);
        }
        else if (choice == 2) {
            // Purchase Order module
            if (poMgr != nullptr) {
                poMgr->purchaseOrderPage(db);
            } else {
                cout << "[ERROR] Purchase Order manager not available." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
        else if (choice == 3) {
            // Receiving module
            if (recvMgr != nullptr) {
                recvMgr->receivingPage(db);
            } else {
                cout << "[ERROR] Receiving manager not available." << endl;
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
        else if (choice == 4) {
            break;
        }
    }
}