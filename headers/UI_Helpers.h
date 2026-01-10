#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

namespace UI {

// ANSI color codes (Win10+ supported when VT mode enabled)
static const std::string RESET  = "\x1b[0m";
static const std::string BOLD   = "\x1b[1m";
static const std::string CYAN   = "\x1b[36m";
static const std::string BLUE   = "\x1b[34m";
static const std::string GREEN  = "\x1b[92m";   // Bright green
static const std::string RED    = "\x1b[31m";
static const std::string YELLOW = "\x1b[33m";
static const std::string GREY   = "\x1b[90m";
static const std::string WHITE  = "\x1b[97m";

inline void enableAnsi() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    mode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, mode);
#endif
}

inline int consoleWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        int w = info.srWindow.Right - info.srWindow.Left + 1;
        if (w > 0) return w;
    }
#endif
    return 100; // Fallback
}

inline void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

inline std::string centered(const std::string& text, int width) {
    if ((int)text.size() >= width) return text;
    int padding = (width - static_cast<int>(text.size())) / 2;
    return std::string(padding, ' ') + text + std::string(width - padding - (int)text.size(), ' ');
}

inline void divider(int width, char ch = '=', const std::string& color = GREY) {
    std::cout << color << std::string(width, ch) << RESET << std::endl;
}

inline void printSuccess(const std::string& msg) {
    std::cout << GREEN << BOLD << "[SUCCESS] " << RESET << GREEN << msg << RESET << std::endl;
}

inline void printError(const std::string& msg) {
    std::cout << RED << BOLD << "[ERROR] " << RESET << RED << msg << RESET << std::endl;
}

inline void printWarning(const std::string& msg) {
    std::cout << YELLOW << BOLD << "[WARNING] " << RESET << YELLOW << msg << RESET << std::endl;
}

inline void pause() {
    std::cout << GREY << "\nPress any key to continue..." << RESET;
    std::cin.get();
}

inline void bannerGrocerySystem() {
    std::cout << CYAN << BOLD;
    std::cout << std::string(100, '=') << "\n";
    
    std::cout << R"(
   ____ ____   ___   ____ _____ ______   ______ _     _____        __  ___ __  __ ____  
  / ___|  _ \ / _ \ / ___| ____|  _ \ \ / /  _/| |   / _ \ \      / / |_ _|  \/  / ___| 
 | |  _| |_) | | | | |   |  _| | |_) \ V /| |_ | |  | | | \ \ /\ / /   | || |\/| \___ \ 
 | |_| |  _ <| |_| | |___| |___|  _ < | | |  _|| |__| |_| |\ V  V /    | || |  | |___) |
  \____|_| \_\\___/ \___ |_____|_| \_\|_| |_|  |_____\___/  \_/\_/    |___|_|  |_|____/  
   )" << "\n";
    
    std::cout << centered("Professional Inventory Management System", 100) << "\n";
    
    std::cout << std::string(100, '=') << "\n";
    std::cout << RESET;
}

inline void drawBox(const std::string& title, const std::vector<std::string>& lines,
                    int width = 0, const std::string& borderColor = GREY,
                    const std::string& headerColor = CYAN) {
    int w = (width > 0 ? width : std::min(consoleWidth() - 4, 100));
    std::cout << borderColor << "+" << std::string(w - 2, '=') << "+" << RESET << std::endl;
    std::cout << borderColor << "|" << RESET << headerColor << BOLD
              << centered(title, w - 2) << RESET
              << borderColor << "|" << RESET << std::endl;
    std::cout << borderColor << "|" << std::string(w - 2, ' ') << "|" << RESET << std::endl;
    for (const auto& ln : lines) {
        std::cout << borderColor << "| " << RESET
                  << std::left << std::setw(w - 4) << ln
                  << borderColor << " |" << RESET << std::endl;
    }
    std::cout << borderColor << "+" << std::string(w - 2, '-') << "+" << RESET << std::endl;
}

inline void drawMenu(const std::string& title, const std::vector<std::string>& items, int width = 0) {
    std::vector<std::string> lines = items;
    drawBox(title, lines, width);
}

// Fit text to a fixed column width with ellipsis
inline std::string fit(const std::string& s, int width) {
    if (width <= 3) return s.substr(0, std::min<int>(3, (int)s.size()));
    if ((int)s.size() > width) return s.substr(0, width - 3) + "...";
    return s;
}

// Print a styled table header
inline void printTableHeader(const std::vector<std::pair<std::string,int>>& cols, const std::string& title = "") {
    int total = 1; // left border
    for (auto& c : cols) total += c.second + 3; // " | " + content
    total += 1; // right border
    divider(total, '=');
    if (!title.empty()) {
        std::cout << CYAN << BOLD << centered(title, total) << RESET << std::endl;
    }
    divider(total, '=');
    std::cout << GREY << "|" << RESET;
    for (size_t i = 0; i < cols.size(); ++i) {
        std::cout << " " << WHITE << BOLD << std::left << std::setw(cols[i].second) << cols[i].first << RESET
                  << GREY << " |" << RESET;
    }
    std::cout << std::endl;
    divider(total, '-');
}

// Print one table row aligned to column widths
inline void printTableRow(const std::vector<std::string>& cells, const std::vector<std::pair<std::string,int>>& cols) {
    std::cout << GREY << "|" << RESET;
    for (size_t i = 0; i < cols.size(); ++i) {
        std::string val = (i < cells.size() ? fit(cells[i], cols[i].second) : "");
        std::cout << " " << std::left << std::setw(cols[i].second) << val << GREY << " |" << RESET;
    }
    std::cout << std::endl;
}

inline void printTableFooter(int count, int totalWidth) {
    divider(totalWidth, '=');
    std::cout << GREY << "Showing " << count << (count == 1 ? " entry" : " entries") << RESET << std::endl;
}

inline void loadingStartup(const std::string& label = "Starting Grocery System...") {
    int w = std::min(consoleWidth() - 4, 80);
    clear();
    bannerGrocerySystem();
    std::cout << WHITE << centered(label, w) << RESET << std::endl;
    divider(w, '-');
    const int barWidth = std::max(20, w - 20);
    for (int p = 0; p <= 100; p += 5) {
        int filled = (p * barWidth) / 100;
        std::cout << "[" << GREEN << std::string(filled, '#') << RESET
                  << GREY << std::string(barWidth - filled, '.') << RESET << "] "
                  << std::setw(3) << p << "%\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(40));
    }
    std::cout << std::endl;
}

} // namespace UI
