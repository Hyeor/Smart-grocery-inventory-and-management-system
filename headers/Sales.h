#ifndef SALES_H
#define SALES_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include "Database.h"

using namespace std;

struct CartItem {
    int productId;
    string productName;
    string barcode;
    double unitPrice;
    int quantity;
    double totalPrice;
};

class SalesManager {
private:
    vector<CartItem> cart;
    
public:
    void processSale(Database& db, int userID);
    void salesPage(Database& db, int userID);
    void cashierMode(Database& db, int userID);
    void scanBarcode(Database& db, const string& barcode, int& quantity);
    void displayCart();
    void removeFromCart(int index);
    void clearCart();
    double getCartTotal();
    void processPayment(Database& db, int userID);
    void generateReceipt(int userID, double totalAmount);
};

#endif
