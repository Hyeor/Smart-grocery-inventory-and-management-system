#include <iostream>
#include <iomanip>
#include <cstdio>
#include <stdexcept>
#include <mysql.h>
#include "Database.h"
#include "Dashboard.h"

using namespace std;

// Show low stock items
void DashboardManager::showLowStockItems(Database& db, int threshold) {
    try {
        string query = "SELECT p.name, p.stock_quantity, p.unit, p.category "
                       "FROM Product p WHERE p.stock_quantity < " + to_string(threshold) + 
                       " ORDER BY p.stock_quantity ASC";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch low stock items: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        int count = 0;
        
        cout << "\n[!] LOW STOCK ITEMS (Below " << threshold << " units)" << endl;
        cout << "======================================================================" << endl;
        cout << "Product Name             | Stock | Unit | Category" << endl;
        cout << "======================================================================" << endl;
        
        while ((row = mysql_fetch_row(res))) {
            count++;
            string name = row[0] ? string(row[0]) : "N/A";
            string stock = row[1] ? string(row[1]) : "0";
            string unit = row[2] ? string(row[2]) : "pcs";
            string category = row[3] ? string(row[3]) : "Others";
            
            cout << left << setw(24) << name << " | "
                 << setw(5) << stock << " | "
                 << setw(4) << unit << " | "
                 << setw(20) << category << endl;
        }
        cout << "======================================================================" << endl;
        
        if (count == 0) {
            cout << "[OK] All products have adequate stock!" << endl;
        } else {
            cout << "[!] Found " << count << " item(s) with low stock" << endl;
        }
        cout << "======================================================================" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Show supplier status overview
void DashboardManager::showSupplierStatus(Database& db) {
    try {
        string query = "SELECT s.supplier_id, s.supplier_name, s.status, COUNT(p.product_id) as product_count "
                       "FROM Supplier s LEFT JOIN Product p ON s.supplier_id = p.supplier_id "
                       "GROUP BY s.supplier_id, s.supplier_name, s.status "
                       "ORDER BY s.status DESC, s.supplier_name";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch supplier status: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        int activeCount = 0, inactiveCount = 0;
        
        cout << "\n[SUPPLIERS] SUPPLIER STATUS OVERVIEW" << endl;
        cout << "======================================================================" << endl;
        cout << "Supplier ID | Supplier Name       | Status    | Products" << endl;
        cout << "======================================================================" << endl;
        
        while ((row = mysql_fetch_row(res))) {
            string id = row[0] ? string(row[0]) : "N/A";
            string name = row[1] ? string(row[1]) : "N/A";
            string status = row[2] ? string(row[2]) : "Unknown";
            string prodCount = row[3] ? string(row[3]) : "0";
            
            if (status == "Active") activeCount++;
            else inactiveCount++;
            
            string statusSymbol = (status == "Active") ? "[+] Active" : "[-] Inactive";
            
            cout << setw(11) << id << " | "
                 << left << setw(19) << name << " | "
                 << setw(9) << statusSymbol << " | "
                 << setw(8) << prodCount << endl;
        }
        cout << "======================================================================" << endl;
        cout << "Active Suppliers: " << activeCount << " | Inactive Suppliers: " << inactiveCount << endl;
        cout << "======================================================================" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Show sales statistics
void DashboardManager::showSalesStatistics(Database& db) {
    try {
        // Total sales
        string totalQuery = "SELECT COUNT(*) as transaction_count, SUM(total_amount) as total_revenue, "
                           "SUM(profit) as total_profit FROM Sales";
        
        if (mysql_query(db.conn, totalQuery.c_str())) {
            throw runtime_error("Failed to fetch sales data: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        cout << "\n[SALES] SALES STATISTICS" << endl;
        cout << "======================================================================" << endl;
        
        if (row) {
            string transCount = row[0] ? string(row[0]) : "0";
            string revenue = row[1] ? string(row[1]) : "0.00";
            string profit = row[2] ? string(row[2]) : "0.00";
            
            cout << "Total Transactions: " << transCount << endl;
            cout << "Total Revenue:      RM " << revenue << endl;
            cout << "Total Profit:       RM " << profit << endl;
        } else {
            cout << "No sales data available" << endl;
        }
        
        cout << "======================================================================" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Show inventory summary
void DashboardManager::showInventorySummary(Database& db) {
    try {
        string query = "SELECT COUNT(*) as total_products, "
                       "SUM(stock_quantity) as total_stock, "
                       "AVG(stock_quantity) as avg_stock "
                       "FROM Product";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch inventory data: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        cout << "\n[INVENTORY] INVENTORY SUMMARY" << endl;
        cout << "======================================================================" << endl;
        
        if (row) {
            string totalProds = row[0] ? string(row[0]) : "0";
            string totalStock = row[1] ? string(row[1]) : "0";
            string avgStock = row[2] ? string(row[2]) : "0";
            
            cout << "Total Products:     " << totalProds << endl;
            cout << "Total Stock Units:  " << totalStock << endl;
            cout << "Average Stock/Item: " << avgStock << " units" << endl;
        } else {
            cout << "No inventory data available" << endl;
        }
        
        cout << "======================================================================" << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Main monitoring dashboard
void DashboardManager::showMonitoringDashboard(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================================" << endl;
        cout << "     REAL-TIME MONITORING DASHBOARD" << endl;
        cout << "========================================================" << endl;
        
        // Show quick summaries
        showInventorySummary(db);
        showLowStockItems(db, 5);
        showSupplierStatus(db);
        showSalesStatistics(db);
        
        cout << "\n";
        cout << "========================================" << endl;
        cout << "           MONITORING OPTIONS           " << endl;
        cout << "========================================" << endl;
        cout << "1. Refresh Dashboard" << endl;
        cout << "2. Set Low Stock Threshold" << endl;
        cout << "3. Back to Main Menu" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-3: ";
        }
        cin.ignore(10000, '\n');
        
        if (choice == 1) {
            // Refresh - loop continues
            continue;
        }
        else if (choice == 2) {
            int threshold;
            cout << "Enter low stock threshold (current: 5 units): ";
            while (!(cin >> threshold) || threshold < 1) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter a valid threshold: ";
            }
            cin.ignore(10000, '\n');
            
            system("cls");
            showInventorySummary(db);
            showLowStockItems(db, threshold);
            
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            break;
        }
    }
}
