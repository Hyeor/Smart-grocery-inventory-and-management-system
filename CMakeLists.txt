cmake_minimum_required(VERSION 3.20)
project(GroceryFlow_IMS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add vcpkg toolchain if it exists
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
elseif(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
  set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Try to find FTXUI
find_package(ftxui CONFIG REQUIRED)

# Build Demo UI (works without FTXUI)
add_executable(demo_ui source/demo_ui.cpp)
target_include_directories(demo_ui PRIVATE ${CMAKE_SOURCE_DIR}/headers)

# Build the main FTXUI application
message(STATUS "FTXUI found - building main_ui")
add_executable(main_ui source/main_ui_ftxui.cpp)
target_include_directories(main_ui PRIVATE ${CMAKE_SOURCE_DIR}/headers)
target_link_libraries(main_ui
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
)

# Optional: Original C++ backend (GroceryFlow IMS)
# Uncomment to build the existing system:
# add_executable(groceryflow
#     source/main.cpp
#     source/Database.cpp
#     source/Product.cpp
#     source/User.cpp
#     source/PurchaseOrder.cpp
#     source/Receiving.cpp
#     source/Sales.cpp
#     source/Supplier.cpp
#     source/Dashboard.cpp
# )
# target_include_directories(groceryflow PRIVATE ${CMAKE_SOURCE_DIR}/headers)
# target_link_libraries(groceryflow PRIVATE mysqlclient)
