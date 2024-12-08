BUILD_DIR = ./build

THIRD_PARTY_DIR = ./third-party
GENERATED_ANTLR_DIR = $(BUILD_DIR)/generated/antlr

GENERATED_SRCS = $(GENERATED_ANTLR_DIR)/Fortran90Lexer.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90Parser.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserBaseVisitor.cpp \
					$(GENERATED_ANTLR_DIR)/Fortran90ParserVisitor.cpp
