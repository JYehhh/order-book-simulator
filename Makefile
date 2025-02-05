# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 $(shell pkg-config --cflags boost)
LDFLAGS = $(shell pkg-config --libs boost)
SRC_DIR = src
INCLUDE_DIR = include
TARGET_SERVER = server
TARGET_CLIENT = client

# Default rule
all: $(TARGET_SERVER) $(TARGET_CLIENT)

# Rule to compile server
$(TARGET_SERVER): $(SRC_DIR)/server.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $< -o $@ $(LDFLAGS)

# Rule to compile client
$(TARGET_CLIENT): $(SRC_DIR)/client.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $< -o $@ $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)

# Phony targets
.PHONY: all clean