#include <iostream>
#include <string>
#include <cstdio>
#include <mysql.h>
#include "Database.h"
#include "Supplier.h"

using namespace std;

// CREATE - Add new supplier
void SupplierManager::addSupplier(Database& db) {
    string supplierName, contactPerson, phone;
    
    cout << "\n=== ADD NEW SUPPLIER ===" << endl;
    cout << "Enter Supplier Name: ";
    cin.ignore();
    getline(cin, supplierName);
    
    while (supplierName.empty()) {
        cout << "Error: Supplier name cannot be empty!" << endl;
        cout << "Enter Supplier Name: ";
        getline(cin, supplierName);
    }
    
    cout << "Enter Contact Person: ";
    getline(cin, contactPerson);
    
    while (contactPerson.empty()) {
        cout << "Error: Contact person cannot be empty!" << endl;
        cout << "Enter Contact Person: ";
        getline(cin, contactPerson);
    }
    
    cout << "Enter Phone Number: ";
    getline(cin, phone);
    
    string query = "INSERT INTO Supplier (supplier_name, contact_person, phone, status) VALUES ('" 
                   + supplierName + "', '" + contactPerson + "', '" + phone + "', 'Active')";
    db.executeQuery(query);
    cout << "\n[OK] Supplier Added Successfully." << endl;
}

// READ - View all suppliers
void SupplierManager::viewSuppliers(Database& db) {
    string query = "SELECT * FROM Supplier";
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row;

    cout << "\n=== LIST OF SUPPLIERS ===" << endl;
    cout << "========================================================================" << endl;
    cout << "#  | Date Created       | Supplier         | Contact Person   | Status  " << endl;
    cout << "========================================================================" << endl;
    
    int count = 0;
    while ((row = mysql_fetch_row(res))) {
        count++;
        printf("%-2d | %-18s | %-16s | %-16s | %s\n",
               count, row[4], row[1], row[2], row[5]);
    }
    cout << "========================================================================" << endl;
    printf("Showing 1 to %d of %d entries\n", count, count);
}

// READ - View single supplier by ID
void SupplierManager::viewSupplier(Database& db, int supplierID) {
    string query = "SELECT * FROM Supplier WHERE supplier_id = " + to_string(supplierID);
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if (row) {
        cout << "\n--- Supplier Details ---" << endl;
        cout << "ID: " << row[0] << endl;
        cout << "Supplier Name: " << row[1] << endl;
        cout << "Contact Person: " << row[2] << endl;
        cout << "Phone: " << row[3] << endl;
        cout << "Status: " << row[4] << endl;
        cout << "------------------------" << endl;
    } else {
        cout << "[ERROR] Supplier not found." << endl;
    }
}

// UPDATE - Update supplier details
void SupplierManager::updateSupplier(Database& db) {
    int supplierID;
    cout << "Enter Supplier ID to update: ";
    while (!(cin >> supplierID) || supplierID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid Supplier ID: ";
    }
    
    viewSupplier(db, supplierID);
    
    cout << "What do you want to update?" << endl;
    cout << "1. Supplier Name" << endl;
    cout << "2. Contact Person" << endl;
    cout << "3. Phone Number" << endl;
    cout << "4. Status" << endl;
    cout << "Select: ";
    
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 4) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-4: ";
    }
    cin.ignore();
    
    string query;
    if (choice == 1) {
        string newName;
        cout << "Enter new Supplier Name: ";
        getline(cin, newName);
        query = "UPDATE Supplier SET supplier_name = '" + newName + "' WHERE supplier_id = " + to_string(supplierID);
    }
    else if (choice == 2) {
        string newContact;
        cout << "Enter new Contact Person: ";
        getline(cin, newContact);
        query = "UPDATE Supplier SET contact_person = '" + newContact + "' WHERE supplier_id = " + to_string(supplierID);
    }
    else if (choice == 3) {
        string newPhone;
        cout << "Enter new Phone Number: ";
        getline(cin, newPhone);
        query = "UPDATE Supplier SET phone = '" + newPhone + "' WHERE supplier_id = " + to_string(supplierID);
    }
    else if (choice == 4) {
        cout << "Enter Status (Active/Inactive): ";
        string newStatus;
        getline(cin, newStatus);
        query = "UPDATE Supplier SET status = '" + newStatus + "' WHERE supplier_id = " + to_string(supplierID);
    }
    
    db.executeQuery(query);
    cout << "[OK] Supplier Updated Successfully." << endl;
}

// DELETE - Delete supplier
void SupplierManager::deleteSupplier(Database& db) {
    int supplierID;
    cout << "Enter Supplier ID to delete: ";
    while (!(cin >> supplierID) || supplierID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid Supplier ID: ";
    }
    
    viewSupplier(db, supplierID);
    
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
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          ADD NEW SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            addSupplier(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          VIEW ALL SUPPLIERS            " << endl;
            cout << "========================================\n" << endl;
            viewSuppliers(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "        VIEW SUPPLIER DETAILS           " << endl;
            cout << "========================================\n" << endl;
            int supplierID;
            cout << "Enter Supplier ID: ";
            while (!(cin >> supplierID) || supplierID <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid Supplier ID: ";
            }
            cin.ignore(10000, '\n');
            viewSupplier(db, supplierID);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 4) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          UPDATE SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            updateSupplier(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 5) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "          DELETE SUPPLIER              " << endl;
            cout << "========================================\n" << endl;
            deleteSupplier(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 6) {
            break;
        }
    }
}
