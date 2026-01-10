#include <iostream>
#include <string>
#include <cstdio>
#include <stdexcept>
#include <mysql.h>
#include <ctime>
#include <iomanip>
#include "Database.h"
#include "user.h"
#include "UI_Helpers.h"

using namespace std;

// CREATE - Add new staff user
void UserManager::addUser(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string staffID, fullName, email, phone, position;
        int roleChoice;
        string role;
        string defaultPassword = "admin";  // Default password for first login
        
        cout << "\n=== ADD NEW STAFF ===" << endl;
        
        // Generate Staff ID
        staffID = generateStaffID(db);
        cout << "\nAuto-generated Staff ID: " << staffID << endl;
        
        // Get Full Name
        cout << "Enter Full Name (0 to cancel): ";
        cin.ignore();
        getline(cin, fullName);
        if (fullName == "0") return;
        while (fullName.empty()) {
            cout << "Error: Full Name cannot be empty!" << endl;
            cout << "Enter Full Name (0 to cancel): ";
            getline(cin, fullName);
            if (fullName == "0") return;
        }
        
        // Get Email
        cout << "Enter Email Address (0 to cancel): ";
        getline(cin, email);
        if (email == "0") return;
        while (!isValidEmail(email)) {
            cout << "Error: Invalid email format! Please try again." << endl;
            cout << "Enter Email Address (0 to cancel): ";
            getline(cin, email);
            if (email == "0") return;
        }
        
        // Get Phone Number
        cout << "Enter Phone Number (0 to cancel): ";
        getline(cin, phone);
        if (phone == "0") return;
        while (!isValidPhone(phone)) {
            cout << "Error: Invalid phone number! Must contain at least 7 digits." << endl;
            cout << "Enter Phone Number (0 to cancel): ";
            getline(cin, phone);
            if (phone == "0") return;
        }
        
        // Get Position/Job Title
        cout << "Enter Position/Job Title (0 to cancel): ";
        getline(cin, position);
        if (position == "0") return;
        while (position.empty()) {
            cout << "Error: Position cannot be empty!" << endl;
            cout << "Enter Position/Job Title (0 to cancel): ";
            getline(cin, position);
            if (position == "0") return;
        }
        
        // Select Role
        cout << "\nSelect Role/Access Level:" << endl;
        cout << "1. ADMIN (Full system access)" << endl;
        cout << "2. STAFF (Limited access: Dashboard, Inventory, Sales)" << endl;
        cout << "3. INVENTORY ADMIN (Inventory & Supplier Management)" << endl;
        cout << "Select: ";
        while (!(cin >> roleChoice) || roleChoice < 1 || roleChoice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-3: ";
        }
        cin.ignore();
        
        if (roleChoice == 1) {
            role = "ADMIN";
        } else if (roleChoice == 2) {
            role = "STAFF";
        } else {
            role = "INVENTORY ADMIN";
        }
        
        // Get current timestamp
        string dateCreated = getCurrentTimestamp();
        
        // Insert new staff with default password and password change flag
        string query = "INSERT INTO User (staff_id, full_name, email, phone, position, role, "
                      "password, account_status, password_change_required, date_created) "
                      "VALUES ('" + staffID + "', '" + fullName + "', '" + email + "', '" 
                      + phone + "', '" + position + "', '" + role + "', MD5('" + defaultPassword 
                      + "'), 'ACTIVE', 1, '" + dateCreated + "')";
        
        db.executeQuery(query);
        cout << "\n[OK] New staff account created successfully!" << endl;
        cout << "Staff ID: " << staffID << endl;
        cout << "Default Password: " << defaultPassword << endl;
        cout << "[NOTE] Staff must change password on first login." << endl;
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] Failed to add staff: " << e.what() << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// READ - View all staff
void UserManager::viewUsers(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string query = "SELECT staff_id, TRIM(full_name), email, phone, position, COALESCE(role, 'UNASSIGNED'), account_status, date_created FROM User ORDER BY date_created DESC";
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch staff: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row;

        cout << "\n" << setfill('=') << setw(140) << "" << setfill(' ') << endl;
        cout << "                           === LIST OF STAFF ACCOUNTS ===" << endl;
        cout << setfill('=') << setw(140) << "" << setfill(' ') << endl;
        
        // Header row
        cout << left << setw(12) << "Staff ID" 
             << " | " << left << setw(28) << "Full Name"
             << " | " << left << setw(20) << "Position"
             << " | " << left << setw(16) << "Role"
             << " | " << left << setw(10) << "Status"
             << " | " << left << setw(25) << "Date Created" << endl;
        
        cout << setfill('-') << setw(140) << "" << setfill(' ') << endl;
        
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            count++;
            // Truncate long fields for display
            string fullName = (row[1] ? string(row[1]) : "");
            if (fullName.length() > 26) fullName = fullName.substr(0, 23) + "...";
            
            string position = (row[4] ? string(row[4]) : "");
            if (position.length() > 18) position = position.substr(0, 15) + "...";
            
            string role = (row[5] ? string(row[5]) : "");
            if (role.length() > 14) role = role.substr(0, 11) + "...";
            
            // Use cout with setw for proper alignment
            cout << left << setw(12) << (row[0] ? row[0] : "")
                 << " | " << left << setw(28) << fullName
                 << " | " << left << setw(20) << position
                 << " | " << left << setw(16) << role
                 << " | " << left << setw(10) << (row[6] ? row[6] : "")
                 << " | " << left << setw(25) << (row[7] ? row[7] : "") << endl;
        }
        cout << setfill('=') << setw(140) << "" << setfill(' ') << endl;
        cout << "\nShowing " << count << " entr" << (count == 1 ? "y" : "ies") << endl;
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// READ - View single staff by Staff ID
void UserManager::viewUser(Database& db, const string& staffID) {
    try {
        string query = "SELECT staff_id, full_name, email, phone, position, role, account_status, date_created, password_change_required, DATE_FORMAT(last_access, '%Y-%m-%d %H:%i:%s') as last_access_formatted FROM User WHERE staff_id = '" + staffID + "'";
        
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch staff: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row) {
            cout << "\n--- STAFF DETAILS ---" << endl;
            cout << "Full Name: " << row[1] << endl;
            cout << "Email: " << row[2] << endl;
            cout << "Phone: " << row[3] << endl;
            cout << "Position: " << row[4] << endl;
            cout << "Role/Access Level: " << row[5] << endl;
            cout << "Account Status: " << row[6] << endl;
            cout << "Date Created: " << row[7] << endl;
            cout << "Last Access: " << (row[9] ? row[9] : "Never") << endl;
            cout << "Password Change Required: " << (atoi(row[8]) ? "YES" : "NO") << endl;
            cout << "---------------------" << endl;
        } else {
            cout << "[ERROR] Staff not found." << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}

// UPDATE - Update staff details
void UserManager::updateUser(Database& db) {
    string staffID;
    cout << "Enter Staff ID to update (0 to go back): ";
    getline(cin, staffID);
    
    if (staffID == "0") return;
    
    viewUser(db, staffID);
    
    cout << "\nWhat do you want to update?" << endl;
    cout << "1. Full Name" << endl;
    cout << "2. Email" << endl;
    cout << "3. Phone" << endl;
    cout << "4. Position" << endl;
    cout << "5. Role/Access Level" << endl;
    cout << "6. Reset Password to 'admin'" << endl;
    cout << "Select: ";
    
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 6) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-6: ";
    }
    cin.ignore();
    
    string query;
    if (choice == 1) {
        string newName;
        cout << "Enter new Full Name (0 to cancel): ";
        getline(cin, newName);
        if (newName == "0") return;
        query = "UPDATE User SET full_name = '" + newName + "' WHERE staff_id = '" + staffID + "'";
    }
    else if (choice == 2) {
        string newEmail;
        cout << "Enter new Email (0 to cancel): ";
        getline(cin, newEmail);
        if (newEmail == "0") return;
        while (!isValidEmail(newEmail)) {
            cout << "Invalid email format! Please try again (0 to cancel): ";
            getline(cin, newEmail);
            if (newEmail == "0") return;
        }
        query = "UPDATE User SET email = '" + newEmail + "' WHERE staff_id = '" + staffID + "'";
    }
    else if (choice == 3) {
        string newPhone;
        cout << "Enter new Phone (0 to cancel): ";
        getline(cin, newPhone);
        if (newPhone == "0") return;
        while (!isValidPhone(newPhone)) {
            cout << "Invalid phone format! Please try again (0 to cancel): ";
            getline(cin, newPhone);
            if (newPhone == "0") return;
        }
        query = "UPDATE User SET phone = '" + newPhone + "' WHERE staff_id = '" + staffID + "'";
    }
    else if (choice == 4) {
        string newPosition;
        cout << "Enter new Position (0 to cancel): ";
        getline(cin, newPosition);
        if (newPosition == "0") return;
        query = "UPDATE User SET position = '" + newPosition + "' WHERE staff_id = '" + staffID + "'";
    }
    else if (choice == 5) {
        cout << "Select new Role:" << endl;
        cout << "1. ADMIN" << endl;
        cout << "2. STAFF" << endl;
        cout << "Select: ";
        
        int roleChoice;
        while (!(cin >> roleChoice) || roleChoice < 1 || roleChoice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        
        string newRole = (roleChoice == 1) ? "ADMIN" : "STAFF";
        query = "UPDATE User SET role = '" + newRole + "' WHERE staff_id = '" + staffID + "'";
    }
    else if (choice == 6) {
        cout << "\nAre you sure you want to reset the password to 'admin'? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            query = "UPDATE User SET password = MD5('admin'), password_change_required = 1 WHERE staff_id = '" + staffID + "'";
        } else {
            cout << "Password reset cancelled." << endl;
            return;
        }
    }
    
    db.executeQuery(query);
    cout << "[OK] Staff updated successfully." << endl;
}

