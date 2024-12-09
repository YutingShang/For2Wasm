# #---------
# # Definitions
# #---------

include definitions.mk

# Define the directories
OBJ_DIR = $(BUILD_DIR)/obj

# EDIT: change the antlr4-runtime and lib path to the one on your system
CXX = g++
INCLUDE_DIRS = /usr/local/lib/antlr4-runtime ./include/visitor ./include/irTree $(GENERATED_ANTLR_DIR) $(THIRD_PARTY_DIR)/fortran
CXXFLAGS = -std=c++17 $(foreach dir, $(INCLUDE_DIRS), -I$(dir))
LDFLAGS = -L/usr/local/lib/lib -lantlr4-runtime

# Target executable
TARGET = $(BUILD_DIR)/bin/main

# Source files and directories - EDIT: add the new source files here, also add the new include directories to INCLUDE_DIRS
SRCS = src/main.cpp \
	$(THIRD_PARTY_DIR)/fortran/Fortran90LexerBase.cpp \
	$(GENERATED_SRCS) \
	src/visitor/Fortran90ParserCustomVisitor.cpp \
	src/visitor/Fortran90ParserASTVisitor.cpp \
	src/visitor/Fortran90ParserTranslatorVisitor.cpp \
	src/visitor/Fortran90ContextCopy.cpp \
	src/visitor/Fortran90ParserIRTreeVisitor.cpp \
	src/irTree/BaseNode.cpp \
	src/irTree/SimpleNode.cpp \
	src/irTree/LoopNode.cpp \
	src/irTree/IfNode.cpp \
	src/irTree/IRTreeTools.cpp

# Get the directories of the source files
SRC_DIRS = $(sort $(dir $(SRCS)))   

# Add source directories to vpath to help Make locate the source files
vpath %.cpp $(SRC_DIRS)

# List of object files, regardless of source file directory
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

#---------
# Rules
#---------

# Default target
all: $(TARGET)

# Linking object files
$(TARGET): $(OBJS) | $(BUILD_DIR)/bin 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Ensure the bin directory exists
$(BUILD_DIR)/bin: 
	mkdir -p $(BUILD_DIR)/bin


# Compilation rule for object files
# $< is the first prerequisite, $@ is the target
$(OBJ_DIR)/%.o: %.cpp $(SRCS)| $(OBJ_DIR) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure the object directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)


# Phony targets
.PHONY: all 

