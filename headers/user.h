#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <cstdio>
#include <mysql.h>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <ctime>
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

    // Helper function to validate email format
    bool isValidEmail(const string& email) {
        size_t atPos = email.find('@');
        size_t dotPos = email.rfind('.');
        return atPos != string::npos && dotPos != string::npos && 
               atPos < dotPos && atPos > 0 && dotPos < email.length() - 1;
    }

    // Helper function to validate phone number (basic check for digits)
    bool isValidPhone(const string& phone) {
        if (phone.empty() || phone.length() < 7) return false;
        for (char c : phone) {
            if (!isdigit(c) && c != '-' && c != '+' && c != ' ') return false;
        }
        return true;
    }

    // Generate next staff ID in format STF001, STF002, etc.
    string generateStaffID(Database& db) {
        string query = "SELECT MAX(CAST(SUBSTRING(staff_id, 4) AS UNSIGNED)) FROM User";
        if (mysql_query(db.conn, query.c_str()) == 0) {
            MYSQL_RES* res = mysql_store_result(db.conn);
            MYSQL_ROW row = mysql_fetch_row(res);
            
            int nextID = 1;
            if (row && row[0]) {
                nextID = atoi(row[0]) + 1;
            }
            mysql_free_result(res);
            
            stringstream ss;
            ss << "STF" << setfill('0') << setw(3) << nextID;
            return ss.str();
        }
        return "STF001";
    }

    // Get current timestamp in YYYY-MM-DD HH:MM:SS format
    string getCurrentTimestamp() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        stringstream ss;
        ss << (1900 + timeinfo->tm_year) << "-"
           << setfill('0') << setw(2) << (1 + timeinfo->tm_mon) << "-"
           << setw(2) << timeinfo->tm_mday << " "
           << setw(2) << timeinfo->tm_hour << ":"
           << setw(2) << timeinfo->tm_min << ":"
           << setw(2) << timeinfo->tm_sec;
        return ss.str();
    }
    
    // Returns the role if login success, else returns empty string
    // Also handles forced password change on first login
    string login(Database& db) {
        string staffID, password;
        int attempts = 3;
        
        while (attempts > 0) {
            cout << "\n=== LOGIN ===" << endl;
            cout << "Staff ID: "; 
            fflush(stdin);  // Clear input buffer
            getline(cin, staffID);
            cout << "Password: "; 
            password = getPasswordInput();

            // Check if account exists and is active
            string query = "SELECT role, password_change_required FROM User WHERE staff_id = '" + staffID + 
                          "' AND password = MD5('" + password + "') AND account_status = 'ACTIVE'";
            
            if (mysql_query(db.conn, query.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(db.conn);
                if (res) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    
                    if (row) {
                        string role = (row[0] ? string(row[0]) : "");
                        int passwordChangeRequired = (row[1] ? atoi(row[1]) : 0);
                        
                        cout << "\n[OK] Login Valid! Welcome " << staffID << endl;
                        
                        // Update last access time
                        string updateAccess = "UPDATE User SET last_access = NOW() WHERE staff_id = '" + staffID + "'";
                        mysql_query(db.conn, updateAccess.c_str());
                        
                        // Force password change on first login
                        if (passwordChangeRequired == 1) {
                            cout << "\n[NOTICE] You must change your password on first login." << endl;
                            changePassword(db, staffID);
                        }
                        
                        mysql_free_result(res);
                        return role;
                    }
                    mysql_free_result(res);
                } else {
                    cout << "\n[ERROR] Database error: " << mysql_error(db.conn) << endl;
                    attempts--;
                    continue;
                }
            } else {
                cout << "\n[ERROR] Database query failed: " << mysql_error(db.conn) << endl;
                attempts--;
                continue;
            }
            
            attempts--;
            if (attempts > 0) {
                cout << "\n[ERROR] Invalid credentials or account is inactive. " << attempts << " attempt(s) remaining." << endl;
            } else {
                cout << "\n[ERROR] Login Failed! Maximum attempts exceeded. Access Denied." << endl;
            }
        }
        return "";
    }

    // Staff password change
    void changePassword(Database& db, const string& staffID) {
        cout << "\n=== CHANGE PASSWORD ===" << endl;
        string oldPassword, newPassword, confirmPassword;
        
        cout << "Enter current password: ";
        oldPassword = getPasswordInput();
        
        // Verify old password
        string verifyQuery = "SELECT user_id FROM User WHERE staff_id = '" + staffID + 
                            "' AND password = MD5('" + oldPassword + "')";
        if (mysql_query(db.conn, verifyQuery.c_str()) != 0) {
            cout << "[ERROR] Password verification failed." << endl;
            return;
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) {
            cout << "[ERROR] Current password is incorrect." << endl;
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
        
        // Get new password
        bool validPassword = false;
        while (!validPassword) {
            cout << "\nEnter new password: ";
            newPassword = getPasswordInput();
            
            if (newPassword.length() < 6) {
                cout << "[ERROR] Password must be at least 6 characters." << endl;
                continue;
            }
            
            cout << "Confirm new password: ";
            confirmPassword = getPasswordInput();
            
            if (newPassword == confirmPassword) {
                validPassword = true;
            } else {
                cout << "[ERROR] Passwords do not match. Please try again." << endl;
            }
        }
        
        // Update password and reset the flag
        string updateQuery = "UPDATE User SET password = MD5('" + newPassword + 
                            "'), password_change_required = 0 WHERE staff_id = '" + staffID + "'";
        db.executeQuery(updateQuery);
        cout << "[OK] Password changed successfully!" << endl;
    }
    
    // User Management Methods
    void addUser(Database& db);
    void viewUsers(Database& db);
    void viewUser(Database& db, const string& staffID);
    void updateUser(Database& db);
    void deactivateUser(Database& db);
    void reactivateUser(Database& db);
    void manageStaffStatus(Database& db);
    void userManagementMenu(Database& db);
    void staffManagementMenu(Database& db);
    void updateCashierProfile(Database& db);
    void inventoryAdminMenu(Database& db);
    void updateInventoryAdminProfile(Database& db);
};

#endif