// DEACTIVATE - Deactivate staff account
void UserManager::deactivateUser(Database& db) {
    string staffID;
    cout << "Enter Staff ID to deactivate (0 to go back): ";
    cin.ignore();
    getline(cin, staffID);
    
    if (staffID == "0") return;
    
    viewUser(db, staffID);
    
    cout << "\nAre you sure you want to deactivate this account? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        string query = "UPDATE User SET account_status = 'INACTIVE' WHERE staff_id = '" + staffID + "'";
        db.executeQuery(query);
        cout << "[OK] Staff account deactivated successfully." << endl;
    } else {
        cout << "Deactivation cancelled." << endl;
    }
}

// REACTIVATE - Reactivate staff account
void UserManager::reactivateUser(Database& db) {
    string staffID;
    cout << "Enter Staff ID to reactivate (0 to go back): ";
    cin.ignore();
    getline(cin, staffID);
    
    if (staffID == "0") return;
    
    viewUser(db, staffID);
    
    cout << "\nAre you sure you want to reactivate this account? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        string query = "UPDATE User SET account_status = 'ACTIVE' WHERE staff_id = '" + staffID + "'";
        db.executeQuery(query);
        cout << "[OK] Staff account reactivated successfully." << endl;
    } else {
        cout << "Reactivation cancelled." << endl;
    }
}

