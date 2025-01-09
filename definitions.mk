BUILD_DIR = ./build
SRC_DIR = ./src
THIRD_PARTY_DIR = ./third-party
GENERATED_DIR = $(BUILD_DIR)/generated
GENERATED_ANTLR_DIR = $(GENERATED_DIR)/antlr

GENERATED_SRCS = $(GENERATED_ANTLR_DIR)/Fortran90Lexer.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90Parser.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserBaseVisitor.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserVisitor.cpp
