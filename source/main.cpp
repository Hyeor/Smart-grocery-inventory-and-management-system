#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Sales.h"
#include "Supplier.h"

// Clear screen function
void clearScreen() {
    system("cls");
}

// Display dashboard header
void displayDashboardHeader(string role) {
    clearScreen();
    cout << "\n";
    cout << "========================================" << endl;
    cout << "    SMART GROCERY MANAGEMENT SYSTEM     " << endl;
    cout << "========================================" << endl;
    cout << "       Role: " << role << endl;
    cout << "========================================" << endl;
}

// Display admin dashboard
void showAdminDashboard(Database& db, UserManager& userMgr, 
                       InventoryManager& invMgr, SalesManager& salesMgr, 
                       SupplierManager& supplierMgr) {
    int choice;
    while (true) {
        displayDashboardHeader("Admin");
        cout << "\nMain Menu:" << endl;
        cout << "1. Inventory Management" << endl;
        cout << "2. User Management" << endl;
        cout << "3. Supplier Management" << endl;
        cout << "4. Process Sale" << endl;
        cout << "5. Logout" << endl;
        cout << "\nSelect option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            invMgr.inventoryPage(db);
        } 
        else if (choice == 2) {
            userMgr.userManagementMenu(db);
        } 
        else if (choice == 3) {
            supplierMgr.supplierPage(db);
        } 
        else if (choice == 4) {
            salesMgr.salesPage(db, 1);
        } 
        else if (choice == 5) {
            clearScreen();
            cout << "\nLogging out... Thank you for using Smart Grocery Management System!" << endl;
            break;
        }
    }
}

// Display staff dashboard
void showStaffDashboard(Database& db, UserManager& userMgr, 
                       InventoryManager& invMgr, SalesManager& salesMgr, 
                       SupplierManager& supplierMgr) {
    int choice;
    while (true) {
        displayDashboardHeader("Staff");
        cout << "\nMain Menu:" << endl;
        cout << "1. Inventory Management" << endl;
        cout << "2. Supplier Management" << endl;
        cout << "3. Process Sale" << endl;
        cout << "4. Logout" << endl;
        cout << "\nSelect option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-4: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            invMgr.inventoryPage(db);
        } 
        else if (choice == 2) {
            supplierMgr.supplierPage(db);
        } 
        else if (choice == 3) {
            salesMgr.salesPage(db, 1);
        } 
        else if (choice == 4) {
            clearScreen();
            cout << "\nLogging out... Thank you for using Smart Grocery Management System!" << endl;
            break;
        }
    }
}

int main() {
    cout << "\n=== Smart Grocery Management System ===" << endl;
    
    Database db;

    UserManager userMgr;
    InventoryManager invMgr;
    SalesManager salesMgr;
    SupplierManager supplierMgr;

    // 1. Authenticate
    string role = userMgr.login(db);

    if (role == "") return 0; // Exit if failed

    // 2. Dashboard Loop based on role
    if (role == "Admin") {
        showAdminDashboard(db, userMgr, invMgr, salesMgr, supplierMgr);
    } else {
        showStaffDashboard(db, userMgr, invMgr, salesMgr, supplierMgr);
    }

    return 0;
}