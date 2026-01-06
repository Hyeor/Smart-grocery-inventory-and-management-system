#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <mysql.h>
#include "Database.h"
#include "PurchaseOrder.h"

using namespace std;

// Generate unique PO Code in format PO-0001
string PurchaseOrderManager::generatePOCode(Database& db) {
    try {
        string query = "SELECT po_code FROM PurchaseOrder ORDER BY po_id DESC LIMIT 1";
        
        if (mysql_query(db.conn, query.c_str())) {
            // If table is empty or doesn't exist, start with PO-0001
            return "PO-0001";
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            return "PO-0001";
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row && row[0]) {
            string lastCode = row[0];
            // Extract number from PO-XXXX
            int num = stoi(lastCode.substr(3)) + 1;
            mysql_free_result(res);
            
            // Format as PO-XXXX with leading zeros
            char buffer[10];
            sprintf(buffer, "PO-%04d", num);
            return string(buffer);
        }
        
        mysql_free_result(res);
        return "PO-0001";
        
    } catch (const exception& e) {
        return "PO-0001";
    }
}

// Display available active suppliers
void PurchaseOrderManager::displaySuppliers(Database& db) {
    try {
        string query = "SELECT supplier_id, supplier_name, contact_person FROM Supplier WHERE status = 'Active' ORDER BY supplier_name";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch suppliers: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        
        cout << "\n=== AVAILABLE SUPPLIERS ===" << endl;
        cout << "==========================================================" << endl;
        cout << "ID   | Supplier Name                | Contact Person      " << endl;
        cout << "==========================================================" << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            printf("%-4s | %-28s | %-20s\n", row[0], row[1], row[2]);
        }
        cout << "==========================================================" << endl;
        
        if (count == 0) {
            cout << "No active suppliers found." << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Display items (products) by selected supplier
void PurchaseOrderManager::displayItemsBySupplier(Database& db, int supplierID) {
    try {
        string query = "SELECT product_id, name, unit, cost_price, stock_quantity FROM Product WHERE supplier_id = " 
                       + to_string(supplierID) + " ORDER BY name";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch items: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        
        cout << "\n=== ITEMS FROM SELECTED SUPPLIER ===" << endl;
        cout << "=========================================================================" << endl;
        cout << "ID   | Item Name                    | Unit      | Cost Price | Stock    " << endl;
        cout << "=========================================================================" << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            printf("%-4s | %-28s | %-9s | RM %-7s | %-8s\n", 
                   row[0], row[1], row[2], row[3], row[4]);
        }
        cout << "=========================================================================" << endl;
        
        if (count == 0) {
            cout << "No items found for this supplier." << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Get item details from database
bool PurchaseOrderManager::getItemDetails(Database& db, int itemId, string& itemName, string& unit, double& costPrice) {
    try {
        string query = "SELECT name, unit, cost_price FROM Product WHERE product_id = " + to_string(itemId);
        
        if (mysql_query(db.conn, query.c_str())) {
            return false;
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            return false;
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row) {
            itemName = row[0];
            unit = row[1];
            costPrice = stod(row[2]);
            mysql_free_result(res);
            return true;
        }
        
        mysql_free_result(res);
        return false;
        
    } catch (const exception& e) {
        return false;
    }
}

// Display current items in PO list
void PurchaseOrderManager::displayPOItems(const vector<POItem>& items) {
    cout << "\n=== ITEMS IN PURCHASE ORDER ===" << endl;
    cout << "=====================================================================================" << endl;
    cout << "No. | Qty      | Unit      | Item Name                    | Cost      | Total      " << endl;
    cout << "=====================================================================================" << endl;
    
    for (size_t i = 0; i < items.size(); i++) {
        printf("%-3zu | %-8.2f | %-9s | %-28s | RM %-6.2f | RM %-8.2f\n",
               i + 1, items[i].quantity, items[i].unit.c_str(), 
               items[i].itemName.c_str(), items[i].costPrice, items[i].totalCost);
    }
    cout << "=====================================================================================" << endl;
}

// Calculate subtotal from all items
double PurchaseOrderManager::calculateSubTotal(const vector<POItem>& items) {
    double subTotal = 0.0;
    for (const auto& item : items) {
        subTotal += item.totalCost;
    }
    return subTotal;
}

// Display PO summary with calculations
void PurchaseOrderManager::displayPOSummary(const vector<POItem>& items, double discount, double tax) {
    double subTotal = calculateSubTotal(items);
    double discountAmount = (subTotal * discount) / 100.0;
    double afterDiscount = subTotal - discountAmount;
    double taxAmount = (afterDiscount * tax) / 100.0;
    double total = afterDiscount + taxAmount;
    
    cout << "\n=== PURCHASE ORDER SUMMARY ===" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << fixed << setprecision(2);
    cout << "Sub Total:                                  RM " << subTotal << endl;
    cout << "Discount (" << discount << "%):                            RM " << discountAmount << endl;
    cout << "Tax (" << tax << "%):                                 RM " << taxAmount << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "Total:                                      RM " << total << endl;
    cout << "-----------------------------------------------------" << endl;
}

// CREATE - Create new purchase order
void PurchaseOrderManager::createPurchaseOrder(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        system("cls");
        cout << "\n========================================" << endl;
        cout << "      CREATE NEW PURCHASE ORDER         " << endl;
        cout << "========================================" << endl;
        
        // Step 1: Generate PO Code
        string poCode = generatePOCode(db);
        cout << "\nP.O. Code: " << poCode << endl;
        
        // Step 2: Select Supplier
        displaySuppliers(db);
        cout << "\nSelect Supplier ID (0 to go back): ";
        int supplierID;
        while (!(cin >> supplierID) || supplierID < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid Supplier ID (0 to go back): ";
        }
        cin.ignore();
        
        if (supplierID == 0) {
            cout << "\n[INFO] Purchase order creation cancelled." << endl;
            return;
        }
        
        // Verify supplier exists and is active
        string verifyQuery = "SELECT supplier_name FROM Supplier WHERE supplier_id = " 
                             + to_string(supplierID) + " AND status = 'Active'";
        
        if (mysql_query(db.conn, verifyQuery.c_str())) {
            cout << "[ERROR] Database query failed: " << mysql_error(db.conn) << endl;
            return;
        }
        
        MYSQL_RES* verifyRes = mysql_store_result(db.conn);
        if (!verifyRes) {
            cout << "[ERROR] Database query failed: Unable to store result set" << endl;
            return;
        }
        
        MYSQL_ROW supplierRow = mysql_fetch_row(verifyRes);
        if (!supplierRow) {
            cout << "[ERROR] Supplier ID not found or not active!" << endl;
            mysql_free_result(verifyRes);
            return;
        }
        
        string supplierName = supplierRow[0];
        mysql_free_result(verifyRes);
        
        cout << "\nSelected Supplier: " << supplierName << endl;
        
        // Step 3: Add items to purchase order
        vector<POItem> items;
        bool addingItems = true;
        
        while (addingItems) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "      CREATE NEW PURCHASE ORDER         " << endl;
            cout << "========================================" << endl;
            cout << "P.O. Code: " << poCode << endl;
            cout << "Supplier: " << supplierName << endl;
            cout << "========================================" << endl;
            
            // Display current items if any
            if (!items.empty()) {
                displayPOItems(items);
            }
            
            // Display available items from supplier
            displayItemsBySupplier(db, supplierID);
            
            cout << "\n=== ADD ITEM TO LIST ===" << endl;
            cout << "Enter Item ID (0 to finish adding items, -1 to go back): ";
            int itemId;
            while (!(cin >> itemId)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter Item ID: ";
            }
            
            if (itemId == -1) {
                cout << "\n[INFO] Purchase order creation cancelled." << endl;
                cin.ignore();
                return;
            }
            
            if (itemId == 0) {
                addingItems = false;
                cin.ignore();
                break;
            }
            
            // Get item details
            string itemName, unit;
            double costPrice;
            
            if (!getItemDetails(db, itemId, itemName, unit, costPrice)) {
                cout << "[ERROR] Item not found!" << endl;
                cin.ignore();
                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }
            
            // Verify item belongs to selected supplier
            string checkQuery = "SELECT supplier_id FROM Product WHERE product_id = " + to_string(itemId);
            if (mysql_query(db.conn, checkQuery.c_str())) {
                cout << "[ERROR] Failed to verify item." << endl;
                cin.ignore();
                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }
            
            MYSQL_RES* checkRes = mysql_store_result(db.conn);
            MYSQL_ROW checkRow = mysql_fetch_row(checkRes);
            
            if (!checkRow || stoi(checkRow[0]) != supplierID) {
                cout << "[ERROR] This item does not belong to the selected supplier!" << endl;
                mysql_free_result(checkRes);
                cin.ignore();
                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }
            mysql_free_result(checkRes);
            
            // Get quantity (numbers only)
            cout << "Enter Quantity: ";
            double quantity;
            while (!(cin >> quantity) || quantity <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid quantity: ";
            }
            cin.ignore();
            
            // Calculate total cost for this item
            double totalCost = quantity * costPrice;
            
            // Add item to list
            POItem item;
            item.itemId = itemId;
            item.itemName = itemName;
            item.unit = unit;
            item.quantity = quantity;
            item.costPrice = costPrice;
            item.totalCost = totalCost;
            items.push_back(item);
            
            cout << "\n[OK] Item added to purchase order!" << endl;
            cout << "Press Enter to continue...";
            cin.get();
        }
        
        // Check if at least one item was added
        if (items.empty()) {
            cout << "\n[ERROR] Cannot create purchase order without items!" << endl;
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        // Step 4: Enter discount and tax (optional)
        system("cls");
        cout << "\n========================================" << endl;
        cout << "      CREATE NEW PURCHASE ORDER         " << endl;
        cout << "========================================" << endl;
        cout << "P.O. Code: " << poCode << endl;
        cout << "Supplier: " << supplierName << endl;
        cout << "========================================" << endl;
        
        displayPOItems(items);
        
        double discount = 0.0;
        double tax = 0.0;
        
        cout << "\n=== DISCOUNT & TAX ===" << endl;
        cout << "Enter discount percentage (0 for no discount, -1 to cancel): ";
        while (!(cin >> discount) || discount < -1 || discount > 100) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter discount (0-100%, -1 to cancel): ";
        }
        
        if (discount == -1) {
            cout << "\n[INFO] Purchase order creation cancelled." << endl;
            cin.ignore();
            return;
        }
        
        cout << "Enter tax percentage (0 for no tax, -1 to cancel): ";
        while (!(cin >> tax) || tax < -1 || tax > 100) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter tax (0-100%, -1 to cancel): ";
        }
        cin.ignore();
        
        if (tax == -1) {
            cout << "\n[INFO] Purchase order creation cancelled." << endl;
            return;
        }
        
        // Step 5: Display summary and confirm
        system("cls");
        cout << "\n========================================" << endl;
        cout << "      CREATE NEW PURCHASE ORDER         " << endl;
        cout << "========================================" << endl;
        cout << "P.O. Code: " << poCode << endl;
        cout << "Supplier: " << supplierName << endl;
        cout << "========================================" << endl;
        
        displayPOItems(items);
        displayPOSummary(items, discount, tax);
        
        // Optional: Add remarks
        string remarks;
        cout << "\nEnter remarks (optional, press Enter to skip): ";
        getline(cin, remarks);
        
        // Confirm save
        cout << "\nDo you want to save this purchase order? (Y/N, B to go back): ";
        char confirm;
        cin >> confirm;
        cin.ignore();
        
        if (confirm == 'B' || confirm == 'b') {
            cout << "\n[INFO] Returned to previous step." << endl;
            return;
        }
        
        if (confirm != 'Y' && confirm != 'y') {
            cout << "\n[INFO] Purchase order creation cancelled." << endl;
            return;
        }
        
        // Step 6: Save to database
        double subTotal = calculateSubTotal(items);
        double discountAmount = (subTotal * discount) / 100.0;
        double afterDiscount = subTotal - discountAmount;
        double taxAmount = (afterDiscount * tax) / 100.0;
        double total = afterDiscount + taxAmount;
        
        // Insert purchase order
        string insertPOQuery = "INSERT INTO PurchaseOrder (po_code, supplier_id, sub_total, discount_percent, "
                               "discount_amount, tax_percent, tax_amount, total_amount, remarks, status) VALUES ('"
                               + poCode + "', " + to_string(supplierID) + ", " + to_string(subTotal) + ", "
                               + to_string(discount) + ", " + to_string(discountAmount) + ", " + to_string(tax) + ", "
                               + to_string(taxAmount) + ", " + to_string(total) + ", '" + remarks + "', 'Pending')";
        
        db.executeQuery(insertPOQuery);
        
        // Get the inserted PO ID
        string getIDQuery = "SELECT LAST_INSERT_ID()";
        if (mysql_query(db.conn, getIDQuery.c_str())) {
            throw runtime_error("Failed to get PO ID");
        }
        
        MYSQL_RES* idRes = mysql_store_result(db.conn);
        MYSQL_ROW idRow = mysql_fetch_row(idRes);
        int poID = stoi(idRow[0]);
        mysql_free_result(idRes);
        
        // Insert purchase order items
        for (const auto& item : items) {
            string insertItemQuery = "INSERT INTO PurchaseOrderItem (po_id, product_id, quantity, unit_price, total_price) VALUES ("
                                     + to_string(poID) + ", " + to_string(item.itemId) + ", " + to_string(item.quantity) + ", "
                                     + to_string(item.costPrice) + ", " + to_string(item.totalCost) + ")";
            db.executeQuery(insertItemQuery);
        }
        
        cout << "\n[OK] Purchase Order Created Successfully!" << endl;
        cout << "     P.O. Code: " << poCode << endl;
        cout << "     Total Amount: RM" << fixed << setprecision(2) << total << endl;
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] Failed to create purchase order: " << e.what() << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// READ - View all purchase orders
void PurchaseOrderManager::viewAllPurchaseOrders(Database& db) {
    try {
        string query = "SELECT po.po_id, po.po_code, s.supplier_name, po.total_amount, po.status, po.order_date "
                       "FROM PurchaseOrder po "
                       "INNER JOIN Supplier s ON po.supplier_id = s.supplier_id "
                       "ORDER BY po.order_date DESC";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch purchase orders: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        
        cout << "\n=== LIST OF PURCHASE ORDERS ===" << endl;
        cout << "==================================================================================" << endl;
        cout << "PO Code  | Supplier                 | Total Amount | Status    | Order Date     " << endl;
        cout << "==================================================================================" << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            printf("%-8s | %-24s | RM %-9s | %-9s | %s\n",
                   row[1], row[2], row[3], row[4], row[5]);
        }
        cout << "==================================================================================" << endl;
        printf("Showing 1 to %d of %d entries\n", count, count);
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View purchase order details
void PurchaseOrderManager::viewPurchaseOrderDetails(Database& db) {
    try {
        // First, display list of all purchase orders for reference
        cout << "\n=== AVAILABLE PURCHASE ORDERS ===" << endl;
        string listQuery = "SELECT po.po_code, s.supplier_name, po.total_amount, po.status, po.order_date "
                           "FROM PurchaseOrder po "
                           "INNER JOIN Supplier s ON po.supplier_id = s.supplier_id "
                           "ORDER BY po.order_date DESC LIMIT 10";
        
        if (mysql_query(db.conn, listQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase orders: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* listRes = mysql_store_result(db.conn);
        if (!listRes) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW listRow;
        
        cout << "==================================================================================" << endl;
        cout << "PO Code  | Supplier                 | Total Amount | Status    | Order Date     " << endl;
        cout << "==================================================================================" << endl;
        
        int count = 0;
        while ((listRow = mysql_fetch_row(listRes))) {
            count++;
            printf("%-8s | %-24s | RM %-9s | %-9s | %s\n",
                   listRow[0], listRow[1], listRow[2], listRow[3], listRow[4]);
        }
        cout << "==================================================================================" << endl;
        
        if (count == 0) {
            cout << "No purchase orders found." << endl;
            mysql_free_result(listRes);
            return;
        }
        
        mysql_free_result(listRes);
        
        string poCode;
        cout << "\nEnter P.O. Code to view details (0 to go back): ";
        getline(cin >> ws, poCode);
        
        if (poCode == "0") {
            return;
        }
        
        if (poCode.empty()) {
            cout << "[ERROR] P.O. Code cannot be empty!" << endl;
            return;
        }
        
        // Get purchase order details
        string poQuery = "SELECT po.po_code, s.supplier_name, s.contact_person, po.sub_total, "
                         "po.discount_percent, po.discount_amount, po.tax_percent, po.tax_amount, "
                         "po.total_amount, po.remarks, po.status, po.order_date "
                         "FROM PurchaseOrder po "
                         "INNER JOIN Supplier s ON po.supplier_id = s.supplier_id "
                         "WHERE po.po_code = '" + poCode + "'";
        
        if (mysql_query(db.conn, poQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase order: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* poRes = mysql_store_result(db.conn);
        if (!poRes) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW poRow = mysql_fetch_row(poRes);
        
        if (!poRow) {
            cout << "[ERROR] Purchase order not found." << endl;
            mysql_free_result(poRes);
            return;
        }
        
        // Display PO header
        cout << "\n=== PURCHASE ORDER DETAILS ===" << endl;
        cout << "========================================" << endl;
        cout << "P.O. Code: " << poRow[0] << endl;
        cout << "Supplier: " << poRow[1] << endl;
        cout << "Contact Person: " << poRow[2] << endl;
        cout << "Status: " << poRow[10] << endl;
        cout << "Order Date: " << poRow[11] << endl;
        cout << "========================================" << endl;
        
        string subTotal = poRow[3];
        string discountPercent = poRow[4];
        string discountAmount = poRow[5];
        string taxPercent = poRow[6];
        string taxAmount = poRow[7];
        string totalAmount = poRow[8];
        string remarks = poRow[9] ? poRow[9] : "";
        
        mysql_free_result(poRes);
        
        // Get purchase order items
        string itemsQuery = "SELECT p.name, poi.quantity, p.unit, poi.unit_price, poi.total_price "
                            "FROM PurchaseOrderItem poi "
                            "INNER JOIN Product p ON poi.product_id = p.product_id "
                            "INNER JOIN PurchaseOrder po ON poi.po_id = po.po_id "
                            "WHERE po.po_code = '" + poCode + "'";
        
        if (mysql_query(db.conn, itemsQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase order items: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* itemsRes = mysql_store_result(db.conn);
        if (!itemsRes) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW itemRow;
        
        cout << "\n=== ITEMS ===" << endl;
        cout << "=====================================================================================" << endl;
        cout << "Item Name                    | Qty      | Unit      | Unit Price | Total Price   " << endl;
        cout << "=====================================================================================" << endl;
        
        while ((itemRow = mysql_fetch_row(itemsRes))) {
            printf("%-28s | %-8s | %-9s | RM %-7s | RM %-11s\n",
                   itemRow[0], itemRow[1], itemRow[2], itemRow[3], itemRow[4]);
        }
        cout << "=====================================================================================" << endl;
        
        mysql_free_result(itemsRes);
        
        // Display summary
        cout << "\n=== SUMMARY ===" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "Sub Total:                                  RM " << subTotal << endl;
        cout << "Discount (" << discountPercent << "%):                            RM " << discountAmount << endl;
        cout << "Tax (" << taxPercent << "%):                                 RM " << taxAmount << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "Total:                                      RM " << totalAmount << endl;
        cout << "-----------------------------------------------------" << endl;
        
        if (!remarks.empty()) {
            cout << "\nRemarks: " << remarks << endl;
        }
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Purchase Order menu
void PurchaseOrderManager::purchaseOrderMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "      PURCHASE ORDER MANAGEMENT         " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Create New Purchase Order" << endl;
        cout << "2. View All Purchase Orders" << endl;
        cout << "3. View Purchase Order Details" << endl;
        cout << "4. Back to Previous Menu" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-4: ";
        }
        cin.ignore(10000, '\n');
        
        if (choice == 1) {
            createPurchaseOrder(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        VIEW ALL PURCHASE ORDERS        " << endl;
            cout << "========================================\n" << endl;
            viewAllPurchaseOrders(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "      VIEW PURCHASE ORDER DETAILS       " << endl;
            cout << "========================================\n" << endl;
            viewPurchaseOrderDetails(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 4) {
            break;
        }
    }
}

// Purchase order page with clean layout (wrapper for menu)
void PurchaseOrderManager::purchaseOrderPage(Database& db) {
    purchaseOrderMenu(db);
}
