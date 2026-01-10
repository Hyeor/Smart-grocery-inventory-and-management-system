#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <mysql.h>
#include "Database.h"
#include "Supplier.h"

using namespace std;

// CREATE - Add new supplier
void SupplierManager::addSupplier(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string supplierName, address, contactPerson, phone;
        
        cout << "\n=== ADD NEW SUPPLIER ===" << endl;
        cout << "Enter Supplier Name (0 to cancel): ";
        getline(cin >> ws, supplierName);
        
        while (supplierName.empty()) {
            cout << "Error: Supplier name cannot be empty!" << endl;
            cout << "Enter Supplier Name (0 to cancel): ";
            getline(cin >> ws, supplierName);
        }
        if (supplierName == "0") {
            return; // cancel adding
        }
        
        cout << "Enter Address (0 to cancel): ";
        getline(cin >> ws, address);
        
        while (address.empty()) {
            cout << "Error: Address cannot be empty!" << endl;
            cout << "Enter Address (0 to cancel): ";
            getline(cin >> ws, address);
        }
        if (address == "0") {
            return;
        }
        
        cout << "Enter Contact Person (0 to cancel): ";
        getline(cin >> ws, contactPerson);
        
        while (contactPerson.empty()) {
            cout << "Error: Contact person cannot be empty!" << endl;
            cout << "Enter Contact Person (0 to cancel): ";
            getline(cin >> ws, contactPerson);
        }
        if (contactPerson == "0") {
            return;
        }
        
        cout << "Enter Phone Number (0 to cancel): ";
        getline(cin >> ws, phone);
        if (phone == "0") {
            return;
        }
        
        string query = "INSERT INTO Supplier (supplier_name, address, contact_person, phone, status) VALUES ('" 
                       + supplierName + "', '" + address + "', '" + contactPerson + "', '" + phone + "', 'Active')";
        db.executeQuery(query);
        cout << "\n[OK] Supplier Added Successfully." << endl;
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] Failed to add supplier: " << e.what() << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// READ - View all suppliers
void SupplierManager::viewSuppliers(Database& db) {
    try {
        string query = "SELECT * FROM Supplier";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch suppliers: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n=== LIST OF SUPPLIERS ===" << endl;
        cout << "===============================================================================" << endl;
        cout << "Supplier                | Contact Person      | Status   | Date Created     " << endl;
        cout << "===============================================================================" << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
             // Column order: 0:id, 1:name, 2:address, 3:contact, 4:phone, 5:status, 6:date_created
             printf("%-23s | %-19s | %-8s | %s\n",
                 row[1], row[3], row[5], row[6]);
        }
        cout << "===============================================================================" << endl;
        printf("Showing 1 to %d of %d entries\n", count, count);
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View single supplier by ID
void SupplierManager::viewSupplier(Database& db, int supplierID) {
    try {
        string query = "SELECT * FROM Supplier WHERE supplier_id = " + to_string(supplierID);
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch supplier: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row) {
            cout << "\n--- Supplier Details ---" << endl;
            cout << "Supplier Name: " << row[1] << endl;
            cout << "Address: " << row[2] << endl;
            cout << "Contact Person: " << row[3] << endl;
            cout << "Phone: " << row[4] << endl;
            cout << "Status: " << row[5] << endl;
            cout << "Date Created: " << row[6] << endl;
            cout << "------------------------" << endl;
        } else {
            cout << "[ERROR] Supplier not found." << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View supplier by name
void SupplierManager::viewSupplierByName(Database& db, const string& supplierName) {
    try {
        string query = "SELECT * FROM Supplier WHERE supplier_name LIKE '%" + supplierName + "%'";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch supplier: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;
        int count = 0;
        
        while ((row = mysql_fetch_row(res))) {
            count++;
            cout << "\n--- Supplier Details ---" << endl;
            cout << "Supplier Name: " << row[1] << endl;
            cout << "Address: " << row[2] << endl;
            cout << "Contact Person: " << row[3] << endl;
            cout << "Phone: " << row[4] << endl;
            cout << "Status: " << row[5] << endl;
            cout << "Date Created: " << row[6] << endl;
            cout << "------------------------" << endl;
        }
        
        if (count == 0) {
            cout << "[ERROR] No supplier found with name matching '" << supplierName << "'." << endl;
        } else {
            cout << "\nTotal suppliers found: " << count << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// UPDATE - Update supplier details
void SupplierManager::updateSupplier(Database& db) {
    string supplierName;
    cout << "Enter Supplier Name to update (0 to go back): ";
    getline(cin, supplierName);
    
    if (supplierName == "0") {
        return;
    }
    
    if (supplierName.empty()) {
        cout << "[ERROR] Supplier name cannot be empty!" << endl;
        return;
    }
    
    // Search for supplier by name
    try {
        string searchQuery = "SELECT * FROM Supplier WHERE supplier_name LIKE '%" + supplierName + "%'";
        
        if (mysql_query(db.conn, searchQuery.c_str())) {
            throw runtime_error("Failed to fetch supplier: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        // Store results in a vector to allow selection if multiple found
        vector<vector<string>> suppliers;
        MYSQL_ROW row;
        
        while ((row = mysql_fetch_row(res))) {
            vector<string> supplier;
            for (int i = 0; i < 7; i++) {  // 7 columns in Supplier table
                supplier.push_back(row[i] ? row[i] : "");
            }
            suppliers.push_back(supplier);
        }
        mysql_free_result(res);
        
        if (suppliers.empty()) {
            cout << "[ERROR] No supplier found with name matching '" << supplierName << "'." << endl;
            return;
        }
        
        int selectedIndex = 0;
        
        // If multiple suppliers found, let user select
        if (suppliers.size() > 1) {
            cout << "\nMultiple suppliers found:" << endl;
            cout << "----------------------------" << endl;
            for (size_t i = 0; i < suppliers.size(); i++) {
                cout << (i + 1) << ". " << suppliers[i][1] << " - " << suppliers[i][3] << endl;
            }
            cout << "----------------------------" << endl;
            cout << "Select supplier (1-" << suppliers.size() << "): ";
            
            int selection;
            while (!(cin >> selection) || selection < 1 || selection > (int)suppliers.size()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid selection! Select 1-" << suppliers.size() << ": ";
            }
            cin.ignore(10000, '\n');
            selectedIndex = selection - 1;
        }
        
        // Display selected supplier details
        cout << "\n--- Supplier Details ---" << endl;
        cout << "Supplier Name: " << suppliers[selectedIndex][1] << endl;
        cout << "Address: " << suppliers[selectedIndex][2] << endl;
        cout << "Contact Person: " << suppliers[selectedIndex][3] << endl;
        cout << "Phone: " << suppliers[selectedIndex][4] << endl;
        cout << "Status: " << suppliers[selectedIndex][5] << endl;
        cout << "Date Created: " << suppliers[selectedIndex][6] << endl;
        cout << "------------------------" << endl;
        
        int supplierID = stoi(suppliers[selectedIndex][0]);
        
        cout << "\nWhat do you want to update?" << endl;
        cout << "1. Supplier Name" << endl;
        cout << "2. Address" << endl;
        cout << "3. Contact Person" << endl;
        cout << "4. Phone Number" << endl;
        cout << "5. Status" << endl;
        cout << "Select: ";
        
        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore(10000, '\n');
        
        string query;
        if (choice == 1) {
            string newName;
            cout << "Enter new Supplier Name (0 to cancel): ";
            getline(cin, newName);
            if (newName == "0") return;
            query = "UPDATE Supplier SET supplier_name = '" + newName + "' WHERE supplier_id = " + to_string(supplierID);
        }
        else if (choice == 2) {
            string newAddress;
            cout << "Enter new Address (0 to cancel): ";
            getline(cin, newAddress);
            if (newAddress == "0") return;
            query = "UPDATE Supplier SET address = '" + newAddress + "' WHERE supplier_id = " + to_string(supplierID);
        }
        else if (choice == 3) {
            string newContact;
            cout << "Enter new Contact Person (0 to cancel): ";
            getline(cin, newContact);
            if (newContact == "0") return;
            query = "UPDATE Supplier SET contact_person = '" + newContact + "' WHERE supplier_id = " + to_string(supplierID);
        }
        else if (choice == 4) {
            string newPhone;
            cout << "Enter new Phone Number (0 to cancel): ";
            getline(cin, newPhone);
            if (newPhone == "0") return;
            query = "UPDATE Supplier SET phone = '" + newPhone + "' WHERE supplier_id = " + to_string(supplierID);
        }
        else if (choice == 5) {
            string newStatus;
            cout << "Enter Status (Active/Inactive, 0 to cancel): ";
            getline(cin, newStatus);
            if (newStatus == "0") return;
            query = "UPDATE Supplier SET status = '" + newStatus + "' WHERE supplier_id = " + to_string(supplierID);
        }
        
        db.executeQuery(query);
        cout << "[OK] Supplier Updated Successfully." << endl;
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// DELETE - Delete supplier
void SupplierManager::deleteSupplier(Database& db) {
    string supplierName;
    cout << "Enter Supplier Name to delete (0 to go back): ";
    getline(cin, supplierName);
    
    if (supplierName == "0") {
        return;
    }
    
    if (supplierName.empty()) {
        cout << "[ERROR] Supplier name cannot be empty!" << endl;
        return;
    }
    
    // Search for supplier by name
    try {
        string searchQuery = "SELECT * FROM Supplier WHERE supplier_name LIKE '%" + supplierName + "%'";
        
        if (mysql_query(db.conn, searchQuery.c_str())) {
            throw runtime_error("Failed to fetch supplier: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        // Store results in a vector to allow selection if multiple found
        vector<vector<string>> suppliers;
        MYSQL_ROW row;
        
        while ((row = mysql_fetch_row(res))) {
            vector<string> supplier;
            for (int i = 0; i < 7; i++) {  // 7 columns in Supplier table
                supplier.push_back(row[i] ? row[i] : "");
            }
            suppliers.push_back(supplier);
        }
        mysql_free_result(res);
        
        if (suppliers.empty()) {
            cout << "[ERROR] No supplier found with name matching '" << supplierName << "'." << endl;
            return;
        }
        
        int selectedIndex = 0;
        
        // If multiple suppliers found, let user select
        if (suppliers.size() > 1) {
            cout << "\nMultiple suppliers found:" << endl;
            cout << "----------------------------" << endl;
            for (size_t i = 0; i < suppliers.size(); i++) {
                cout << (i + 1) << ". " << suppliers[i][1] << " - " << suppliers[i][3] << endl;
            }
            cout << "----------------------------" << endl;
            cout << "Select supplier (1-" << suppliers.size() << "): ";
            
            int selection;
            while (!(cin >> selection) || selection < 1 || selection > (int)suppliers.size()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid selection! Select 1-" << suppliers.size() << ": ";
            }
            cin.ignore(10000, '\n');
            selectedIndex = selection - 1;
        }
        
        // Display selected supplier details
        cout << "\n--- Supplier Details ---" << endl;
        cout << "Supplier Name: " << suppliers[selectedIndex][1] << endl;
        cout << "Address: " << suppliers[selectedIndex][2] << endl;
        cout << "Contact Person: " << suppliers[selectedIndex][3] << endl;
        cout << "Phone: " << suppliers[selectedIndex][4] << endl;
        cout << "Status: " << suppliers[selectedIndex][5] << endl;
        cout << "Date Created: " << suppliers[selectedIndex][6] << endl;
        cout << "------------------------" << endl;
        
        int supplierID = stoi(suppliers[selectedIndex][0]);
        
        cout << "\nAre you sure you want to delete this supplier? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            string query = "DELETE FROM Supplier WHERE supplier_id = " + to_string(supplierID);
            db.executeQuery(query);
            cout << "[OK] Supplier Deleted Successfully." << endl;
        } else {
            cout << "Deletion cancelled." << endl;
        }
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// Supplier Management Menu
void SupplierManager::supplierMenu(Database& db) {
    int choice;
    while (true) {
        cout << "\n=== SUPPLIER MANAGEMENT ===" << endl;
        cout << "1. Create New Supplier" << endl;
        cout << "2. View All Suppliers" << endl;
        cout << "3. View Specific Supplier" << endl;
        cout << "4. Update Supplier" << endl;
        cout << "5. Delete Supplier" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Select: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
        }
        // Drop the trailing newline from numeric input before any getline in called flows
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            addSupplier(db);
        }
        else if (choice == 2) {
            viewSuppliers(db);
        }
        else if (choice == 3) {
            int supplierID;
            cout << "Enter Supplier ID: ";
            while (!(cin >> supplierID) || supplierID <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid Supplier ID: ";
            }
            viewSupplier(db, supplierID);
        }
        else if (choice == 4) {
            updateSupplier(db);
        }
        else if (choice == 5) {
            deleteSupplier(db);
        }
        else if (choice == 6) {
            break;
        }
    }
}

// Supplier Page - Full CRUD with clean page layout
void SupplierManager::supplierPage(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "    SUPPLIER MANAGEMENT SYSTEM          " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Add New Supplier" << endl;
        cout << "2. View All Suppliers" << endl;
        cout << "3. View Supplier Details" << endl;
        cout << "4. Update Supplier" << endl;
        cout << "5. Delete Supplier" << endl;
        cout << "6. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
        }
        cin.ignore(10000, '\n'); // Clear the newline from input buffer

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          ADD NEW SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            addSupplier(db);
            cout << "\n";
            viewSuppliers(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          VIEW ALL SUPPLIERS            " << endl;
            cout << "========================================\n" << endl;
            viewSuppliers(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        VIEW SUPPLIER DETAILS           " << endl;
            cout << "========================================\n" << endl;
            viewSuppliers(db);
            cout << "\n";
            string supplierName;
            cout << "Enter Supplier Name (0 to go back): ";
            getline(cin, supplierName);
            if (supplierName == "0") continue;
            viewSupplierByName(db, supplierName);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 4) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          UPDATE SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            viewSuppliers(db);
            cout << "\n";
            updateSupplier(db);
            cout << "\n";
            viewSuppliers(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 5) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          DELETE SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            // Show current suppliers first for easier selection
            viewSuppliers(db);
            cout << "\n";
            deleteSupplier(db);
            cout << "\n";
            viewSuppliers(db);
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == 6) {
            break;
        }
    }
}
