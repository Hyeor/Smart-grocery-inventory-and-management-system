#ifndef SALES_H
#define SALES_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class SalesManager {
public:
    void processSale(Database& db, int userID) {
        int prodID, qty;
        cout << "Enter Product ID to sell: "; cin >> prodID;
        cout << "Enter Quantity: "; cin >> qty;

        // 1. Get Product Details
        string query = "SELECT sell_price, cost_price, stock_quantity FROM Product WHERE product_id = " + to_string(prodID);
        mysql_query(db.conn, query.c_str());
        MYSQL_RES* res = mysql_store_result(db.conn);
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

                cout << "Sale Recorded! Total: " << totalAmount << " Profit: " << profit << endl;
            } else {
                cout << "Error: Insufficient Stock!" << endl;
            }
        } else {
            cout << "Product not found." << endl;
        }
    }
};

#endif
