#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Sales.h"
#include "Supplier.h"
#include "Dashboard.h"
#include "PurchaseOrder.h"
#include "Receiving.h"

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
                       SupplierManager& supplierMgr, PurchaseOrderManager& poMgr,
                       ReceivingManager& recvMgr) {
    DashboardManager dashMgr;
    int choice;
    while (true) {
        displayDashboardHeader("Admin");
        cout << "\nMain Menu:" << endl;
        cout << "1. Monitoring Dashboard" << endl;
        cout << "2. Inventory Management" << endl;
        cout << "3. User Management" << endl;
        cout << "4. Supplier Management" << endl;
        cout << "5. Purchase Order" << endl;
        cout << "6. Receiving" << endl;
        cout << "7. Process Sale" << endl;
        cout << "8. Logout" << endl;
        cout << "\nSelect option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 8) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-8: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            dashMgr.showMonitoringDashboard(db);
        } 
        else if (choice == 2) {
            invMgr.inventoryPage(db);
        } 
        else if (choice == 3) {
            userMgr.userManagementMenu(db);
        } 
        else if (choice == 4) {
            supplierMgr.supplierPage(db);
        } 
        else if (choice == 5) {
            poMgr.purchaseOrderPage(db);
        } 
        else if (choice == 6) {
            recvMgr.receivingPage(db);
        } 
        else if (choice == 7) {
            salesMgr.salesPage(db, 1);
        } 
        else if (choice == 8) {
            clearScreen();
            cout << "\nLogging out... Thank you for using Smart Grocery Management System!" << endl;
            break;
        }
    }
}

// Display staff dashboard
void showStaffDashboard(Database& db, UserManager& userMgr, 
                       InventoryManager& invMgr, SalesManager& salesMgr, 
                       SupplierManager& supplierMgr, PurchaseOrderManager& poMgr,
                       ReceivingManager& recvMgr) {
    (void)userMgr; // Staff doesn't have user management access
    DashboardManager dashMgr;
    int choice;
    while (true) {
        displayDashboardHeader("Staff");
        cout << "\nMain Menu:" << endl;
        cout << "1. Monitoring Dashboard" << endl;
        cout << "2. Inventory Management" << endl;
        cout << "3. Supplier Management" << endl;
        cout << "4. Purchase Order" << endl;
        cout << "5. Receiving" << endl;
        cout << "6. Process Sale" << endl;
        cout << "7. Logout" << endl;
        cout << "\nSelect option: "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 7) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-7: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            dashMgr.showMonitoringDashboard(db);
        } 
        else if (choice == 2) {
            invMgr.inventoryPage(db);
        } 
        else if (choice == 3) {
            supplierMgr.supplierPage(db);
        } 
        else if (choice == 4) {
            poMgr.purchaseOrderPage(db);
        } 
        else if (choice == 5) {
            recvMgr.receivingPage(db);
        } 
        else if (choice == 6) {
            salesMgr.salesPage(db, 1);
        } 
        else if (choice == 7) {
            clearScreen();
            cout << "\nLogging out... Thank you for using Smart Grocery Management System!" << endl;
            break;
        }
    }
}

int main() {
    try {
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
        PurchaseOrderManager poMgr;
        ReceivingManager recvMgr;

        // Main application loop - allows return to login after logout
        while (true) {
            clearScreen();
            cout << "\n=== Smart Grocery Management System ===" << endl;
            
            // 1. Authenticate
            string role = userMgr.login(db);

            if (role == "") {
                // User chose to exit from login
                clearScreen();
                cout << "\nThank you for using Smart Grocery Management System!" << endl;
                cout << "Press Enter to exit...";
                cin.get();
                break;
            }

            // 2. Dashboard Loop based on role
            if (role == "ADMIN") {
                showAdminDashboard(db, userMgr, invMgr, salesMgr, supplierMgr, poMgr, recvMgr);
            } else {
                showStaffDashboard(db, userMgr, invMgr, salesMgr, supplierMgr, poMgr, recvMgr);
            }
            
            // After logout, loop continues and returns to login
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