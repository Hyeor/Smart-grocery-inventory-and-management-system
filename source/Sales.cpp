#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <mysql.h>
#include "Database.h"
#include "Sales.h"

using namespace std;

#define COLOR_RESET     "\033[0m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_RED       "\033[31m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_CYAN      "\033[36m"

// Scan barcode and add to cart
void SalesManager::scanBarcode(Database& db, const string& barcode, int& quantity) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        // Find product by barcode
        string query = "SELECT product_id, name, sell_price, stock_quantity FROM Product WHERE barcode = '" + barcode + "'";
        
        if (mysql_query(db.conn, query.c_str())) {
            cout << COLOR_RED << "[ERROR] Database query failed" << COLOR_RESET << endl;
            return;
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            cout << COLOR_RED << "[ERROR] Failed to get product" << COLOR_RESET << endl;
            return;
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);
        
        if (row) {
            int productId = stoi(row[0]);
            string productName = row[1];
            double sellPrice = stod(row[2]);
            int currentStock = stoi(row[3]);
            
            if (currentStock <= 0) {
                cout << COLOR_RED << "\n[ERROR] '" << productName << "' is out of stock!" << COLOR_RESET << endl;
                mysql_free_result(res);
                return;
            }
            
            if (quantity > currentStock) {
                cout << COLOR_RED << "\n[ERROR] Only " << currentStock << " units available!" << COLOR_RESET << endl;
                mysql_free_result(res);
                return;
            }
            
            // Check if item already in cart
            bool found = false;
            for (auto& item : cart) {
                if (item.productId == productId) {
                    if (item.quantity + quantity > currentStock) {
                        cout << COLOR_RED << "\n[ERROR] Cannot add more. Only " << currentStock << " units available!" << COLOR_RESET << endl;
                        mysql_free_result(res);
                        return;
                    }
                    item.quantity += quantity;
                    item.totalPrice = item.quantity * item.unitPrice;
                    cout << COLOR_GREEN << "\n[OK] Added " << quantity << " more to '" << productName << "'" << COLOR_RESET << endl;
                    found = true;
                    break;
                }
            }
            
            // Add new item to cart
            if (!found) {
                CartItem newItem;
                newItem.productId = productId;
                newItem.productName = productName;
                newItem.barcode = barcode;
                newItem.unitPrice = sellPrice;
                newItem.quantity = quantity;
                newItem.totalPrice = sellPrice * quantity;
                cart.push_back(newItem);
                cout << COLOR_GREEN << "\n[OK] '" << productName << "' added to cart" << COLOR_RESET << endl;
            }
        } else {
            cout << COLOR_RED << "\n[ERROR] Barcode not found in inventory!" << COLOR_RESET << endl;
        }
        
        mysql_free_result(res);
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "\n[ERROR] " << e.what() << COLOR_RESET << endl;
    }
}

// Display cart items
void SalesManager::displayCart() {
    if (cart.empty()) {
        cout << "\n" << COLOR_YELLOW << "Cart is empty" << COLOR_RESET << endl;
        return;
    }
    
    cout << "\n";
    cout << "==============================================================================" << endl;
    cout << " No  | Product Name              | Qty | Unit Price | Total Price" << endl;
    cout << "==============================================================================" << endl;
    
    int index = 1;
    for (const auto& item : cart) {
        printf(" %-3d | %-25s | %-3d | RM %-8.2f | RM %.2f\n", 
               index, item.productName.substr(0, 25).c_str(), item.quantity, item.unitPrice, item.totalPrice);
        index++;
    }
    
    cout << "==============================================================================" << endl;
    cout << " SUBTOTAL: RM " << fixed << setprecision(2) << getCartTotal() << endl;
    cout << "==============================================================================" << endl;
}

// Remove item from cart
void SalesManager::removeFromCart(int index) {
    if (index < 1 || index > (int)cart.size()) {
        cout << COLOR_RED << "[ERROR] Invalid item number!" << COLOR_RESET << endl;
        return;
    }
    
    string removedItem = cart[index - 1].productName;
    cart.erase(cart.begin() + index - 1);
    cout << COLOR_GREEN << "[OK] Removed '" << removedItem << "' from cart" << COLOR_RESET << endl;
}

// Clear cart
void SalesManager::clearCart() {
    cart.clear();
    cout << COLOR_GREEN << "[OK] Cart cleared" << COLOR_RESET << endl;
}

// Get cart total
double SalesManager::getCartTotal() {
    double total = 0;
    for (const auto& item : cart) {
        total += item.totalPrice;
    }
    return total;
}

