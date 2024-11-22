# Compiler and flags - EDIT: change runtime headers and library directories
CXX = g++
CXXFLAGS = -std=c++17 -I/usr/local/lib/antlr4-runtime -I./include
LDFLAGS = -L/usr/local/lib/lib -lantlr4-runtime

# Source files and directories
SRCS = main.cpp Fortran90Lexer.cpp Fortran90Parser.cpp Fortran90LexerBase.cpp Fortran90ParserBaseVisitor.cpp Fortran90ParserCustomVisitor.cpp
BUILD_DIR = build
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))
TARGET = main

# Default target
all: $(TARGET)

# Linking rule
# each rule is made up of target: prerequisites
# $@ is the target, $^ is all prerequisites
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Rule to create the build directory for object files
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compilation rule for object files
# $< is the first prerequisite, $@ is the target
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
