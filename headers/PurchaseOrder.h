#ifndef PURCHASEORDER_H
#define PURCHASEORDER_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include "Database.h"

using namespace std;

// Structure to hold purchase order item details
struct POItem {
    int itemId;
    string itemName;
    string unit;
    double quantity;
    double costPrice;
    double totalCost;
};

class PurchaseOrderManager {
private:
    // Helper functions
    string generatePOCode(Database& db);
    void displaySuppliers(Database& db);
    void displayItemsBySupplier(Database& db, int supplierID);
    bool getItemDetails(Database& db, int itemId, string& itemName, string& unit, double& costPrice);
    void displayPOItems(const vector<POItem>& items);
    double calculateSubTotal(const vector<POItem>& items);
    void displayPOSummary(const vector<POItem>& items, double discount, double tax);
    
public:
    // Main purchase order operations
    void createPurchaseOrder(Database& db);
    void viewAllPurchaseOrders(Database& db);
    void viewPurchaseOrderDetails(Database& db);
    void purchaseOrderMenu(Database& db);
    
    // Purchase order page with clean layout
    void purchaseOrderPage(Database& db);
};

#endif
