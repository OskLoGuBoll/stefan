SRCS := $(wildcard *.cc)
CXXFlAGS := std=c++17 -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast -fmax-errors=3 -g
COMMON_DIR = ./common
BUILD_DIR := ./build
EXEC := stefan

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
INCL := -I$(COMMON_DIR)/ -I($COMMON_DIR)/Linux -DGL_GLEXT_PROTOTYPES -lX11 -lGL -lm

$(BUILD_DIR)/%.cc.o: %.cc
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCL) $^ -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
