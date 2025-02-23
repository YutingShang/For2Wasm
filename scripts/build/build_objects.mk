# #---------
# # Definitions
# #---------

include definitions.mk

# Target executable
TARGET = $(BUILD_DIR)/bin/main
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(OBJ_DIR)/main.o

SRCS += $(MAIN_SRC)
OBJS += $(MAIN_OBJ)

# Get the directories of the source files
# Add source directories to vpath to help Make locate the source files
SRCS_DIRS = $(sort $(dir $(SRCS)))   
vpath %.cpp $(SRCS_DIRS)

#EDIT: src and include directories in definitions.mk

#---------
# Rules
#---------

# Default target
all: $(TARGET)

# Linking object files
$(TARGET): $(OBJS)| $(BUILD_DIR)/bin 
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

