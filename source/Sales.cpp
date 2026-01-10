#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <sstream>
#include <cctype>
#include <mysql.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Database.h"
#include "Sales.h"

using namespace std;

#define COLOR_RESET     "\033[0m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_RED       "\033[31m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_CYAN      "\033[36m"

constexpr double SST_RATE = 0.06; // 6% SST applied to cart totals

namespace {
bool parseDate(const string& dateStr, int& year, int& month, int& day) {
    if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return false;
    }

    for (size_t i = 0; i < dateStr.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (!isdigit(static_cast<unsigned char>(dateStr[i]))) {
            return false;
        }
    }

    year = stoi(dateStr.substr(0, 4));
    month = stoi(dateStr.substr(5, 2));
    day = stoi(dateStr.substr(8, 2));

    tm t{};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_isdst = -1;

    time_t ts = mktime(&t);
    if (ts == -1) {
        return false;
    }

    tm* validated = localtime(&ts);
    return validated && validated->tm_year == t.tm_year && validated->tm_mon == t.tm_mon && validated->tm_mday == t.tm_mday;
}

string formatDateString(int year, int month, int day) {
    stringstream ss;
    ss << setw(4) << setfill('0') << year << "-" << setw(2) << month << "-" << setw(2) << day;
    return ss.str();
}

string addOneDay(const string& dateStr) {
    int year = 0, month = 0, day = 0;
    if (!parseDate(dateStr, year, month, day)) {
        return "";
    }

    tm t{};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day + 1;
    t.tm_isdst = -1;

    time_t ts = mktime(&t);
    if (ts == -1) {
        return "";
    }

    tm* next = localtime(&ts);
    if (!next) {
        return "";
    }

    return formatDateString(next->tm_year + 1900, next->tm_mon + 1, next->tm_mday);
}

bool isStartBeforeOrEqual(const string& startDate, const string& endDate) {
    int sy = 0, sm = 0, sd = 0, ey = 0, em = 0, ed = 0;
    if (!parseDate(startDate, sy, sm, sd) || !parseDate(endDate, ey, em, ed)) {
        return false;
    }

    tm st{};
    st.tm_year = sy - 1900;
    st.tm_mon = sm - 1;
    st.tm_mday = sd;
    st.tm_isdst = -1;

    tm et{};
    et.tm_year = ey - 1900;
    et.tm_mon = em - 1;
    et.tm_mday = ed;
    et.tm_isdst = -1;

    time_t startTs = mktime(&st);
    time_t endTs = mktime(&et);

    if (startTs == -1 || endTs == -1) {
        return false;
    }

    return difftime(endTs, startTs) >= 0;
}
} // namespace

// Scan barcode and add to cart
void SalesManager::scanBarcode(Database& db, const string& barcode, int quantity) {
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
                    cout << COLOR_GREEN << "[OK] '" << productName << "' added to cart" << COLOR_RESET << endl;
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
                cout << COLOR_GREEN << "[OK] '" << productName << "' added to cart" << COLOR_RESET << endl;
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
    double subtotal = getCartTotal();
    double taxAmount = subtotal * SST_RATE;
    double totalWithTax = subtotal + taxAmount;

    cout << " SUBTOTAL: RM " << fixed << setprecision(2) << subtotal << endl;
    cout << " SST (6%): RM " << fixed << setprecision(2) << taxAmount << endl;
    cout << " TOTAL:    RM " << fixed << setprecision(2) << totalWithTax << endl;
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
        
        double subtotal = getCartTotal();
        double taxAmount = subtotal * SST_RATE;
        double totalAmount = subtotal + taxAmount;
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
        cout << "\nSubtotal:     RM " << fixed << setprecision(2) << subtotal << endl;
        cout << "SST (6%):     RM " << fixed << setprecision(2) << taxAmount << endl;
        cout << "Total Amount: RM " << fixed << setprecision(2) << totalAmount << endl;
        cout << "Profit:       RM " << profit << endl;

        generateReceipt(userID, subtotal, taxAmount, totalAmount);
        
        clearCart();
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "\n[ERROR] " << e.what() << COLOR_RESET << endl;
    }
}

