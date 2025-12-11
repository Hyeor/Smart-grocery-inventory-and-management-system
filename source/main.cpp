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
    try {
        cout << "\n=== Smart Grocery Management System ===" << endl;
        
        Database db;
        
        // Check if database connection is valid
        if (!db.conn) {
            cerr << "\n[CRITICAL ERROR] Cannot proceed without database connection." << endl;
            cerr << "Please ensure MySQL server is running and credentials are correct." << endl;
            cout << "\nPress Enter to exit...";
            cin.get();
            return 1;
        }

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

    } catch (const exception& e) {
        cerr << "\n[FATAL ERROR] " << e.what() << endl;
        cout << "\nThe application encountered a critical error and must close." << endl;
        cout << "Press Enter to exit...";
        cin.ignore();
        cin.get();
        return 1;
    } catch (...) {
        cerr << "\n[UNKNOWN ERROR] An unexpected error occurred." << endl;
        cout << "Press Enter to exit...";
        cin.ignore();
        cin.get();
        return 1;
    }

    return 0;
}