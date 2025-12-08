cmake_minimum_required(VERSION 3.10)
project(SmartGrocery)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MySQL
find_package(MySQL REQUIRED)

# Add executable
add_executable(main 
    main.cpp
    Database.cpp
    User.cpp
    Product.cpp
    Sales.cpp
)

# Link libraries
target_include_directories(main PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${MYSQL_LIBRARIES})

# Compiler flags
if(MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()