// Generate receipt
void SalesManager::generateReceipt(int userID, double subtotal, double taxAmount, double totalAmount) {
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
    receipt << "          GROCERYFLOW IMS\n";
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
    receipt << "SUBTOTAL: RM " << fixed << setprecision(2) << subtotal << "\n";
    receipt << "SST (6%): RM " << fixed << setprecision(2) << taxAmount << "\n";
    receipt << "TOTAL:    RM " << fixed << setprecision(2) << totalAmount << "\n";
    receipt << "==============================================\n";
    receipt << "Thank you for shopping!\n";
    
    receipt.close();
    cout << "\n[INFO] Receipt saved to: " << filename << endl;
}

// Generate Monthly Sales Report
void SalesManager::generateMonthlySalesReport(Database& db, int userID, const string& month, const string& year) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }
        
        // Get user name for report header
        string userName = "Staff";
        string getUserQuery = "SELECT username FROM User WHERE user_id = " + to_string(userID);
        if (mysql_query(db.conn, getUserQuery.c_str()) == 0) {
            MYSQL_RES* userRes = mysql_store_result(db.conn);
            if (userRes) {
                MYSQL_ROW userRow = mysql_fetch_row(userRes);
                if (userRow && userRow[0]) {
                    userName = userRow[0];
                }
                mysql_free_result(userRes);
            }
        }
        
        // Generate date range for the month
        string startDate = year + "-" + month + "-01";
        string endDate;
        int monthInt = stoi(month);
        int yearInt = stoi(year);
        
        if (monthInt == 12) {
            endDate = to_string(yearInt + 1) + "-01-01";
        } else {
            endDate = year + "-" + (monthInt < 9 ? "0" : "") + to_string(monthInt + 1) + "-01";
        }
        
        // Month names
        string monthNames[] = {"January", "February", "March", "April", "May", "June",
                                "July", "August", "September", "October", "November", "December"};
        string monthName = monthNames[monthInt - 1];
        
        // Query to get sales data for the month
        string salesQuery = "SELECT COUNT(*) as total_transactions, "
                           "COALESCE(SUM(total_amount), 0) as total_revenue, "
                           "COALESCE(SUM(profit), 0) as total_profit "
                           "FROM Sales "
                           "WHERE sale_date >= '" + startDate + "' AND sale_date < '" + endDate + "'";
        
        if (mysql_query(db.conn, salesQuery.c_str())) {
            throw runtime_error("Failed to fetch sales data: " + string(mysql_error(db.conn)));
        }
        
        MYSQL_RES* salesRes = mysql_store_result(db.conn);
        if (!salesRes) {
            throw runtime_error("Failed to store sales result set");
        }
        
        MYSQL_ROW salesRow = mysql_fetch_row(salesRes);
        
        int totalTransactions = 0;
        double totalRevenue = 0.0;
        double totalProfit = 0.0;
        
        if (salesRow) {
            totalTransactions = salesRow[0] ? stoi(salesRow[0]) : 0;
            totalRevenue = salesRow[1] ? stod(salesRow[1]) : 0.0;
            totalProfit = salesRow[2] ? stod(salesRow[2]) : 0.0;
        }
        mysql_free_result(salesRes);
        
        // Calculate total cost from revenue and profit
        double totalCost = totalRevenue - totalProfit;
        
        // Calculate profit margin
        double profitMargin = (totalRevenue > 0) ? (totalProfit / totalRevenue) * 100 : 0.0;
        
        // Calculate average transaction value
        double avgTransactionValue = (totalTransactions > 0) ? totalRevenue / totalTransactions : 0.0;
        
        // Calculate total tax collected (6% SST from revenue)
        // Note: Since total_amount includes tax, we need to extract the tax portion
        // If total = subtotal * 1.06, then tax = total * (0.06/1.06)
        double totalTaxCollected = totalRevenue * (SST_RATE / (1 + SST_RATE));
        
        // Get total items sold
        string itemsQuery = "SELECT COUNT(*) FROM Sales WHERE sale_date >= '" + startDate + "' AND sale_date < '" + endDate + "'";
        int totalItemsSold = 0;
        
        if (mysql_query(db.conn, itemsQuery.c_str()) == 0) {
            MYSQL_RES* itemsRes = mysql_store_result(db.conn);
            if (itemsRes) {
                MYSQL_ROW itemsRow = mysql_fetch_row(itemsRes);
                if (itemsRow && itemsRow[0]) {
                    totalItemsSold = stoi(itemsRow[0]);
                }
                mysql_free_result(itemsRes);
            }
        }
        
        // Total discounts (currently 0 as not implemented)
        double totalDiscounts = 0.0;
        
        // Generate timestamp
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char dateBuffer[20];
        char timestampBuffer[30];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d", timeinfo);
        strftime(timestampBuffer, sizeof(timestampBuffer), "%d-%m-%Y %H:%M:%S", timeinfo);
        
        // Generate report ID
        string reportID = "RPT-" + string(dateBuffer) + "-001";
        
        // Default filename for PDF
        string defaultFilename = "Monthly_Sales_Report_" + year + "_" + month + ".pdf";
        string pdfFilename;
        string htmlTempFile = "reports/temp_report_" + year + "_" + month + ".html";
        
        #ifdef _WIN32
        // Show Windows Save File Dialog for PDF
        char szFile[260] = {0};
        strcpy(szFile, defaultFilename.c_str());
        
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrTitle = "Save Monthly Sales Report as PDF";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = "pdf";
        
        if (GetSaveFileNameA(&ofn) == TRUE) {
            pdfFilename = string(ofn.lpstrFile);
            // Ensure .pdf extension
            if (pdfFilename.length() < 4 || pdfFilename.substr(pdfFilename.length() - 4) != ".pdf") {
                pdfFilename += ".pdf";
            }
        } else {
            cout << COLOR_YELLOW << "\n[INFO] Save cancelled by user" << COLOR_RESET << endl;
            return;
        }
        
        // Create reports directory if it doesn't exist
        system("if not exist reports mkdir reports");
        #else
        // For non-Windows systems, use default reports folder
        system("mkdir -p reports");
        pdfFilename = "reports/Monthly_Sales_Report_" + year + "_" + month + ".pdf";
        #endif
        
        // Generate HTML to temporary file first
        ofstream report(htmlTempFile);
        if (!report.is_open()) {
            cout << COLOR_RED << "[ERROR] Could not create temporary HTML file" << COLOR_RESET << endl;
            return;
        }
        
        // Write HTML report
        report << "<!DOCTYPE html>\n";
        report << "<html>\n<head>\n";
        report << "<meta charset=\"UTF-8\">\n";
        report << "<title>Monthly Sales Report - " << monthName << " " << year << "</title>\n";
        report << "<style>\n";
        report << "body { font-family: Arial, sans-serif; margin: 40px; }\n";
        report << ".header { border: 3px solid #000; padding: 20px; text-align: center; margin-bottom: 30px; }\n";
        report << ".header h1 { margin: 5px 0; font-size: 24px; }\n";
        report << ".header h2 { margin: 5px 0; font-size: 20px; }\n";
        report << ".info-section { border: 2px solid #000; padding: 15px; margin-bottom: 30px; }\n";
        report << ".info-line { margin: 5px 0; }\n";
        report << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        report << "th { background-color: #333; color: white; padding: 12px; text-align: left; border: 1px solid #000; }\n";
        report << "td { padding: 10px; border: 1px solid #000; }\n";
        report << ".metric-value { font-weight: bold; color: #0066cc; }\n";
        report << "h3 { margin-top: 30px; border-bottom: 2px solid #333; padding-bottom: 10px; }\n";
        report << "@media print { .no-print { display: none; } }\n";
        report << "</style>\n";
        report << "</head>\n<body>\n";
        
        // Header Section
        report << "<div class=\"header\">\n";
        report << "<h1>GROCERYFLOW IMS</h1>\n";
        report << "<h2>MONTHLY SALES REPORT</h2>\n";
        report << "</div>\n";
        
        // Report Information
        report << "<div class=\"info-section\">\n";
        report << "<div class=\"info-line\"><strong>Report Period:</strong> " << monthName << " " << year << " (e.g., January 2026)</div>\n";
        report << "<div class=\"info-line\"><strong>Date Range:</strong> [01-" << month << "-" << year << "] to [";
        
        // Calculate last day of month
        int lastDay = 31;
        if (monthInt == 2) {
            lastDay = (yearInt % 4 == 0 && (yearInt % 100 != 0 || yearInt % 400 == 0)) ? 29 : 28;
        } else if (monthInt == 4 || monthInt == 6 || monthInt == 9 || monthInt == 11) {
            lastDay = 30;
        }
        
        report << (lastDay < 10 ? "0" : "") << lastDay << "-" << month << "-" << year << "]</div>\n";
        report << "<div class=\"info-line\"><strong>Generated By:</strong> " << userName << " ([Staff ID: " << userID << "])</div>\n";
        report << "<div class=\"info-line\"><strong>Generated On:</strong> [" << timestampBuffer << "]</div>\n";
        report << "<div class=\"info-line\"><strong>Report ID:</strong> [" << reportID << "]</div>\n";
        report << "</div>\n";
        
        // Executive Summary
        report << "<table>\n";
        
        report << "<tr><td><strong>Total Transactions</strong></td><td class=\"metric-value\">" 
               << totalTransactions << "</td></tr>\n";
        
        report << "<tr><td><strong>Total Revenue</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalRevenue << "</td></tr>\n";
        
        report << "<tr><td><strong>Total Cost</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalCost << "</td></tr>\n";
        
        report << "<tr><td><strong>Gross Profit</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalProfit << "</td></tr>\n";
        
        report << "<tr><td><strong>Profit Margin %</strong></td><td class=\"metric-value\">" 
               << fixed << setprecision(2) << profitMargin << "%</td></tr>\n";
        
        report << "<tr><td><strong>Average Transaction Value</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << avgTransactionValue << "</td></tr>\n";
        
        report << "<tr><td><strong>Total Items Sold</strong></td><td class=\"metric-value\">" 
               << totalItemsSold << "</td></tr>\n";
        
        report << "<tr><td><strong>Total Tax Collected</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalTaxCollected << "</td></tr>\n";
        
        report << "</table>\n";
        
        report << "</body>\n</html>";
        report.close();
        
        // Convert HTML to PDF using wkhtmltopdf or browser
        #ifdef _WIN32
        // Try wkhtmltopdf first
        string wkhtmltopdfCmd = "wkhtmltopdf --quiet --enable-local-file-access --page-size A4 --margin-top 10mm --margin-bottom 10mm --margin-left 10mm --margin-right 10mm \"" + htmlTempFile + "\" \"" + pdfFilename + "\" >nul 2>&1";
        int result = system(wkhtmltopdfCmd.c_str());
        
        if (result != 0) {
            // Fallback: Open HTML in browser with print dialog
            cout << COLOR_YELLOW << "\n[INFO] wkhtmltopdf not found. Opening in browser..." << COLOR_RESET << endl;
            cout << COLOR_YELLOW << "Please use browser's Print function and select 'Save as PDF'" << COLOR_RESET << endl;
            cout << COLOR_YELLOW << "Save to: " << pdfFilename << COLOR_RESET << endl;
            
            // Open HTML in default browser
            string openCmd = "start \"\" \"" + htmlTempFile + "\"";
            system(openCmd.c_str());
            
            cout << "\n[INFO] Temporary HTML saved. Please print to PDF manually." << endl;
            cout << "Target PDF location: " << pdfFilename << endl;
        } else {
            // Delete temporary HTML file
            remove(htmlTempFile.c_str());
            
            cout << COLOR_GREEN << "\n[SUCCESS] Monthly Sales Report Generated as PDF!" << COLOR_RESET << endl;
            cout << "PDF saved to: " << pdfFilename << endl;
        }
        #else
        // For Linux/Mac, try wkhtmltopdf
        string wkhtmltopdfCmd = "wkhtmltopdf --quiet --enable-local-file-access --page-size A4 " + htmlTempFile + " " + pdfFilename + " 2>/dev/null";
        int result = system(wkhtmltopdfCmd.c_str());
        
        if (result == 0) {
            remove(htmlTempFile.c_str());
            cout << COLOR_GREEN << "\n[SUCCESS] Monthly Sales Report Generated as PDF!" << COLOR_RESET << endl;
            cout << "PDF saved to: " << pdfFilename << endl;
        } else {
            cout << COLOR_YELLOW << "\n[INFO] wkhtmltopdf not found. HTML file saved instead." << COLOR_RESET << endl;
            cout << "File: " << htmlTempFile << endl;
        }
        #endif
        cout << "\nExecutive Summary:" << endl;
        cout << "==================" << endl;
        cout << "Total Transactions: " << totalTransactions << endl;
        cout << "Total Revenue: RM " << fixed << setprecision(2) << totalRevenue << endl;
        cout << "Total Cost: RM " << totalCost << endl;
        cout << "Gross Profit: RM " << totalProfit << endl;
        cout << "Profit Margin: " << profitMargin << "%" << endl;
        cout << "Average Transaction: RM " << avgTransactionValue << endl;
        cout << "Total Items Sold: " << totalItemsSold << endl;
        cout << "Total Tax Collected: RM " << totalTaxCollected << endl;
        cout << "Total Discounts: RM " << totalDiscounts << endl;
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "\n[ERROR] " << e.what() << COLOR_RESET << endl;
    }
}

