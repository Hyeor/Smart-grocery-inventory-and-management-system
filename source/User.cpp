#include <iostream>
#include <string>
#include <cstdio>
#include <mysql.h>
#include "Database.h"
#include "user.h"

using namespace std;

// CREATE - Add new user
void UserManager::addUser(Database& db) {
    string username, password;
    int roleChoice;
    string role;
    
    cout << "\n=== ADD NEW USER ===" << endl;
    cout << "Enter Username: ";
    cin.ignore();
    getline(cin, username);
    
    while (username.empty()) {
        cout << "Error: Username cannot be empty!" << endl;
        cout << "Enter Username: ";
        getline(cin, username);
    }

        cout << "\nSelect User Role:" << endl;
        cout << "1. Admin" << endl;
        cout << "2. Staff" << endl;
        cout << "Select: ";
        while (!(cin >> roleChoice) || roleChoice < 1 || roleChoice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        cin.ignore();
        if (roleChoice == 1) {
            role = "Admin";
        } else {
            role = "Staff";
        }
    
    if (roleChoice == 1) {
        role = "Admin";
    } else {
        role = "Staff";
    }
    
    string query = "INSERT INTO User (username, password, role) VALUES ('" 
                   + username + "', '" + password + "', '" + role + "')";
    db.executeQuery(query);
    cout << "✓ User Added Successfully." << endl;
}

// READ - View all users
void UserManager::viewUsers(Database& db) {
    string query = "SELECT user_id, username, role FROM User ORDER BY user_id";
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row;

    cout << "\n=== LIST OF SYSTEM USERS ===" << endl;
    cout << "========================================================================" << endl;
    cout << "#  | User ID  | Username         | User Type                        " << endl;
    cout << "========================================================================" << endl;
    
    int count = 0;
    while ((row = mysql_fetch_row(res))) {
        count++;
        printf("%-2d | %-8s | %-16s | %s\n",
               count, row[0], row[1], row[2]);
    }
    cout << "========================================================================" << endl;
    printf("Showing 1 to %d of %d entries\n", count, count);
}

// READ - View single user by ID
void UserManager::viewUser(Database& db, int userID) {
    string query = "SELECT user_id, username, password, role FROM User WHERE user_id = " + to_string(userID);
    mysql_query(db.conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(db.conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if (row) {
        cout << "\n--- User Details ---" << endl;
        cout << "ID: " << row[0] << endl;
        cout << "Username: " << row[1] << endl;
        cout << "Password: " << row[2] << endl;
        cout << "Role: " << row[3] << endl;
        cout << "---------------------" << endl;
    } else {
        cout << "✗ User not found." << endl;
    }
}

// UPDATE - Update user details
void UserManager::updateUser(Database& db) {
    int userID;
    cout << "Enter User ID to update: ";
    while (!(cin >> userID) || userID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid User ID: ";
    }
    
    viewUser(db, userID);
    
    cout << "\nWhat do you want to update?" << endl;
    cout << "1. Username" << endl;
    cout << "2. Password" << endl;
    cout << "3. User Role" << endl;
    cout << "Select: ";
    
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 3) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice! Select 1-3: ";
    }
    cin.ignore();
    
    string query;
    if (choice == 1) {
        string newUsername;
        cout << "Enter new Username: ";
        getline(cin, newUsername);
        query = "UPDATE User SET username = '" + newUsername + "' WHERE user_id = " + to_string(userID);
    }
    else if (choice == 2) {
        string newPassword;
        cout << "Enter new Password: ";
        getline(cin, newPassword);
        query = "UPDATE User SET password = '" + newPassword + "' WHERE user_id = " + to_string(userID);
    }
    else if (choice == 3) {
        cout << "Select new User Role:" << endl;
        cout << "1. Admin" << endl;
        cout << "2. Staff" << endl;
        cout << "Select: ";
        
        int roleChoice;
        while (!(cin >> roleChoice) || roleChoice < 1 || roleChoice > 2) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-2: ";
        }
        
        string newRole;
        if (roleChoice == 1) {
            newRole = "Admin";
        } else {
            newRole = "Staff";
        }
        
        query = "UPDATE User SET role = '" + newRole + "' WHERE user_id = " + to_string(userID);
    }
    
    db.executeQuery(query);
    cout << "✓ User Updated Successfully." << endl;
}

// DELETE - Delete user
void UserManager::deleteUser(Database& db) {
    int userID;
    cout << "Enter User ID to delete: ";
    while (!(cin >> userID) || userID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! Enter valid User ID: ";
    }
    
    viewUser(db, userID);
    
    cout << "\nAre you sure you want to delete this user? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        string query = "DELETE FROM User WHERE user_id = " + to_string(userID);
        db.executeQuery(query);
        cout << "✓ User Deleted Successfully." << endl;
    } else {
        cout << "Deletion cancelled." << endl;
    }
}

// User Management Menu
void UserManager::userManagementMenu(Database& db) {
    int choice;
    while (true) {
        cout << "\n=== USER MANAGEMENT ===" << endl;
        cout << "1. Create New User" << endl;
        cout << "2. View All Users" << endl;
        cout << "3. View Specific User" << endl;
        cout << "4. Update User" << endl;
        cout << "5. Delete User" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Select: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-6: ";
        }
        
        if (choice == 1) {
            addUser(db);
        }
        else if (choice == 2) {
            viewUsers(db);
        }
        else if (choice == 3) {
            int userID;
            cout << "Enter User ID: ";
            while (!(cin >> userID) || userID <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Enter valid User ID: ";
            }
            viewUser(db, userID);
        }
        else if (choice == 4) {
            updateUser(db);
        }
        else if (choice == 5) {
            deleteUser(db);
        }
        else if (choice == 6) {
            break;
        }
    }
}