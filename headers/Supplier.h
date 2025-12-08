#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class SupplierManager {
public:
    void addSupplier(Database& db);
    void viewSuppliers(Database& db);
    void viewSupplier(Database& db, int supplierID);
    void updateSupplier(Database& db);
    void deleteSupplier(Database& db);
    void supplierMenu(Database& db);
};

#endif
