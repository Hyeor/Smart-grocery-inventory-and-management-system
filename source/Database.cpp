#include <mysql.h>
#include <iostream>
#include <string>

using namespace std;

class Database {
public:
    MYSQL* conn;

    Database() {
        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "root", "", "smart_grocery", 3306, NULL, 0);
        if (conn) {
            cout << "Database Connected Successfully!" << endl;
        } else {
            cout << "Failed to connect to database." << endl;
        }
    }
    
    // Helper to execute simple queries
    void executeQuery(string query) {
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
    }
};