// Combined Status Management - Change staff account status
void UserManager::manageStaffStatus(Database& db) {
    // Show current staff list first for easier selection
    viewUsers(db);
    cout << "\n";

    string staffID;
    cout << "Enter Staff ID (0 to go back): ";
    // Do NOT call cin.ignore() here; the caller already cleared the buffer
    getline(cin, staffID);
    
    if (staffID == "0") return;
    
    viewUser(db, staffID);
    
    cout << "\nSelect Action:" << endl;
    cout << "1. Deactivate Account" << endl;
    cout << "2. Reactivate Account" << endl;
    cout << "3. Cancel" << endl;
    cout << "Select: ";
    
    int statusChoice;
    while (!(cin >> statusChoice) || statusChoice < 1 || statusChoice > 3) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-3: ";
    }
    
    if (statusChoice == 3) {
        return;
    }
    
    cout << "\nAre you sure you want to " 
         << (statusChoice == 1 ? "deactivate" : "reactivate") 
         << " this account? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        string newStatus = (statusChoice == 1) ? "INACTIVE" : "ACTIVE";
        string query = "UPDATE User SET account_status = '" + newStatus + "' WHERE staff_id = '" + staffID + "'";
        db.executeQuery(query);
        cout << "[OK] Staff account " 
             << (statusChoice == 1 ? "deactivated" : "reactivated") 
             << " successfully." << endl;
    } else {
        cout << "Action cancelled." << endl;
    }
}

