#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class DashboardManager {
public:
    // Main monitoring dashboard
    void showMonitoringDashboard(Database& db);
    
    // Show low stock items
    void showLowStockItems(Database& db, int threshold = 5);
    
    // Show supplier status overview
    void showSupplierStatus(Database& db);
    
    // Show sales statistics
    void showSalesStatistics(Database& db);
    
    // Show inventory summary
    void showInventorySummary(Database& db);
};

#endif