// Process payment and save to database
void SalesManager::processPayment(Database& db, int userID) {
    try {
        if (cart.empty()) {
            cout << COLOR_RED << "[ERROR] Cart is empty! Cannot process payment." << COLOR_RESET << endl;
            return;
        }
        
        double totalAmount = getCartTotal();
        double profit = 0;
        
        // Calculate profit and update stock for each item
        for (const auto& item : cart) {
            // Get cost price
            string getCostQuery = "SELECT cost_price FROM Product WHERE product_id = " + to_string(item.productId);
            if (mysql_query(db.conn, getCostQuery.c_str())) {
                throw runtime_error("Failed to fetch cost price");
            }
            
            MYSQL_RES* costRes = mysql_store_result(db.conn);
            MYSQL_ROW costRow = mysql_fetch_row(costRes);
            double costPrice = stod(costRow[0]);
            mysql_free_result(costRes);
            
            // Add to profit
            profit += (item.unitPrice - costPrice) * item.quantity;
            
            // Update stock
            string updateStockQuery = "UPDATE Product SET stock_quantity = stock_quantity - " + 
                                     to_string(item.quantity) + " WHERE product_id = " + 
                                     to_string(item.productId);
            db.executeQuery(updateStockQuery);
        }
        
        // Record sale
        string saleQuery = "INSERT INTO Sales (user_id, total_amount, profit) VALUES (" 
                          + to_string(userID) + ", " + to_string(totalAmount) + ", " + 
                          to_string(profit) + ")";
        db.executeQuery(saleQuery);
        
        cout << COLOR_GREEN << "\n===================================================" << COLOR_RESET << endl;
        cout << COLOR_GREEN << "  PAYMENT SUCCESSFUL                                " << COLOR_RESET << endl;
        cout << COLOR_GREEN << "===================================================" << COLOR_RESET << endl;
        cout << "\nTotal Amount: RM " << fixed << setprecision(2) << totalAmount << endl;
        cout << "Profit: RM " << profit << endl;
        
        generateReceipt(userID, totalAmount);
        
        clearCart();
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "\n[ERROR] " << e.what() << COLOR_RESET << endl;
    }
}

// Generate receipt
void SalesManager::generateReceipt(int userID, double totalAmount) {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char dateBuffer[20];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d_%H%M%S", timeinfo);
    
    string filename = "reports/Receipt_" + string(dateBuffer) + ".txt";
    
    #ifdef _WIN32
    system("if not exist reports mkdir reports");
    #else
    system("mkdir -p reports");
    #endif
    
    ofstream receipt(filename);
    if (!receipt.is_open()) {
        cout << COLOR_RED << "[ERROR] Could not create receipt file" << COLOR_RESET << endl;
        return;
    }
    
    receipt << "============================================\n";
    receipt << "   SMART GROCERY MANAGEMENT SYSTEM\n";
    receipt << "         SALES RECEIPT\n";
    receipt << "============================================\n\n";
    receipt << "Date: " << dateBuffer << "\n";
    receipt << "Cashier ID: " << userID << "\n";
    receipt << "---------------------------------------------\n";
    receipt << "Item                    Qty  Price  Total\n";
    receipt << "---------------------------------------------\n";
    
    for (const auto& item : cart) {
        receipt << item.productName.substr(0, 20) << "  " << item.quantity << "  RM" 
               << fixed << setprecision(2) << item.unitPrice << "  RM" << item.totalPrice << "\n";
    }
    
    receipt << "---------------------------------------------\n";
    receipt << "TOTAL: RM " << fixed << setprecision(2) << totalAmount << "\n";
    receipt << "==============================================\n";
    receipt << "Thank you for shopping!\n";
    
    receipt.close();
    cout << "\n[INFO] Receipt saved to: " << filename << endl;
}

