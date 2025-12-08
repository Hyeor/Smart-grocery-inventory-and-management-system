#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class ItemManager {
public:
    void addItem(Database& db);
    void viewItems(Database& db);
    void viewItem(Database& db, int itemID);
    void updateItem(Database& db);
    void deleteItem(Database& db);
    void itemMenu(Database& db);
};

#endif
