#include <iostream>
#include <string>
#include <stdexcept>
#include <mysql.h>
#include "Database.h"
#include "Sales.h"

using namespace std;

void SalesManager::processSale(Database& db, int userID) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        int prodID, qty;
        cout << "Enter Product ID to sell (0 to go back): ";
        while (!(cin >> prodID) || prodID < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid Product ID (0 to go back): ";
        }
        if (prodID == 0) {
            cin.ignore(10000, '\n');
            return;
        }
        cout << "Enter Quantity: ";
        while (!(cin >> qty) || qty <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter positive quantity: ";
        }
        cin.ignore();

        // 1. Get Product Details
        string query = "SELECT sell_price, cost_price, stock_quantity FROM Product WHERE product_id = " + to_string(prodID);
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch product details: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);

        if (row) {
            double sellPrice = stod(row[0]);
            double costPrice = stod(row[1]);
            int currentStock = stoi(row[2]);

            if (currentStock >= qty) {
                // Calculate Totals 
                double totalAmount = sellPrice * qty;
                double profit = (sellPrice - costPrice) * qty;

                // 2. Record Sale [cite: 121]
                string saleQuery = "INSERT INTO Sales (user_id, total_amount, profit) VALUES (" 
                                   + to_string(userID) + ", " + to_string(totalAmount) + ", " + to_string(profit) + ")";
                db.executeQuery(saleQuery);

                // 3. Update Stock (Business Rule: Stock decreases after sale) 
                string updateStock = "UPDATE Product SET stock_quantity = stock_quantity - " + to_string(qty) 
                                     + " WHERE product_id = " + to_string(prodID);
                db.executeQuery(updateStock);

            cout << "\n[OK] Sale Recorded Successfully!" << endl;
            cout << "Total: RM" << totalAmount << " | Profit: RM" << profit << endl;
        } else {
            cout << "\n[ERROR] Insufficient Stock!" << endl;
        }
        
        mysql_free_result(res);
        
    } else {
        mysql_free_result(res);
        cout << "\n[ERROR] Product not found." << endl;
    }
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] Failed to process sale: " << e.what() << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Sales Page - Process sale with clean page layout
void SalesManager::salesPage(Database& db, int userID) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "       PROCESS SALES TRANSACTION        " << endl;
        cout << "========================================" << endl;
        cout << "\nOptions:" << endl;
        cout << "1. Record New Sale" << endl;
        cout << "2. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          RECORD NEW SALE               " << endl;
            cout << "========================================\n" << endl;
            processSale(db, userID);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            break;
        }
    }
}