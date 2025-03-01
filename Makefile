# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 $(shell pkg-config --cflags boost)
LDFLAGS = $(shell pkg-config --libs boost)

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

TARGET_SERVER = server
TARGET_CLIENT = client

# Define object files in correct order
SERVER_OBJS = $(BUILD_DIR)/FilledTrade.o $(BUILD_DIR)/Order.o $(BUILD_DIR)/OrderIdGenerator.o $(BUILD_DIR)/OrderBook.o $(BUILD_DIR)/Adapter.o $(BUILD_DIR)/Server.o
CLIENT_OBJS = $(BUILD_DIR)/FilledTrade.o $(BUILD_DIR)/Order.o $(BUILD_DIR)/OrderIdGenerator.o $(BUILD_DIR)/OrderBook.o $(BUILD_DIR)/Adapter.o $(BUILD_DIR)/Client.o

# Default rule
all: $(TARGET_SERVER) $(TARGET_CLIENT)

# Rule to compile server
$(TARGET_SERVER): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

# Rule to compile client
$(TARGET_CLIENT): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

# Compile each .cpp file to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)

# Phony targets
.PHONY: all clean