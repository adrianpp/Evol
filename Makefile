#configuration for inputs/outputs
EXEC_NAME=evol
SRCS=main.cpp

#where to store intermediates and our header directory
BUILD_DIR=build
INCLUDE_DIR=include

#compiler flags, makefile variables
CPPFLAGS+=-std=c++11 -g -I $(INCLUDE_DIR)
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

#some basic rules
all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJS)
	$(CXX) -o $@ $^

#if we need to rebuild the intermediate directory
$(BUILD_DIR):
	mkdir -p $@

#include dependencies, but dont fail if we cant find them
-include $(OBJS:%.o=%.d)

#order-only prereq of the build dir, to force it being made before our objects
#note also that by passing -MMD in our build flags, dependency file will be made as well
$(OBJS): $(BUILD_DIR)/%.o : %.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

#clean the intermediate files and final exec
clean:
	$(RM) -rf $(BUILD_DIR) $(EXEC_NAME)

.PHONY: all clean
