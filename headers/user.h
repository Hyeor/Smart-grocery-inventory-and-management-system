#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <mysql.h>
#include "Database.h"

using namespace std;

class UserManager {
public:
    // Returns the role if login success, else returns empty string
    string login(Database& db) {
        string username, password;
        int attempts = 3;
        
        while (attempts > 0) {
            cout << "\n=== LOGIN ===" << endl;
            cout << "Username: "; cin >> username;
            cout << "Password: "; cin >> password;

            string query = "SELECT role FROM User WHERE username = '" + username + "' AND password = '" + password + "'";
            
            if (mysql_query(db.conn, query.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(db.conn);
                MYSQL_ROW row = mysql_fetch_row(res);
                
                if (row) {
                    cout << "\n✓ Login Valid! Welcome " << username << endl;
                    return string(row[0]);
                }
            }
            
            attempts--;
            if (attempts > 0) {
                cout << "\n✗ Invalid Login. " << attempts << " attempt(s) remaining." << endl;
            } else {
                cout << "\n✗ Login Failed! Maximum attempts exceeded. Access Denied." << endl;
            }
        }
        return "";
    }
    
    // User Management Methods
    void addUser(Database& db);
    void viewUsers(Database& db);
    void viewUser(Database& db, int userID);
    void updateUser(Database& db);
    void deleteUser(Database& db);
    void userManagementMenu(Database& db);
};

#endif
