BUILD_DIR = ../../build
SRC_DIR = ../../src
OBJ_DIR = $(BUILD_DIR)/obj
THIRD_PARTY_DIR = ../../third-party
GENERATED_DIR = $(BUILD_DIR)/generated
GENERATED_ANTLR_DIR = $(GENERATED_DIR)/antlr

GENERATED_SRCS = $(GENERATED_ANTLR_DIR)/Fortran90Lexer.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90Parser.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserBaseVisitor.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserVisitor.cpp

# EDIT: change the antlr4-runtime and lib path to the one on your system in LDFLAGS and INCLUDE_DIRS
# EDIT: add the new include directories to INCLUDE_DIRS
CXX = g++
INCLUDE_DIRS = /usr/local/lib/antlr4-runtime ../../include/visitor ../../include/irTree ../../include/utils ../../include/flowgraph $(GENERATED_ANTLR_DIR) $(THIRD_PARTY_DIR)/fortran
CXXFLAGS += -std=c++17 $(foreach dir, $(INCLUDE_DIRS), -I$(dir)) 
LDFLAGS += -L/usr/local/lib/lib -lantlr4-runtime

# Source files and directories - EDIT: add the new source files here, also add the new include directories to INCLUDE_DIRS
# Does not include main.cpp or the test .cpp files
SRCS = $(THIRD_PARTY_DIR)/fortran/Fortran90LexerBase.cpp \
	$(GENERATED_SRCS) \
	$(wildcard $(SRC_DIR)/irTree/*.cpp) \
	$(wildcard $(SRC_DIR)/visitor/*.cpp) \
	$(wildcard $(SRC_DIR)/utils/*.cpp) \
	$(wildcard $(SRC_DIR)/flowgraph/*.cpp)

# Get the directories of the source files
SRCS_DIRS = $(sort $(dir $(SRCS)))   

# Add source directories to vpath to help Make locate the source files
# NOTE: if I change the SRCS variable in a makefile, I need to change copy this line too into the makefile
vpath %.cpp $(SRCS_DIRS)

# List of object files, regardless of source file directory
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))