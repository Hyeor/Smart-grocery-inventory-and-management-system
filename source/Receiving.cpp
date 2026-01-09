#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <ctime>
#include <mysql.h>
#include "Database.h"
#include "Receiving.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

using namespace std;

// ANSI Color codes for console output
#define COLOR_RESET     "\033[0m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_RED       "\033[31m"

// Helper function to get colored status
inline string getColoredStatus(const string& status) {
    if (status == "Pending" || status == "Approved") {
        return string(COLOR_YELLOW) + status + string(COLOR_RESET);
    } else if (status == "Received") {
        return string(COLOR_GREEN) + status + string(COLOR_RESET);
    }
    return status;
}

// List all purchase orders (both Pending and Received)
void ReceivingManager::listPendingPurchaseOrders(Database& db) {
    try {
        // Show ALL purchase orders (Pending, Approved, and Received)
        // Using COALESCE to handle NULL supplier names and show 'Unknown Supplier' instead
        string query = "SELECT po.po_code, COALESCE(s.supplier_name, 'Unknown Supplier'), "
                   "po.total_amount, po.status, po.order_date "
                   "FROM PurchaseOrder po "
                   "LEFT JOIN Supplier s ON po.supplier_id = s.supplier_id "
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
            string coloredStatus = getColoredStatus(row[3] ? row[3] : "");
            printf("%-8s | %-24s | RM %-9s | ", row[0], row[1], row[2]);
            cout << coloredStatus;
            printf(" | %s\n", row[4]);
        }
        cout << "==================================================================================" << endl;
        printf("Showing 1 to %d of %d entries\n", count, count);

        mysql_free_result(res);
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Generate Received Order Report in HTML format
void ReceivingManager::generateReceivedOrderReport(Database& db) {
    try {
        // Show list of received purchase orders
        cout << "\n=== RECEIVED PURCHASE ORDERS ===" << endl;
        string listQuery = "SELECT po.po_code, COALESCE(s.supplier_name, 'Unknown Supplier'), "
                       "po.total_amount, DATE_FORMAT(po.received_date, '%Y-%m-%d %H:%i:%s') "
                       "FROM PurchaseOrder po "
                       "LEFT JOIN Supplier s ON po.supplier_id = s.supplier_id "
                       "WHERE po.status = 'Received' "
                       "ORDER BY po.received_date DESC";

        if (mysql_query(db.conn, listQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase orders: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* listRes = mysql_store_result(db.conn);
        if (!listRes) {
            throw runtime_error("Failed to store result set");
        }

        MYSQL_ROW listRow;

        cout << "============================================================================================" << endl;
        cout << "PO Code  | Supplier                 | Total Amount | Received Date & Time       " << endl;
        cout << "============================================================================================" << endl;

        int count = 0;
        while ((listRow = mysql_fetch_row(listRes))) {
            count++;
            printf("%-8s | %-24s | RM %-9s | %s\n",
                   listRow[0], listRow[1], listRow[2], listRow[3] ? listRow[3] : "Not Recorded");
        }
        cout << "============================================================================================" << endl;

        mysql_free_result(listRes);

        if (count == 0) {
            cout << "No received orders found." << endl;
            return;
        }

        string poCode;
        cout << "\nEnter P.O. Code to generate report (0 to cancel): ";
        getline(cin >> ws, poCode);

        if (poCode == "0") {
            return;
        }

        if (poCode.empty()) {
            cout << "[ERROR] P.O. Code cannot be empty!" << endl;
            return;
        }

        // Fetch PO header details
        string poQuery = "SELECT po.po_code, s.supplier_name, s.supplier_id, po.sub_total, "
                         "po.discount_percent, po.discount_amount, po.tax_percent, po.tax_amount, "
                         "po.total_amount, po.remarks, DATE_FORMAT(po.received_date, '%Y-%m-%d %H:%i:%s') "
                         "FROM PurchaseOrder po "
                         "LEFT JOIN Supplier s ON po.supplier_id = s.supplier_id "
                         "WHERE po.po_code = '" + poCode + "' AND po.status = 'Received'";

        if (mysql_query(db.conn, poQuery.c_str())) {
            throw runtime_error("Failed to fetch purchase order: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* poRes = mysql_store_result(db.conn);
        if (!poRes) {
            throw runtime_error("Failed to store result set");
        }

        MYSQL_ROW poRow = mysql_fetch_row(poRes);
        if (!poRow) {
            cout << "[ERROR] Purchase order not found or not received yet." << endl;
            mysql_free_result(poRes);
            return;
        }

        string supplierName = poRow[1] ? poRow[1] : "Unknown Supplier";
        string supplierID = poRow[2] ? poRow[2] : "N/A";
        double subTotal = stod(poRow[3]);
        double discountPercent = stod(poRow[4]);
        double discountAmount = stod(poRow[5]);
        double taxPercent = stod(poRow[6]);
        double taxAmount = stod(poRow[7]);
        double totalAmount = stod(poRow[8]);
        string remarks = poRow[9] ? poRow[9] : "";
        string receivedDate = poRow[10] ? poRow[10] : "Not Recorded";

        mysql_free_result(poRes);

        // Fetch PO items
        string itemsQuery = "SELECT p.name, p.unit, ri.received_qty, ri.unit_price, ri.total_price "
                            "FROM ReceivingItem ri "
                            "INNER JOIN Product p ON ri.product_id = p.product_id "
                            "INNER JOIN Receiving r ON ri.receiving_id = r.receiving_id "
                            "INNER JOIN PurchaseOrder po ON r.po_id = po.po_id "
                            "WHERE po.po_code = '" + poCode + "'";

        if (mysql_query(db.conn, itemsQuery.c_str())) {
            throw runtime_error("Failed to fetch items: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* itemsRes = mysql_store_result(db.conn);
        if (!itemsRes) {
            throw runtime_error("Failed to store result set");
        }

        // Generate HTML report filename with PO code and received date
        string filenameSafeDate = receivedDate;
        // Replace spaces, colons, and other characters with underscores for filename safety
        for (char& c : filenameSafeDate) {
            if (c == ' ' || c == ':' || c == '-') {
                c = '_';
            }
        }
        string filename = "reports/" + poCode + "_" + filenameSafeDate + ".html";
        
        // Create reports directory if it doesn't exist
        #ifdef _WIN32
        system("if not exist reports mkdir reports");
        #else
        system("mkdir -p reports");
        #endif

        ofstream htmlFile(filename);
        if (!htmlFile.is_open()) {
            throw runtime_error("Failed to create HTML file");
        }

        // Write HTML
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html lang=\"en\">\n";
        htmlFile << "<head>\n";
        htmlFile << "    <meta charset=\"UTF-8\">\n";
        htmlFile << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        htmlFile << "    <title>" << poCode << "_" << filenameSafeDate << "</title>\n";
        htmlFile << "    <style>\n";
        htmlFile << "        @media print {\n";
        htmlFile << "            @page { size: A4; margin: 20mm; }\n";
        htmlFile << "            @page { margin: 0; }\n";
        htmlFile << "        }\n";
        htmlFile << "        body {\n";
        htmlFile << "            font-family: 'Segoe UI', Arial, sans-serif;\n";
        htmlFile << "            max-width: 800px;\n";
        htmlFile << "            margin: 40px auto;\n";
        htmlFile << "            padding: 30px;\n";
        htmlFile << "            background: white;\n";
        htmlFile << "            box-shadow: 0 0 20px rgba(0,0,0,0.1);\n";
        htmlFile << "        }\n";
        htmlFile << "        .header {\n";
        htmlFile << "            text-align: center;\n";
        htmlFile << "            border-bottom: 3px solid #4CAF50;\n";
        htmlFile << "            padding-bottom: 20px;\n";
        htmlFile << "            margin-bottom: 30px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .header h1 {\n";
        htmlFile << "            color: #2c3e50;\n";
        htmlFile << "            margin: 0;\n";
        htmlFile << "            font-size: 24px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .header h2 {\n";
        htmlFile << "            color: #4CAF50;\n";
        htmlFile << "            margin: 5px 0;\n";
        htmlFile << "            font-size: 20px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .info-section {\n";
        htmlFile << "            margin-bottom: 25px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .info-label {\n";
        htmlFile << "            color: #00BCD4;\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            margin-bottom: 5px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .info-value {\n";
        htmlFile << "            color: #34495e;\n";
        htmlFile << "            margin-bottom: 10px;\n";
        htmlFile << "        }\n";
        htmlFile << "        table {\n";
        htmlFile << "            width: 100%;\n";
        htmlFile << "            border-collapse: collapse;\n";
        htmlFile << "            margin: 20px 0;\n";
        htmlFile << "        }\n";
        htmlFile << "        th {\n";
        htmlFile << "            background-color: #f8f9fa;\n";
        htmlFile << "            padding: 12px;\n";
        htmlFile << "            text-align: left;\n";
        htmlFile << "            border-bottom: 2px solid #dee2e6;\n";
        htmlFile << "            color: #495057;\n";
        htmlFile << "            font-weight: 600;\n";
        htmlFile << "        }\n";
        htmlFile << "        td {\n";
        htmlFile << "            padding: 10px 12px;\n";
        htmlFile << "            border-bottom: 1px solid #dee2e6;\n";
        htmlFile << "        }\n";
        htmlFile << "        .text-right { text-align: right; }\n";
        htmlFile << "        .text-center { text-align: center; }\n";
        htmlFile << "        .summary {\n";
        htmlFile << "            margin-top: 30px;\n";
        htmlFile << "            border-top: 2px solid #dee2e6;\n";
        htmlFile << "            padding-top: 20px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .summary-row {\n";
        htmlFile << "            display: flex;\n";
        htmlFile << "            justify-content: space-between;\n";
        htmlFile << "            padding: 8px 0;\n";
        htmlFile << "        }\n";
        htmlFile << "        .summary-row.total {\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            font-size: 18px;\n";
        htmlFile << "            border-top: 2px solid #dee2e6;\n";
        htmlFile << "            padding-top: 15px;\n";
        htmlFile << "            margin-top: 10px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .remarks-section {\n";
        htmlFile << "            margin-top: 30px;\n";
        htmlFile << "            padding: 15px;\n";
        htmlFile << "            background-color: #e8f5e9;\n";
        htmlFile << "            border-left: 4px solid #4CAF50;\n";
        htmlFile << "        }\n";
        htmlFile << "        .remarks-title {\n";
        htmlFile << "            color: #2c3e50;\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            margin-bottom: 8px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .status {\n";
        htmlFile << "            display: inline-block;\n";
        htmlFile << "            padding: 5px 15px;\n";
        htmlFile << "            background-color: #4CAF50;\n";
        htmlFile << "            color: white;\n";
        htmlFile << "            border-radius: 4px;\n";
        htmlFile << "            font-weight: bold;\n";
        htmlFile << "            margin-top: 10px;\n";
        htmlFile << "        }\n";
        htmlFile << "        .print-button {\n";
        htmlFile << "            background-color: #2196F3;\n";
        htmlFile << "            color: white;\n";
        htmlFile << "            padding: 12px 30px;\n";
        htmlFile << "            border: none;\n";
        htmlFile << "            border-radius: 4px;\n";
        htmlFile << "            cursor: pointer;\n";
        htmlFile << "            font-size: 16px;\n";
        htmlFile << "            margin-top: 20px;\n";
        htmlFile << "            display: block;\n";
        htmlFile << "            margin-left: auto;\n";
        htmlFile << "            margin-right: auto;\n";
        htmlFile << "        }\n";
        htmlFile << "        .print-button:hover {\n";
        htmlFile << "            background-color: #1976D2;\n";
        htmlFile << "        }\n";
        htmlFile << "        @media print {\n";
        htmlFile << "            .print-button { display: none; }\n";
        htmlFile << "        }\n";
        htmlFile << "    </style>\n";
        htmlFile << "</head>\n";
        htmlFile << "<body>\n";
        htmlFile << "    <div class=\"header\">\n";
        htmlFile << "        <h1>GroceryFlow IMS</h1>\n";
        htmlFile << "        <h2>Received Order</h2>\n";
        htmlFile << "    </div>\n\n";
        
        htmlFile << "    <div class=\"info-section\">\n";
        htmlFile << "        <div class=\"info-label\">FROM P.O. Code</div>\n";
        htmlFile << "        <div class=\"info-value\">" << poCode << "</div>\n";
        htmlFile << "        <div class=\"info-label\">Supplier</div>\n";
        htmlFile << "        <div class=\"info-value\">Supplier " << supplierID << " - " << supplierName << "</div>\n";
        htmlFile << "        <div class=\"info-label\">Received Date</div>\n";
        htmlFile << "        <div class=\"info-value\">" << receivedDate << "</div>\n";
        htmlFile << "    </div>\n\n";
        
        htmlFile << "    <h3 style=\"color: #00BCD4; margin-top: 30px;\">Orders</h3>\n";
        htmlFile << "    <table>\n";
        htmlFile << "        <thead>\n";
        htmlFile << "            <tr>\n";
        htmlFile << "                <th>Qty</th>\n";
        htmlFile << "                <th>Unit</th>\n";
        htmlFile << "                <th>Item</th>\n";
        htmlFile << "                <th class=\"text-right\">Cost</th>\n";
        htmlFile << "                <th class=\"text-right\">Total</th>\n";
        htmlFile << "            </tr>\n";
        htmlFile << "        </thead>\n";
        htmlFile << "        <tbody>\n";

        MYSQL_ROW itemRow;
        while ((itemRow = mysql_fetch_row(itemsRes))) {
            htmlFile << "            <tr>\n";
            htmlFile << "                <td class=\"text-center\">" << fixed << setprecision(2) << stod(itemRow[2]) << "</td>\n";
            htmlFile << "                <td>" << itemRow[1] << "</td>\n";
            htmlFile << "                <td>" << itemRow[0] << "</td>\n";
            htmlFile << "                <td class=\"text-right\">" << fixed << setprecision(2) << stod(itemRow[3]) << "</td>\n";
            htmlFile << "                <td class=\"text-right\">" << fixed << setprecision(2) << stod(itemRow[4]) << "</td>\n";
            htmlFile << "            </tr>\n";
        }

        mysql_free_result(itemsRes);

        htmlFile << "        </tbody>\n";
        htmlFile << "    </table>\n\n";
        
        htmlFile << "    <div class=\"summary\">\n";
        htmlFile << "        <div class=\"summary-row\">\n";
        htmlFile << "            <span>Sub Total:</span>\n";
        htmlFile << "            <span>RM " << fixed << setprecision(2) << subTotal << "</span>\n";
        htmlFile << "        </div>\n";
        htmlFile << "        <div class=\"summary-row\">\n";
        htmlFile << "            <span>Discount (" << fixed << setprecision(0) << discountPercent << "%):</span>\n";
        htmlFile << "            <span>RM " << fixed << setprecision(2) << discountAmount << "</span>\n";
        htmlFile << "        </div>\n";
        htmlFile << "        <div class=\"summary-row\">\n";
        htmlFile << "            <span>Tax (" << fixed << setprecision(0) << taxPercent << "%):</span>\n";
        htmlFile << "            <span>RM " << fixed << setprecision(2) << taxAmount << "</span>\n";
        htmlFile << "        </div>\n";
        htmlFile << "        <div class=\"summary-row total\">\n";
        htmlFile << "            <span>Total:</span>\n";
        htmlFile << "            <span>RM " << fixed << setprecision(2) << totalAmount << "</span>\n";
        htmlFile << "        </div>\n";
        htmlFile << "    </div>\n\n";
        
        htmlFile << "    <div class=\"remarks-section\">\n";
        htmlFile << "        <div class=\"remarks-title\">Remarks</div>\n";
        htmlFile << "        <div class=\"status\">RECEIVED</div>\n";
        if (!remarks.empty()) {
            htmlFile << "        <p style=\"margin-top: 10px;\">" << remarks << "</p>\n";
        }
        htmlFile << "    </div>\n\n";
        
        htmlFile << "    <button class=\"print-button\" onclick=\"printPDF()\">Print / Save as PDF</button>\n\n";
        htmlFile << "    <script>\n";
        htmlFile << "        function printPDF() {\n";
        htmlFile << "            // Set the document title for PDF filename\n";
        htmlFile << "            document.title = \"" << poCode << "_" << filenameSafeDate << "\";\n";
        htmlFile << "            // Give browser a moment to update the title\n";
        htmlFile << "            setTimeout(function() {\n";
        htmlFile << "                window.print();\n";
        htmlFile << "            }, 100);\n";
        htmlFile << "        }\n";
        htmlFile << "    </script>\n";
        htmlFile << "</body>\n";
        htmlFile << "</html>\n";

        htmlFile.close();

        cout << "\n[SUCCESS] Report generated successfully!" << endl;
        cout << "File: " << filename << endl;

        // Open the HTML file in default browser with absolute path
        #ifdef _WIN32
        char currentPath[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentPath);
        string fullPath = string(currentPath) + "\\reports\\" + poCode + "_" + filenameSafeDate + ".html";
        
        cout << "Full path: " << fullPath << endl;
        cout << "\nOpening report in browser..." << endl;
        
        // Use Windows 'start' command which is more reliable for opening files
        string command = "start \"\" \"" + fullPath + "\"";
        int result = system(command.c_str());
        
        if (result != 0) {
            cout << "[WARNING] Could not open browser automatically." << endl;
            cout << "You can manually open the file: " << fullPath << endl;
        }
        #else
        char currentPath[PATH_MAX];
        getcwd(currentPath, sizeof(currentPath));
        string fullPath = string(currentPath) + "/reports/" + poCode + "_" + filenameSafeDate + ".html";
        
        cout << "Full path: " << fullPath << endl;
        cout << "\nOpening report in browser..." << endl;
        
        string command = "xdg-open \"" + fullPath + "\" &";
        int ret = system(command.c_str());
        if (ret != 0) {
            cout << "[WARNING] Could not open browser automatically. File saved at: " << fullPath << endl;
        }

        #endif

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
        cout << "1. List All Purchase Orders" << endl;
        cout << "2. Generate Received Order Report" << endl;
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
            listPendingPurchaseOrders(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 2) {
            system("cls");
            generateReceivedOrderReport(db);
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
