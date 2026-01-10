#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Sales.h"
#include "Supplier.h"
#include "Dashboard.h"
#include "PurchaseOrder.h"
#include "Receiving.h"
#include "UI_Helpers.h"
#ifdef _WIN32
#include <windows.h>
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// Use UI helpers for clear and ANSI enabling

// Display dashboard header
void displayDashboardHeader(string role) {
    UI::clear();
    UI::bannerGrocerySystem();
    int w = std::min(UI::consoleWidth() - 4, 100);
    std::string statusLine = "Role: [ " + role + " ]  |  Status: [ ACTIVE ]";
    std::cout << UI::WHITE << UI::centered(statusLine, w) << UI::RESET << std::endl;
    UI::divider(w, '-');
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
        UI::drawMenu("MENU", {
            "1. [MONITOR]    Real-Time Monitoring Dashboard",
            "2. [SUPPLIERS]  Supplier Management",
            "3. [INVENTORY]  Inventory & Stock Management",
            "4. [SALES]      Sales & Reports",
            "5. [USERS]      User & Staff Management",
            "6. [EXIT]       Logout from System"
        });
        cout << "\nSelect option (1-6): "; 
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
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
            salesMgr.salesPage(db, 0, "ADMIN");
        }
        else if (choice == 5) {
            userMgr.userManagementMenu(db);
        } 
        else if (choice == 6) {
            UI::clear();
            UI::printSuccess("Session ended successfully");
            std::cout << UI::GREY << "Returning to Login Screen..." << UI::RESET << std::endl;
            UI::pause();
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
        UI::drawMenu("MENU", {
            "1. [MONITOR]    Real-Time Monitoring Dashboard",
            "2. [SALES]      Sales & Transactions",
            "3. [STAFF]      Staff Management",
            "4. [EXIT]       Logout from System"
        });
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
            salesMgr.salesPage(db, 0, "STAFF"); // Cashier mode, STAFF role
        } 
        else if (choice == 3) {
            userMgr.staffManagementMenu(db);
        } 
        else if (choice == 4) {
            UI::clear();
            UI::printSuccess("Session ended successfully");
            std::cout << UI::GREY << "Returning to Login Screen..." << UI::RESET << std::endl;
            UI::pause();
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
        UI::drawMenu("MENU", {
            "1. [MONITOR]    Real-Time Monitoring Dashboard",
            "2. [SUPPLIERS]  Supplier Management",
            "3. [INVENTORY]  Inventory & Stock Management",
            "4. [USERS]      User Profile Management",
            "5. [EXIT]       Logout from System"
        });
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
            UI::clear();
            UI::printSuccess("Session ended successfully");
            std::cout << UI::GREY << "Returning to Login Screen..." << UI::RESET << std::endl;
            UI::pause();
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

        UI::enableAnsi();
        UI::loadingStartup();

        // Main application loop - allows return to login after logout
        while (true) {
            UI::clear();
            UI::bannerGrocerySystem();
            UI::drawBox("LOGIN", {"Please authenticate to continue."});
            
            // 1. Authenticate
            string role = userMgr.login(db);

            if (role == "") {
                // User chose to exit from login
                UI::clear();
                UI::drawBox("GOODBYE", {"Thank you for using GroceryFlow IMS!", "Application closed. Have a great day!"});
                UI::pause();
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
