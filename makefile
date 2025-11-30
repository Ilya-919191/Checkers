CXX = g++
CXXFLAGS = -std=c++20 -Wall -Iinclude

SRC_DIR = src
BUILD_DIR = build
DEBUG_DIR = build/debug

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
DEBUG_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(DEBUG_DIR)/%.o,$(SRC))

TARGET = $(BUILD_DIR)/main.out
DEBUG_TARGET = $(DEBUG_DIR)/main_debug.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------- DEBUG MODE --------

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEBUG_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ----------------------------

clean:
	rm -rf $(BUILD_DIR)
