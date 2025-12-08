#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Sales.h"
#include "Supplier.h"

int main() {
    cout << "\n=== Smart Grocery Management System ===" << endl;
    cout << "Connecting to database..." << endl;
    
    Database db;
    
    // Check if database connection was successful
    if (!db.isValid()) {
        cout << "\n✗ Cannot proceed without database connection." << endl;
        cout << "Please ensure:" << endl;
        cout << "  1. MySQL Server is running" << endl;
        cout << "  2. Database 'smart_grocery' exists" << endl;
        cout << "  3. User 'root' has no password" << endl;
        cout << "\nPress Enter to exit..." << endl;
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

    // 2. Dashboard Loop
    int choice;
    while (true) {
        cout << "\n=== DASHBOARD (" << role << ") ===" << endl;
        cout << "1. Inventory Management" << endl;
        if (role == "Admin") {
            cout << "2. User Management" << endl;
            cout << "3. Supplier Management" << endl;
            cout << "4. Process Sale" << endl;
            cout << "5. Exit" << endl;
        } else {
            cout << "2. Supplier Management" << endl;
            cout << "3. Process Sale" << endl;
            cout << "4. Exit" << endl;
        }
        cout << "Select: "; cin >> choice;

        if (choice == 1) {
            // Only Admin or Staff can access inventory
            if (role == "Admin" || role == "Staff") {
                int invChoice;
                while (true) {
                    cout << "\n=== INVENTORY MANAGEMENT ===" << endl;
                    cout << "1. Add Product (CREATE)" << endl;
                    cout << "2. View All Products (READ)" << endl;
                    cout << "3. View Specific Product (READ)" << endl;
                    cout << "4. Update Product (UPDATE)" << endl;
                    cout << "5. Delete Product (DELETE)" << endl;
                    cout << "6. Back to Dashboard" << endl;
                    cout << "Select: "; 
                    while (!(cin >> invChoice) || invChoice < 1 || invChoice > 6) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid choice! Select 1-6: ";
                    }
                    
                    if (invChoice == 1) {
                        invMgr.addProduct(db);
                    }
                    else if (invChoice == 2) {
                        invMgr.viewInventory(db);
                    }
                    else if (invChoice == 3) {
                        int productID;
                        cout << "Enter Product ID: ";
                        while (!(cin >> productID) || productID <= 0) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            cout << "Invalid input! Enter valid Product ID: ";
                        }
                        invMgr.viewProduct(db, productID);
                    }
                    else if (invChoice == 4) {
                        invMgr.updateProduct(db);
                    }
                    else if (invChoice == 5) {
                        invMgr.deleteProduct(db);
                    }
                    else if (invChoice == 6) {
                        break;
                    }
                }
        } else {
            cout << "Access Denied: You do not have Inventory permissions." << endl;
        }
        else if (role == "Admin" && choice == 2) {
            // Only Admin can access user management
            userMgr.userManagementMenu(db);
        }
        else if ((role == "Admin" && choice == 3) || (role != "Admin" && choice == 2)) {
            // Admin accesses via option 3, non-Admin accesses via option 2
            // Only Admin can access supplier management
            if (role == "Admin") {
                supplierMgr.supplierMenu(db);
            } else {
                cout << "Access Denied: Only Admins can manage suppliers." << endl;
            }
        }
        else if ((role == "Admin" && choice == 4) || (role != "Admin" && choice == 3)) {
            // Admin accesses via option 4, non-Admin accesses via option 3
            // Admin or Sales Staff
            // Note: In a real app, you'd fetch the actual logged-in user's ID
            salesMgr.processSale(db, 1); 
        }
        else if ((role == "Admin" && choice == 5) || (role != "Admin" && choice == 4)) {
            // Admin accesses via option 5, non-Admin accesses via option 4
            break;
        }
    }
    return 0;
}