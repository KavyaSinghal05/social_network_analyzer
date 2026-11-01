# =============================================================================
# Social Network Graph Analyzer — Makefile
# DSA Project | B.Tech V Semester | JIIT Noida
#
# Builds a CSR-based social network analyzer (BFS, DFS, Bidirectional BFS,
# Union-Find components, PageRank, degree centrality, friend suggestions).
#
# Compiler: g++ (MinGW on Windows / GCC on Linux)
# Standard: C++11
# =============================================================================

CXX      ?= g++
CXXFLAGS := -std=c++11 -O2 -Wall -Wextra
LDFLAGS  :=

SRC_DIR  := src
BUILD_DIR:= build
TARGET   := social_network

SRCS     := $(SRC_DIR)/graph.cpp $(SRC_DIR)/main.cpp
OBJS     := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
.PHONY: all clean run rebuild
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@echo ""
	@echo "Build complete: ./$(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program (expects data/ folder alongside the executable)
run: $(TARGET)
	./$(TARGET)

# Full clean rebuild
rebuild: clean all

# Remove build artifacts
clean:
	@rm -rf $(BUILD_DIR) $(TARGET) $(TARGET).exe

# =============================================================================
# End of Makefile
# =============================================================================
