CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
INCLUDES = -I"C:/Program Files/MySQL/MySQL Server 9.5/include"
LDFLAGS = -L"C:/Program Files/MySQL/MySQL Server 9.5/lib"
LIBS = -lmysql

SRCS = main.cpp Database.cpp User.cpp Product.cpp Sales.cpp Supplier.cpp Dashboard.cpp PurchaseOrder.cpp Receiving.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main.exe

all: compile

compile: $(OBJS)
	@echo Objects compiled successfully
	@echo Note: Linking requires 64-bit MinGW due to MySQL library format
	@echo To complete linking, use one of:
	@echo 1. Install 64-bit MinGW-w64
	@echo 2. Use MSVC compiler
	@echo 3. Install MinGW-w64 via: winget install BrechtSanders.WinLibs.POSIX.UCRT

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

link: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(OBJS) main.exe

.PHONY: all compile link clean