// Staff Management Menu
void UserManager::userManagementMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "    STAFF MANAGEMENT SYSTEM            " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Register New Staff" << endl;
        cout << "2. View All Staff" << endl;
        cout << "3. View Staff Details" << endl;
        cout << "4. Update Staff Information" << endl;
        cout << "5. Status Staff Account" << endl;
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
            cout << "      REGISTER NEW STAFF              " << endl;
            cout << "========================================\n" << endl;
            addUser(db);
            cout << "\n";
            viewUsers(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "         VIEW ALL STAFF               " << endl;
            cout << "========================================\n" << endl;
            viewUsers(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 3) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "      VIEW STAFF DETAILS              " << endl;
            cout << "========================================\n" << endl;
            viewUsers(db);
            cout << "\n";
            string staffID;
            cout << "Enter Staff ID (0 to go back): ";
            getline(cin, staffID);
            if (staffID == "0") continue;
            viewUser(db, staffID);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 4) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "    UPDATE STAFF INFORMATION          " << endl;
            cout << "========================================\n" << endl;
            viewUsers(db);
            cout << "\n";
            updateUser(db);
            cout << "\n";
            viewUsers(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 5) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "      STATUS STAFF ACCOUNT            " << endl;
            cout << "========================================\n" << endl;
            manageStaffStatus(db);
            cout << "\n";
            viewUsers(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 6) {
            break;
        }
    }
}

// Staff Management Menu for Cashiers - Limited options
void UserManager::staffManagementMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "    STAFF MANAGEMENT SYSTEM            " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Update Staff Information" << endl;
        cout << "2. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";

        while (!(cin >> choice) || choice < 1 || choice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        cin.ignore(10000, '\n');

        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "    UPDATE STAFF INFORMATION          " << endl;
            cout << "========================================\n" << endl;

            string staffID;
            cout << "Enter your Staff ID (0 to go back): ";
            getline(cin, staffID);

            if (staffID == "0") {
                return;
            }

            if (staffID.empty()) {
                cout << "[ERROR] Staff ID cannot be empty." << endl;
                return;
            }

            // Verify staff exists
            string checkQuery = "SELECT staff_id, full_name, email, phone FROM User WHERE staff_id = '" + staffID + "'";
            if (mysql_query(db.conn, checkQuery.c_str())) {
                throw runtime_error("Failed to fetch staff: " + string(mysql_error(db.conn)));
            }

            MYSQL_RES* res = mysql_store_result(db.conn);
            if (!res) {
                throw runtime_error("Failed to store result set");
            }

            MYSQL_ROW row = mysql_fetch_row(res);
            if (!row) {
                cout << "[ERROR] Staff not found." << endl;
                mysql_free_result(res);
                return;
            }

            // Display current info
            cout << "\n--- Current Information ---" << endl;
            cout << "Full Name: " << (row[1] ? row[1] : "N/A") << endl;
            cout << "Email: " << (row[2] ? row[2] : "N/A") << endl;
            cout << "Phone: " << (row[3] ? row[3] : "N/A") << endl;
            cout << "----------------------------\n" << endl;

            mysql_free_result(res);

            // Collect updates
            string newName = "";
            cout << "Enter new Full Name (or press Enter to skip): ";
            getline(cin, newName);

            string newEmail = "";
            cout << "Enter new Email (or press Enter to skip): ";
            getline(cin, newEmail);

            string newPhone = "";
            cout << "Enter new Phone Number (or press Enter to skip): ";
            getline(cin, newPhone);

            // If no updates, return
            if (newName.empty() && newEmail.empty() && newPhone.empty()) {
                cout << "[INFO] No changes made." << endl;
                return;
            }

            // Apply updates
            if (!newName.empty()) {
                string updateQuery = "UPDATE User SET full_name = '" + newName + "' WHERE staff_id = '" + staffID + "'";
                db.executeQuery(updateQuery);
                cout << "[OK] Full Name updated." << endl;
            }

            if (!newEmail.empty()) {
                string updateQuery = "UPDATE User SET email = '" + newEmail + "' WHERE staff_id = '" + staffID + "'";
                db.executeQuery(updateQuery);
                cout << "[OK] Email updated." << endl;
            }

            if (!newPhone.empty()) {
                string updateQuery = "UPDATE User SET phone = '" + newPhone + "' WHERE staff_id = '" + staffID + "'";
                db.executeQuery(updateQuery);
                cout << "[OK] Phone Number updated." << endl;
            }

            cout << "\n[SUCCESS] Profile updated successfully!" << endl;
            cout << "\nPress Enter to continue...";
            cin.ignore();
        } else if (choice == 2) {
            break;
        }
    }
}

