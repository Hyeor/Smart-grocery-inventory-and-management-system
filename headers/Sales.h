#ifndef SALES_H
#define SALES_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class SalesManager {
public:
    void processSale(Database& db, int userID);
    void salesPage(Database& db, int userID);
};

#endif
