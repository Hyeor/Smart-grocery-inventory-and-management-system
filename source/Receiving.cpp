#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <mysql.h>
#include "Database.h"
#include "Receiving.h"

using namespace std;

// List pending purchase orders for reference
void ReceivingManager::listPendingPurchaseOrders(Database& db) {
    try {
        // Show only POs that are not yet received; allow NULL status values and keep rows even if supplier was removed
        string query = "SELECT po.po_code, s.supplier_name, po.total_amount, po.status, po.order_date "
                   "FROM PurchaseOrder po "
                   "LEFT JOIN Supplier s ON po.supplier_id = s.supplier_id "
                   "WHERE (po.status IS NULL OR po.status IN ('Pending','Approved')) "
                   "ORDER BY po.order_date DESC";

        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch purchase orders: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }

        MYSQL_ROW row;

        cout << "\n=== LIST OF PURCHASE ORDERS (Pending/Not Received) ===" << endl;
        cout << "==================================================================================" << endl;
        cout << "PO Code  | Supplier                 | Total Amount | Status    | Order Date     " << endl;
        cout << "==================================================================================" << endl;

        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            printf("%-8s | %-24s | RM %-9s | %-9s | %s\n",
                   row[0], row[1], row[2], row[3], row[4]);
        }
        cout << "==================================================================================" << endl;

        if (count == 0) {
            cout << "No purchase orders available to receive." << endl;
        }

        mysql_free_result(res);
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Receive a purchase order
void ReceivingManager::receivePurchaseOrder(Database& db) {
    try {
        // Show list first
        listPendingPurchaseOrders(db);

        string poCode;
        cout << "\nEnter P.O. Code to receive (0 to go back): ";
        getline(cin >> ws, poCode);

        if (poCode == "0") {
            return;
        }

        if (poCode.empty()) {
            cout << "[ERROR] P.O. Code cannot be empty!" << endl;
            return;
        }

        // Fetch PO header
        string poQuery = "SELECT po.po_id, po.po_code, s.supplier_name, po.discount_percent, po.tax_percent, po.status "
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

        string status = poRow[5] ? poRow[5] : "";
        if (status == "Received") {
            cout << "[INFO] This purchase order is already marked as Received." << endl;
            mysql_free_result(poRes);
            return;
        }

        int poId = stoi(poRow[0]);
        string supplierName = poRow[2];
        double defaultDiscount = stod(poRow[3]);
        double defaultTax = stod(poRow[4]);
        mysql_free_result(poRes);

        // Fetch PO items
        string itemsQuery = "SELECT poi.product_id, p.name, p.unit, poi.quantity, poi.unit_price "
                            "FROM PurchaseOrderItem poi "
                            "INNER JOIN Product p ON poi.product_id = p.product_id "
                            "INNER JOIN PurchaseOrder po ON poi.po_id = po.po_id "
                            "WHERE po.po_id = " + to_string(poId);

        if (mysql_query(db.conn, itemsQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase order items: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* itemsRes = mysql_store_result(db.conn);
        if (!itemsRes) {
            throw runtime_error("Failed to store result set");
        }

        vector<ReceivingItemInput> items;
        MYSQL_ROW itemRow;

        while ((itemRow = mysql_fetch_row(itemsRes))) {
            ReceivingItemInput it;
            it.productId = stoi(itemRow[0]);
            it.name = itemRow[1];
            it.unit = itemRow[2];
            it.poQuantity = stod(itemRow[3]);
            it.unitPrice = stod(itemRow[4]);
            it.receivedQuantity = 0.0;
            items.push_back(it);
        }
        mysql_free_result(itemsRes);

        if (items.empty()) {
            cout << "[ERROR] No items found for this purchase order." << endl;
            return;
        }

        // Input received quantities
        cout << "\n========================================" << endl;
        cout << "          RECEIVE PURCHASE ORDER         " << endl;
        cout << "========================================" << endl;
        cout << "P.O. Code: " << poCode << endl;
        cout << "Supplier: " << supplierName << endl;
        cout << "========================================" << endl;

        cout << "\n=== ITEMS TO RECEIVE ===" << endl;
        cout << "=====================================================================================" << endl;
        cout << "No. | PO Qty   | Unit      | Item Name                    | Unit Price | Max Allowed" << endl;
        cout << "=====================================================================================" << endl;
        for (size_t i = 0; i < items.size(); i++) {
            printf("%-3zu | %-8.2f | %-9s | %-28s | RM %-7.2f | %-11.2f\n",
                   i + 1, items[i].poQuantity, items[i].unit.c_str(), items[i].name.c_str(), items[i].unitPrice, items[i].poQuantity);
        }
        cout << "=====================================================================================" << endl;

        for (auto& it : items) {
            cout << "Enter received quantity for '" << it.name << "' (max " << fixed << setprecision(2) << it.poQuantity << "): ";
            double qty;
            while (!(cin >> qty) || qty < 0 || qty > it.poQuantity) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter value between 0 and " << it.poQuantity << ": ";
            }
            cin.ignore();
            it.receivedQuantity = qty;
        }

        // Ensure at least one item received
        double subTotal = 0.0;
        for (const auto& it : items) {
            subTotal += it.receivedQuantity * it.unitPrice;
        }
        if (subTotal <= 0) {
            cout << "[ERROR] No quantities received. Nothing to update." << endl;
            return;
        }

        // Discount and tax input with defaults
        cout << "\nEnter discount percentage (0-100, press Enter to keep " << defaultDiscount << "): ";
        string discInput;
        getline(cin, discInput);
        double discount = defaultDiscount;
        if (!discInput.empty()) {
            try {
                discount = stod(discInput);
            } catch (...) {
                discount = defaultDiscount;
            }
        }
        if (discount < 0) discount = 0;
        if (discount > 100) discount = 100;

        cout << "Enter tax percentage (0-100, press Enter to keep " << defaultTax << "): ";
        string taxInput;
        getline(cin, taxInput);
        double tax = defaultTax;
        if (!taxInput.empty()) {
            try {
                tax = stod(taxInput);
            } catch (...) {
                tax = defaultTax;
            }
        }
        if (tax < 0) tax = 0;
        if (tax > 100) tax = 100;

        double discountAmount = (subTotal * discount) / 100.0;
        double afterDiscount = subTotal - discountAmount;
        double taxAmount = (afterDiscount * tax) / 100.0;
        double total = afterDiscount + taxAmount;

        // Show summary
        cout << "\n=== RECEIVING SUMMARY ===" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << fixed << setprecision(2);
        cout << "Sub Total:                                  RM " << subTotal << endl;
        cout << "Discount (" << discount << "%):                            RM " << discountAmount << endl;
        cout << "Tax (" << tax << "%):                                 RM " << taxAmount << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "Total:                                      RM " << total << endl;
        cout << "-----------------------------------------------------" << endl;

        cout << "\nConfirm receive and update PO status to 'Received'? (Y/N): ";
        char confirm;
        cin >> confirm;
        cin.ignore();

        if (confirm != 'Y' && confirm != 'y') {
            cout << "[INFO] Receive operation cancelled." << endl;
            return;
        }

        // Insert receiving record
        string insertReceiving = "INSERT INTO Receiving (po_id, sub_total, discount_percent, discount_amount, tax_percent, tax_amount, total_amount, status) VALUES (" +
                                 to_string(poId) + ", " + to_string(subTotal) + ", " + to_string(discount) + ", " + to_string(discountAmount) + ", " +
                                 to_string(tax) + ", " + to_string(taxAmount) + ", " + to_string(total) + ", 'Received')";
        db.executeQuery(insertReceiving);

        // Get receiving id
        string getIdQuery = "SELECT LAST_INSERT_ID()";
        if (mysql_query(db.conn, getIdQuery.c_str())) {
            throw runtime_error("Failed to get receiving ID");
        }
        MYSQL_RES* idRes = mysql_store_result(db.conn);
        MYSQL_ROW idRow = mysql_fetch_row(idRes);
        int receiveId = stoi(idRow[0]);
        mysql_free_result(idRes);

        // Insert receiving items
        for (const auto& it : items) {
            if (it.receivedQuantity <= 0) continue; // skip zero
            double lineTotal = it.receivedQuantity * it.unitPrice;
            string insertItem = "INSERT INTO ReceivingItem (receiving_id, product_id, received_qty, unit_price, total_price) VALUES (" +
                                 to_string(receiveId) + ", " + to_string(it.productId) + ", " + to_string(it.receivedQuantity) + ", " +
                                 to_string(it.unitPrice) + ", " + to_string(lineTotal) + ")";
            db.executeQuery(insertItem);
        }

        // Update PO totals and status
        string updatePO = "UPDATE PurchaseOrder SET sub_total = " + to_string(subTotal) +
                          ", discount_percent = " + to_string(discount) +
                          ", discount_amount = " + to_string(discountAmount) +
                          ", tax_percent = " + to_string(tax) +
                          ", tax_amount = " + to_string(taxAmount) +
                          ", total_amount = " + to_string(total) +
                          ", status = 'Received' WHERE po_id = " + to_string(poId);
        db.executeQuery(updatePO);

        cout << "\n[OK] Purchase Order received successfully!" << endl;
        cout << "     P.O. Code: " << poCode << endl;
        cout << "     Total Received Amount: RM" << fixed << setprecision(2) << total << endl;

    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Receiving menu
void ReceivingManager::receivingMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "           RECEIVING MANAGEMENT          " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Receive Purchase Order" << endl;
        cout << "2. List Pending Purchase Orders" << endl;
        cout << "3. Back to Previous Menu" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";

        while (!(cin >> choice) || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-3: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            receivePurchaseOrder(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 2) {
            system("cls");
            listPendingPurchaseOrders(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            break;
        }
    }
}

void ReceivingManager::receivingPage(Database& db) {
    receivingMenu(db);
}