// Inventory Admin Management Menu
void UserManager::inventoryAdminMenu(Database& db) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "    INVENTORY ADMIN USERS              " << endl;
        cout << "========================================" << endl;
        cout << "\nOperations:" << endl;
        cout << "1. Update User Profile" << endl;
        cout << "2. Back to Dashboard" << endl;
        cout << "========================================" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        cin.ignore(10000, '\n');
        
        if (choice == 1) {
            system("cls");
            cout << "\n========================================" << endl;
            cout << "    UPDATE USER PROFILE                " << endl;
            cout << "========================================\n" << endl;
            updateInventoryAdminProfile(db);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            break;
        }
    }
}

// Update Inventory Admin Profile - Limited to Full Name, Email, and Phone Number
void UserManager::updateInventoryAdminProfile(Database& db) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        string userID;
        cout << "Enter your User ID (0 to go back): ";
        getline(cin, userID);
        
        if (userID == "0") {
            return;
        }
        
        if (userID.empty()) {
            cout << "[ERROR] User ID cannot be empty." << endl;
            return;
        }
        
        // Verify user exists
        string checkQuery = "SELECT staff_id, full_name, email, phone FROM User WHERE staff_id = '" + userID + "'";
        if (mysql_query(db.conn, checkQuery.c_str())) {
            throw runtime_error("Failed to fetch user: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) {
            cout << "[ERROR] User not found." << endl;
            mysql_free_result(res);
            return;
        }
        
        // Display current info
        cout << "\n--- Current Information ---" << endl;
        cout << "Full Name: " << (row[1] ? row[1] : "N/A") << endl;
        cout << "Email: " << (row[2] ? row[2] : "N/A") << endl;
        cout << "Phone: " << (row[3] ? row[3] : "N/A") << endl;
        cout << "----------------------------\n" << endl;
        
        mysql_free_result(res);
        
        // Collect updates
        string newName = "";
        cout << "Enter new Full Name (or press Enter to skip): ";
        getline(cin, newName);
        
        string newEmail = "";
        cout << "Enter new Email (or press Enter to skip): ";
        getline(cin, newEmail);
        
        string newPhone = "";
        cout << "Enter new Phone Number (or press Enter to skip): ";
        getline(cin, newPhone);
        
        // If no updates, return
        if (newName.empty() && newEmail.empty() && newPhone.empty()) {
            cout << "[INFO] No changes made." << endl;
            return;
        }
        
        // Apply updates
        if (!newName.empty()) {
            string updateQuery = "UPDATE User SET full_name = '" + newName + "' WHERE staff_id = '" + userID + "'";
            db.executeQuery(updateQuery);
            cout << "[OK] Full Name updated." << endl;
        }
        
        if (!newEmail.empty()) {
            string updateQuery = "UPDATE User SET email = '" + newEmail + "' WHERE staff_id = '" + userID + "'";
            db.executeQuery(updateQuery);
            cout << "[OK] Email updated." << endl;
        }
        
        if (!newPhone.empty()) {
            string updateQuery = "UPDATE User SET phone = '" + newPhone + "' WHERE staff_id = '" + userID + "'";
            db.executeQuery(updateQuery);
            cout << "[OK] Phone Number updated." << endl;
        }
        
        cout << "\n[SUCCESS] Profile updated successfully!" << endl;
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << endl;
    }
}