// Cashier mode - main barcode scanning interface
void SalesManager::cashierMode(Database& db, int userID) {
    int choice;
    
    while (true) {
        system("cls");
        cout << "\n";
        cout << COLOR_CYAN << "==========================================================" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "         CASHIER MODE - BARCODE SCANNER                  " << COLOR_RESET << endl;
        cout << COLOR_CYAN << "==========================================================" << COLOR_RESET << endl;
        
        displayCart();
        
        cout << "\nOptions:" << endl;
        cout << "1. Scan Barcode / Add Item" << endl;
        cout << "2. Remove Item from Cart" << endl;
        cout << "3. Clear Cart" << endl;
        cout << "4. Process Payment" << endl;
        cout << "5. Exit Cashier" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "Select option: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice! Select 1-5: ";
        }
        cin.ignore();
        
        if (choice == 1) {
            string barcode;
            cout << "\nScan barcode (or enter manually): ";
            getline(cin, barcode);
            
            if (barcode.empty()) {
                continue;
            }
            
            int qty = 1;
            cout << "Enter quantity (default 1): ";
            string qtyStr;
            getline(cin, qtyStr);
            if (!qtyStr.empty()) {
                try {
                    qty = stoi(qtyStr);
                    if (qty <= 0) {
                        cout << COLOR_RED << "[ERROR] Quantity must be positive!" << COLOR_RESET << endl;
                        cin.ignore();
                        continue;
                    }
                } catch (...) {
                    cout << COLOR_RED << "[ERROR] Invalid quantity!" << COLOR_RESET << endl;
                    continue;
                }
            }
            
            scanBarcode(db, barcode, qty);
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 2) {
            displayCart();
            if (!cart.empty()) {
                cout << "\nEnter item number to remove (0 to cancel): ";
                int itemNum;
                while (!(cin >> itemNum)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input! Enter item number: ";
                }
                cin.ignore();
                
                if (itemNum != 0) {
                    removeFromCart(itemNum);
                }
            }
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 3) {
            if (!cart.empty()) {
                cout << "\nAre you sure you want to clear the cart? (yes/no): ";
                string confirm;
                getline(cin, confirm);
                if (confirm == "yes" || confirm == "YES") {
                    clearCart();
                }
            }
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 4) {
            if (cart.empty()) {
                cout << COLOR_RED << "\n[ERROR] Cart is empty!" << COLOR_RESET << endl;
                cout << "\nPress Enter to continue...";
                cin.ignore();
                continue;
            }
            
            displayCart();
            cout << "\nConfirm payment? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            
            if (confirm == "yes" || confirm == "YES") {
                processPayment(db, userID);
            }
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
        else if (choice == 5) {
            if (!cart.empty()) {
                cout << "\nCart has items! Are you sure you want to exit? (yes/no): ";
                string confirm;
                getline(cin, confirm);
                if (confirm != "yes" && confirm != "YES") {
                    continue;
                }
            }
            break;
        }
    }
}

// Old processSale function - kept for backward compatibility
void SalesManager::processSale(Database& db, int userID) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        int prodID, qty;
        cout << "Enter Product ID to sell (0 to go back): ";
        while (!(cin >> prodID) || prodID < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter valid Product ID (0 to go back): ";
        }
        if (prodID == 0) {
            cin.ignore(10000, '\n');
            return;
        }
        cout << "Enter Quantity: ";
        while (!(cin >> qty) || qty <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Enter positive quantity: ";
        }
        cin.ignore();

        string query = "SELECT sell_price, cost_price, stock_quantity FROM Product WHERE product_id = " + to_string(prodID);
        if (mysql_query(db.conn, query.c_str())) {
            throw runtime_error("Failed to fetch product details: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* res = mysql_store_result(db.conn);
        if (!res) {
            throw runtime_error("Failed to store result set");
        }
        
        MYSQL_ROW row = mysql_fetch_row(res);

        if (row) {
            double sellPrice = stod(row[0]);
            double costPrice = stod(row[1]);
            int currentStock = stoi(row[2]);

            if (currentStock >= qty) {
                double totalAmount = sellPrice * qty;
                double profit = (sellPrice - costPrice) * qty;

                string saleQuery = "INSERT INTO Sales (user_id, total_amount, profit) VALUES (" 
                                   + to_string(userID) + ", " + to_string(totalAmount) + ", " + to_string(profit) + ")";
                db.executeQuery(saleQuery);

                string updateStock = "UPDATE Product SET stock_quantity = stock_quantity - " + to_string(qty) 
                                     + " WHERE product_id = " + to_string(prodID);
                db.executeQuery(updateStock);

                cout << "\n[OK] Sale Recorded Successfully!" << endl;
                cout << "Total: RM" << totalAmount << " | Profit: RM" << profit << endl;
            } else {
                cout << "\n[ERROR] Insufficient Stock!" << endl;
            }
            
            mysql_free_result(res);
        } else {
            mysql_free_result(res);
            cout << "\n[ERROR] Product not found." << endl;
        }
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] Failed to process sale: " << e.what() << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Sales Page
void SalesManager::salesPage(Database& db, int userID) {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "========================================" << endl;
        cout << "           SALES MANAGEMENT             " << endl;
        cout << "========================================" << endl;
        cout << "\nOptions:" << endl;
        cout << "1. Cashier Mode (Barcode Scanner)" << endl;
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
            cashierMode(db, userID);
        }
        else if (choice == 2) {
            break;
        }
    }
}