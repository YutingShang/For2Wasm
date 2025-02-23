include definitions.mk

all : 
	$(MAKE) -f generate_antlr.mk 
	$(MAKE) -f build_objects.mk 
	$(MAKE) -f run_tests.mk

# Target that doesn't run tests
install:
	$(MAKE) -f generate_antlr.mk 
	$(MAKE) -f build_objects.mk 

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

# Clean rule for tests
cleantest:
	$(MAKE) -f run_tests.mk cleantest

# Phony targets
.PHONY: all clean install cleantest

