#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>
#include <iostream>
#include <string>

using namespace std;

class Database {
public:
    MYSQL* conn;
    bool isConnected;

    Database() {
        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "root", "", "smart_grocery", 3306, NULL, 0);
        if (conn) {
            cout << "\n✓ Database Connected Successfully!" << endl;
            isConnected = true;
        } else {
            cout << "\n✗ Failed to connect to database." << endl;
            cout << "Make sure MySQL Server is running!" << endl;
            cout << "Error: " << mysql_error(conn) << endl;
            isConnected = false;
        }
    }
    
    // Helper to execute simple queries
    void executeQuery(string query) {
        if (!isConnected) {
            cout << "Error: No database connection!" << endl;
            return;
        }
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
    }
    
    // Check connection status
    bool isValid() {
        return isConnected && conn != nullptr;
    }
};

#endif
