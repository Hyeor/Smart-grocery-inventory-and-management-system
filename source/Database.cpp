#include <mysql.h>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

class Database {
public:
    MYSQL* conn;

    Database() {
        try {
            conn = mysql_init(0);
            if (!conn) {
                throw runtime_error("Failed to initialize MySQL connection");
            }
            
            conn = mysql_real_connect(conn, "localhost", "root", "", "smart_grocery", 3306, NULL, 0);
            if (conn) {
                // Set character set to UTF-8 for proper encoding
                if (mysql_set_character_set(conn, "utf8mb4")) {
                    throw runtime_error("Failed to set character set to UTF-8");
                }
                //cout << "Database Connected Successfully!" << endl;
            } else {
                string error = "Database connection failed: ";
                error += mysql_error(conn);
                throw runtime_error(error);
            }
        } catch (const exception& e) {
            cerr << "[DATABASE ERROR] " << e.what() << endl;
            conn = nullptr;
        }
    }
    
    // Helper to execute simple queries
    void executeQuery(string query) {
        try {
            if (!conn) {
                throw runtime_error("Database connection is not established");
            }
            if (mysql_query(conn, query.c_str())) {
                string error = "Query execution failed: ";
                error += mysql_error(conn);
                throw runtime_error(error);
            }
        } catch (const exception& e) {
            cerr << "[QUERY ERROR] " << e.what() << endl;
        }
    }
    
    ~Database() {
        if (conn) {
            mysql_close(conn);
        }
    }
};