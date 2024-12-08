# #---------
# # Definitions
# #---------

include definitions.mk
ANTLR = antlr4
ANTLR_FLAGS = -Dlanguage=Cpp -visitor -no-listener

GRAMMAR_DIR = $(THIRD_PARTY_DIR)/fortran/grammar
GRAMMAR_SRCS = $(GRAMMAR_DIR)/Fortran90Lexer.g4 \
				$(GRAMMAR_DIR)/Fortran90Parser.g4

# #---------
# # Rules
# #---------

# Generate the lexer and parser files by calling antlr4 to generate the files which outputs to the grammar directory automatically
# Then move the files to the generated antlr directory and remove the .tokens and .interp files
$(GENERATED_SRCS): $(GRAMMAR_SRCS) | $(GENERATED_ANTLR_DIR)
	mkdir -p $(GENERATED_ANTLR_DIR) \
	&& $(ANTLR) $(ANTLR_FLAGS) $(GRAMMAR_SRCS) \
	&& mv $(GRAMMAR_DIR)/*.cpp $(GRAMMAR_DIR)/*.h $(GENERATED_ANTLR_DIR) \
	&& rm $(GRAMMAR_DIR)/*.tokens $(GRAMMAR_DIR)/*.interp 

# Ensure the generated antlr files exist
$(GENERATED_ANTLR_DIR):
	mkdir -p $(GENERATED_ANTLR_DIR)