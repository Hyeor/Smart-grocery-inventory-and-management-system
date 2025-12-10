#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <cstdio>
#include <mysql.h>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include "Database.h"

using namespace std;

class UserManager {
public:
    // Helper function to read password with masking (handles spaces)
    string getPasswordInput() {
        string password = "";
        char ch;
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD rec;
        DWORD count = 0;
        
        while (ReadConsoleInput(hStdin, &rec, 1, &count) && count) {
            // Only process key events
            if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
                ch = rec.Event.KeyEvent.uChar.AsciiChar;
                
                if (ch == '\r') {  // Enter key
                    cout << endl;
                    break;
                } 
                else if (ch == '\b') {  // Backspace
                    if (password.length() > 0) {
                        password.pop_back();
                        cout << "\b \b";
                        cout.flush();
                    }
                } 
                else if (ch >= 32 && ch < 127) {  // Printable characters (including space)
                    password += ch;
                    cout << "*";
                    cout.flush();
                }
            }
        }
        
        return password;
    }
    
    // Returns the role if login success, else returns empty string
    string login(Database& db) {
        string username, password;
        int attempts = 3;
        
        while (attempts > 0) {
            cout << "\n=== LOGIN ===" << endl;
            cout << "Username: "; 
            fflush(stdin);  // Clear input buffer
            getline(cin, username);
            cout << "Password: "; 
            password = getPasswordInput();

            // Debug: Show what was entered
            // cout << "\nDebug - Username: '" << username << "'" << endl;
            // cout << "Debug - Password: '" << password << "'" << endl;

            // Use MySQL MD5 function to hash the password for comparison
            string query = "SELECT role FROM User WHERE username = '" + username + "' AND password = MD5('" + password + "')";
            
            if (mysql_query(db.conn, query.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(db.conn);
                MYSQL_ROW row = mysql_fetch_row(res);
                
                if (row) {
                    cout << "\n[OK] Login Valid! Welcome " << username << endl;
                    return string(row[0]);
                }
            } else {
                // Debug: Show query error
                // cout << "\nDebug - Query error: " << mysql_error(db.conn) << endl;
            }
            
            attempts--;
            if (attempts > 0) {
                cout << "\n[ERROR] Invalid Login. " << attempts << " attempt(s) remaining." << endl;
            } else {
                cout << "\n[ERROR] Login Failed! Maximum attempts exceeded. Access Denied." << endl;
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
