include definitions.mk

all : 
	$(MAKE) -f generate_antlr.mk 
	$(MAKE) -f build_objects.mk 

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean

