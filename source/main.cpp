#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Sales.h"
#include "Supplier.h"
#include "Dashboard.h"
#include "PurchaseOrder.h"
#include "Receiving.h"
#ifdef _WIN32
#include <windows.h>
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// Clear screen function
void clearScreen() {
    system("cls");
}

#ifdef _WIN32
// Enable ANSI color sequences in Windows consoles
static void enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
}
#endif

// Display dashboard header
void displayDashboardHeader(string role) {
    clearScreen();
    cout << "\n";
    cout << "+" << string(62, '=') << "+" << endl;
    cout << "|" << string(62, ' ') << "|" << endl;
    cout << "|" << setw(41) << "GroceryFlow IMS v2.0" << setw(22) << "|" << endl;
    cout << "|" << setw(42) << "Professional Platform" << setw(21) << "|" << endl;
    cout << "|" << setw(48) << "Secure Inventory Management System" << setw(15) << "|" << endl;
    cout << "|" << string(62, ' ') << "|" << endl;
    cout << "+" << string(62, '=') << "+" << endl;
    cout << "\n  Role: [ " << role << " ]  |  Status: [ ACTIVE ] " << endl;
    cout << string(64, '-') << endl;
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
        cout << "\n+" << string(62, '-') << "+" << endl;
        cout << "|  ADMINISTRATOR MENU" << setw(42) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "|  1. [MONITOR]    Real-Time Monitoring Dashboard" << setw(14) << "|" << endl;
        cout << "|  2. [SUPPLIERS]  Supplier Management" << setw(27) << "|" << endl;
        cout << "|  3. [INVENTORY]  Inventory & Stock Management" << setw(17) << "|" << endl;
        cout << "|  4. [USERS]      User & Staff Management" << setw(23) << "|" << endl;
        cout << "|  5. [EXIT]       Logout from System" << setw(28) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "+" << string(62, '-') << "+" << endl;
        cout << "\nSelect option (1-5): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            dashMgr.showMonitoringDashboard(db);
        } 
        else if (choice == 2) {
            supplierMgr.supplierPage(db);
        } 
        else if (choice == 3) {
            invMgr.inventoryPage(db, &poMgr, &recvMgr, &salesMgr);
        } 
        else if (choice == 4) {
            userMgr.userManagementMenu(db);
        } 
        else if (choice == 5) {
            clearScreen();
            cout << "\n" << "+" << string(62, '=') << "+" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "|  " << setw(58) << left << "[SUCCESS] Session ended successfully" << "|" << endl;
            cout << "|  " << setw(58) << left << "Returning to Login Screen..." << "|" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "+" << string(62, '=') << "+" << endl;
            cout << "\n";
            break;
        }
    }
}

// Display staff dashboard
void showStaffDashboard(Database& db, UserManager& userMgr, 
                       InventoryManager& invMgr, SalesManager& salesMgr, 
                       SupplierManager& supplierMgr, PurchaseOrderManager& poMgr,
                       ReceivingManager& recvMgr) {
    DashboardManager dashMgr;
    int choice;
    while (true) {
        displayDashboardHeader("Staff");
        cout << "\n+" << string(62, '-') << "+" << endl;
        cout << "|  STAFF MENU" << setw(50) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "|  1. [MONITOR]    Real-Time Monitoring Dashboard" << setw(14) << "|" << endl;
        cout << "|  2. [SALES]      Sales & Transactions" << setw(27) << "|" << endl;
        cout << "|  3. [STAFF]      Staff Management" << setw(29) << "|" << endl;
        cout << "|  4. [EXIT]       Logout from System" << setw(28) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "+" << string(62, '-') << "+" << endl;
        cout << "\nSelect option (1-4): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-4: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            dashMgr.showMonitoringDashboard(db);
        } 
        else if (choice == 2) {
            salesMgr.salesPage(db, 0); // Cashier mode (userID 0 for now)
        } 
        else if (choice == 3) {
            userMgr.staffManagementMenu(db);
        } 
        else if (choice == 4) {
            clearScreen();
            cout << "\n" << "+" << string(62, '=') << "+" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "|  " << setw(58) << left << "[SUCCESS] Session ended successfully" << "|" << endl;
            cout << "|  " << setw(58) << left << "Returning to Login Screen..." << "|" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "+" << string(62, '=') << "+" << endl;
            cout << "\n";
            break;
        }
    }
}

