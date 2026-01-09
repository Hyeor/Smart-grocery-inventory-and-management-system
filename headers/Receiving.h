#ifndef RECEIVING_H
#define RECEIVING_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include "Database.h"

using namespace std;

struct ReceivingItemInput {
    int productId;
    string name;
    string unit;
    double poQuantity;
    double unitPrice;
    double receivedQuantity;
};

class ReceivingManager {
public:
    void receivingPage(Database& db);
    void receivingMenu(Database& db);

private:
    void listPendingPurchaseOrders(Database& db);
    void generateReceivedOrderReport(Database& db);
};

#endif