// Generate custom date range sales report
void SalesManager::generateRangeSalesReport(Database& db, int userID, const string& startDate, const string& endDate) {
    try {
        if (!db.conn) {
            throw runtime_error("Database connection is not available");
        }

        int sy = 0, sm = 0, sd = 0, ey = 0, em = 0, ed = 0;
        if (!parseDate(startDate, sy, sm, sd) || !parseDate(endDate, ey, em, ed)) {
            throw runtime_error("Invalid date range provided");
        }

        tm st{};
        st.tm_year = sy - 1900;
        st.tm_mon = sm - 1;
        st.tm_mday = sd;
        st.tm_isdst = -1;

        tm et{};
        et.tm_year = ey - 1900;
        et.tm_mon = em - 1;
        et.tm_mday = ed;
        et.tm_isdst = -1;

        time_t startTs = mktime(&st);
        time_t endTs = mktime(&et);

        if (startTs == -1 || endTs == -1 || difftime(endTs, startTs) < 0) {
            throw runtime_error("Start date must not be after end date");
        }

        int rangeDays = static_cast<int>(difftime(endTs, startTs) / (60 * 60 * 24)) + 1;

        string endDateExclusive = addOneDay(endDate);
        if (endDateExclusive.empty()) {
            throw runtime_error("Failed to process end date range");
        }

        // Get user name for report header
        string userName = "Staff";
        string getUserQuery = "SELECT username FROM User WHERE user_id = " + to_string(userID);
        if (mysql_query(db.conn, getUserQuery.c_str()) == 0) {
            MYSQL_RES* userRes = mysql_store_result(db.conn);
            if (userRes) {
                MYSQL_ROW userRow = mysql_fetch_row(userRes);
                if (userRow && userRow[0]) {
                    userName = userRow[0];
                }
                mysql_free_result(userRes);
            }
        }

        // Query to get sales data for the custom range
        string salesQuery = "SELECT COUNT(*) as total_transactions, "
                           "COALESCE(SUM(total_amount), 0) as total_revenue, "
                           "COALESCE(SUM(profit), 0) as total_profit "
                           "FROM Sales "
                           "WHERE sale_date >= '" + startDate + "' AND sale_date < '" + endDateExclusive + "'";

        if (mysql_query(db.conn, salesQuery.c_str())) {
            throw runtime_error("Failed to fetch sales data: " + string(mysql_error(db.conn)));
        }

        MYSQL_RES* salesRes = mysql_store_result(db.conn);
        if (!salesRes) {
            throw runtime_error("Failed to store sales result set");
        }

        MYSQL_ROW salesRow = mysql_fetch_row(salesRes);

        int totalTransactions = 0;
        double totalRevenue = 0.0;
        double totalProfit = 0.0;

        if (salesRow) {
            totalTransactions = salesRow[0] ? stoi(salesRow[0]) : 0;
            totalRevenue = salesRow[1] ? stod(salesRow[1]) : 0.0;
            totalProfit = salesRow[2] ? stod(salesRow[2]) : 0.0;
        }
        mysql_free_result(salesRes);

        // Calculate total cost from revenue and profit
        double totalCost = totalRevenue - totalProfit;

        // Calculate profit margin
        double profitMargin = (totalRevenue > 0) ? (totalProfit / totalRevenue) * 100 : 0.0;

        // Calculate average transaction value
        double avgTransactionValue = (totalTransactions > 0) ? totalRevenue / totalTransactions : 0.0;

        // Calculate total tax collected (6% SST from revenue)
        double totalTaxCollected = totalRevenue * (SST_RATE / (1 + SST_RATE));

        // Get total items sold
        string itemsQuery = "SELECT COUNT(*) FROM Sales WHERE sale_date >= '" + startDate + "' AND sale_date < '" + endDateExclusive + "'";
        int totalItemsSold = 0;

        if (mysql_query(db.conn, itemsQuery.c_str()) == 0) {
            MYSQL_RES* itemsRes = mysql_store_result(db.conn);
            if (itemsRes) {
                MYSQL_ROW itemsRow = mysql_fetch_row(itemsRes);
                if (itemsRow && itemsRow[0]) {
                    totalItemsSold = stoi(itemsRow[0]);
                }
                mysql_free_result(itemsRes);
            }
        }

        double totalDiscounts = 0.0;

        // Generate timestamp
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char dateBuffer[20];
        char timestampBuffer[30];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d", timeinfo);
        strftime(timestampBuffer, sizeof(timestampBuffer), "%d-%m-%Y %H:%M:%S", timeinfo);

        // Generate report ID
        string reportID = "RPT-" + string(dateBuffer) + "-RANGE";

        string rangeToken = startDate + "_to_" + endDate;
        string defaultFilename = "Sales_Report_" + rangeToken + ".pdf";
        string pdfFilename;
        string htmlTempFile = "reports/temp_report_" + rangeToken + ".html";

        #ifdef _WIN32
        // Show Windows Save File Dialog for PDF
        char szFile[260] = {0};
        strcpy(szFile, defaultFilename.c_str());

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrTitle = "Save Sales Report (Date Range) as PDF";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = "pdf";

        if (GetSaveFileNameA(&ofn) == TRUE) {
            pdfFilename = string(ofn.lpstrFile);
            if (pdfFilename.length() < 4 || pdfFilename.substr(pdfFilename.length() - 4) != ".pdf") {
                pdfFilename += ".pdf";
            }
        } else {
            cout << COLOR_YELLOW << "\n[INFO] Save cancelled by user" << COLOR_RESET << endl;
            return;
        }

        system("if not exist reports mkdir reports");
        #else
        system("mkdir -p reports");
        pdfFilename = "reports/Sales_Report_" + rangeToken + ".pdf";
        #endif

        ofstream report(htmlTempFile);
        if (!report.is_open()) {
            cout << COLOR_RED << "[ERROR] Could not create temporary HTML file" << COLOR_RESET << endl;
            return;
        }

        // Write HTML report
        report << "<!DOCTYPE html>\n";
        report << "<html>\n<head>\n";
        report << "<meta charset=\"UTF-8\">\n";
        report << "<title>Sales Report - " << startDate << " to " << endDate << "</title>\n";
        report << "<style>\n";
        report << "body { font-family: Arial, sans-serif; margin: 40px; }\n";
        report << ".header { border: 3px solid #000; padding: 20px; text-align: center; margin-bottom: 30px; }\n";
        report << ".header h1 { margin: 5px 0; font-size: 24px; }\n";
        report << ".header h2 { margin: 5px 0; font-size: 20px; }\n";
        report << ".info-section { border: 2px solid #000; padding: 15px; margin-bottom: 30px; }\n";
        report << ".info-line { margin: 5px 0; }\n";
        report << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        report << "th { background-color: #333; color: white; padding: 12px; text-align: left; border: 1px solid #000; }\n";
        report << "td { padding: 10px; border: 1px solid #000; }\n";
        report << ".metric-value { font-weight: bold; color: #0066cc; }\n";
        report << "h3 { margin-top: 30px; border-bottom: 2px solid #333; padding-bottom: 10px; }\n";
        report << "@media print { .no-print { display: none; } }\n";
        report << "</style>\n";
        report << "</head>\n<body>\n";

        // Header Section
        report << "<div class=\"header\">\n";
        report << "<h1>GROCERYFLOW IMS</h1>\n";
        report << "<h2>SALES REPORT (DATE RANGE)</h2>\n";
        report << "</div>\n";

        // Report Information
        report << "<div class=\"info-section\">\n";
        report << "<div class=\"info-line\"><strong>Report Period:</strong> " << startDate << " to " << endDate << "</div>\n";
        report << "<div class=\"info-line\"><strong>Date Range:</strong> [" << startDate << "] to [" << endDate << "]</div>\n";
        report << "<div class=\"info-line\"><strong>Range Length:</strong> " << rangeDays << " day(s)</div>\n";
        report << "<div class=\"info-line\"><strong>Generated By:</strong> " << userName << " ([Staff ID: " << userID << "])</div>\n";
        report << "<div class=\"info-line\"><strong>Generated On:</strong> [" << timestampBuffer << "]</div>\n";
        report << "<div class=\"info-line\"><strong>Report ID:</strong> [" << reportID << "]</div>\n";
        report << "</div>\n";

        // Executive Summary
        report << "<table>\n";

        report << "<tr><td><strong>Total Transactions</strong></td><td class=\"metric-value\">" 
               << totalTransactions << "</td></tr>\n";

        report << "<tr><td><strong>Total Revenue</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalRevenue << "</td></tr>\n";

        report << "<tr><td><strong>Total Cost</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalCost << "</td></tr>\n";

        report << "<tr><td><strong>Gross Profit</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalProfit << "</td></tr>\n";

        report << "<tr><td><strong>Profit Margin %</strong></td><td class=\"metric-value\">" 
               << fixed << setprecision(2) << profitMargin << "%</td></tr>\n";

        report << "<tr><td><strong>Average Transaction Value</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << avgTransactionValue << "</td></tr>\n";

        report << "<tr><td><strong>Total Items Sold</strong></td><td class=\"metric-value\">" 
               << totalItemsSold << "</td></tr>\n";

        report << "<tr><td><strong>Total Tax Collected</strong></td><td class=\"metric-value\">RM " 
               << fixed << setprecision(2) << totalTaxCollected << "</td></tr>\n";

        report << "</table>\n";

        report << "</body>\n</html>";
        report.close();

        #ifdef _WIN32
        string wkhtmltopdfCmd = "wkhtmltopdf --quiet --enable-local-file-access --page-size A4 --margin-top 10mm --margin-bottom 10mm --margin-left 10mm --margin-right 10mm \"" + htmlTempFile + "\" \"" + pdfFilename + "\" >nul 2>&1";
        int result = system(wkhtmltopdfCmd.c_str());

        if (result != 0) {
            cout << COLOR_YELLOW << "\n[INFO] wkhtmltopdf not found. Opening in browser..." << COLOR_RESET << endl;
            cout << COLOR_YELLOW << "Please use browser's Print function and select 'Save as PDF'" << COLOR_RESET << endl;
            cout << COLOR_YELLOW << "Save to: " << pdfFilename << COLOR_RESET << endl;

            string openCmd = "start \"\" \"" + htmlTempFile + "\"";
            system(openCmd.c_str());

            cout << "\n[INFO] Temporary HTML saved. Please print to PDF manually." << endl;
            cout << "Target PDF location: " << pdfFilename << endl;
        } else {
            remove(htmlTempFile.c_str());

            cout << COLOR_GREEN << "\n[SUCCESS] Range Sales Report Generated as PDF!" << COLOR_RESET << endl;
            cout << "PDF saved to: " << pdfFilename << endl;
        }
        #else
        string wkhtmltopdfCmd = "wkhtmltopdf --quiet --enable-local-file-access --page-size A4 " + htmlTempFile + " " + pdfFilename + " 2>/dev/null";
        int result = system(wkhtmltopdfCmd.c_str());

        if (result == 0) {
            remove(htmlTempFile.c_str());
            cout << COLOR_GREEN << "\n[SUCCESS] Range Sales Report Generated as PDF!" << COLOR_RESET << endl;
            cout << "PDF saved to: " << pdfFilename << endl;
        } else {
            cout << COLOR_YELLOW << "\n[INFO] wkhtmltopdf not found. HTML file saved instead." << COLOR_RESET << endl;
            cout << "File: " << htmlTempFile << endl;
        }
        #endif

        cout << "\nExecutive Summary:" << endl;
        cout << "==================" << endl;
        cout << "Total Transactions: " << totalTransactions << endl;
        cout << "Total Revenue: RM " << fixed << setprecision(2) << totalRevenue << endl;
        cout << "Total Cost: RM " << totalCost << endl;
        cout << "Gross Profit: RM " << totalProfit << endl;
        cout << "Profit Margin: " << profitMargin << "%" << endl;
        cout << "Average Transaction: RM " << avgTransactionValue << endl;
        cout << "Total Items Sold: " << totalItemsSold << endl;
        cout << "Total Tax Collected: RM " << totalTaxCollected << endl;
        cout << "Total Discounts: RM " << totalDiscounts << endl;

    } catch (const exception& e) {
        cerr << COLOR_RED << "\n[ERROR] " << e.what() << COLOR_RESET << endl;
    }
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
            
            scanBarcode(db, barcode);
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