// Inventory Admin Dashboard
void showInventoryAdminDashboard(Database& db, UserManager& userMgr, 
                                 InventoryManager& invMgr, SupplierManager& supplierMgr,
                                 PurchaseOrderManager& poMgr, ReceivingManager& recvMgr) {
    DashboardManager dashMgr;
    int choice;
    while (true) {
        displayDashboardHeader("Inventory Admin");
        cout << "\n+" << string(62, '-') << "+" << endl;
        cout << "|  INVENTORY ADMIN MENU" << setw(40) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "|  1. [MONITOR]    Real-Time Monitoring Dashboard" << setw(14) << "|" << endl;
        cout << "|  2. [SUPPLIERS]  Supplier Management" << setw(28) << "|" << endl;
        cout << "|  3. [INVENTORY]  Inventory & Stock Management" << setw(15) << "|" << endl;
        cout << "|  4. [USERS]      User Profile Management" << setw(22) << "|" << endl;
        cout << "|  5. [EXIT]       Logout from System" << setw(28) << "|" << endl;
        cout << "|" << string(62, ' ') << "|" << endl;
        cout << "+" << string(62, '-') << "+" << endl;
        cout << "\nSelect option (1-5): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            dashMgr.showMonitoringDashboard(db);
        }
        else if (choice == 2) {
            supplierMgr.supplierPage(db);
        }
        else if (choice == 3) {
            invMgr.inventoryAdminSubmenu(db, &poMgr, &recvMgr);
        }
        else if (choice == 4) {
            userMgr.inventoryAdminMenu(db);
        }
        else if (choice == 5) {
            clearScreen();
            cout << "\n" << "+" << string(62, '=') << "+" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "|  " << setw(58) << left << "[SUCCESS] Session ended successfully" << "|" << endl;
            cout << "|  " << setw(58) << left << "Returning to Login Screen..." << "|" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "+" << string(62, '=') << "+" << endl;
            cout << "\n";
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

        #ifdef _WIN32
        enableAnsiColors();
        #endif

        // Main application loop - allows return to login after logout
        while (true) {
            clearScreen();
            cout << "\n" << "+" << string(62, '=') << "+" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "|  " << setw(58) << left << "GroceryFlow IMS v2.0" << "|" << endl;
            cout << "|  " << setw(58) << left << "Professional Platform" << "|" << endl;
            cout << "|  " << setw(58) << left << "Secure Inventory Management System" << "|" << endl;
            cout << "|" << string(62, ' ') << "|" << endl;
            cout << "+" << string(62, '=') << "+" << endl;
            cout << "\n+ LOGIN" << string(55, ' ') << "+" << endl;
            
            // 1. Authenticate
            string role = userMgr.login(db);

            if (role == "") {
                // User chose to exit from login
                clearScreen();
                cout << "\n" << "+" << string(62, '=') << "+" << endl;
                cout << "|" << string(62, ' ') << "|" << endl;
                cout << "|  " << setw(58) << left << "Thank you for using GroceryFlow IMS!" << "|" << endl;
                cout << "|" << string(62, ' ') << "|" << endl;
                cout << "+" << string(62, '=') << "+" << endl;
                cout << "\nApplication closed. Have a great day!" << endl;
                cout << "Press Enter to exit...";
                cin.get();
                break;
            }

            // 2. Dashboard Loop based on role
            if (role == "ADMIN") {
                showAdminDashboard(db, userMgr, invMgr, salesMgr, supplierMgr, poMgr, recvMgr);
            } else if (role == "INVENTORY ADMIN") {
                showInventoryAdminDashboard(db, userMgr, invMgr, supplierMgr, poMgr, recvMgr);
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
