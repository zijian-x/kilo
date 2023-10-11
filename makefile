NAME := kilo

SRC_DIR := src
BIN_DIR := bin
TEST_DIR := test
MAIN := $(BIN_DIR)/$(NAME)
MAIN_TEST := $(BIN_DIR)/test

CXX := clang++
LANG := -x c++
STD := -std=c++20
WARNINGS := -Wall -Wextra -Wpedantic -Weffc++ -Wconversion -Wshadow \
	    -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual \
	    -Wnull-dereference -Wno-unused-parameter -Wno-unused-function \
	    -Wno-unused-variable
OPTM := -O0
CXXFLAGS := $(LANG) $(STD) $(WARNINGS) $(OPTM)

LD := clang++
LIB := -lfmt
SAN := -fsanitize=address,undefined
LDFLAGS := $(SAN) $(LIB)

TEST_ARGUMENTS := --gtest_filter=*

src := $(shell find $(SRC_DIR) -type f -name "*.cpp")
obj := $(src:.cpp=.o)

test_header := $(shell grep -roh '\.\./src/.*\.hpp' test | sort | uniq | sed 's;\.\./;;')
test_src := $(shell find $(TEST_DIR) -type f -name "*.cpp") \
	    $(test_header:.hpp=.cpp)
test_obj := $(test_src:.cpp=.o)

.PHONY: all run init debug compile test clean clean_test fclean leak generate_cc

all: compile

run: compile
	./$(MAIN)

runf: compile
	./$(MAIN) file

debug: CXXFLAGS += -g

debug: fclean compile

test: LDFLAGS += -lgtest

test: CXXFLAGS += -g

test: $(MAIN_TEST)
	./$(MAIN_TEST) $(TEST_ARGUMENTS)

compile: $(MAIN)

$(MAIN): $(obj)
	@test -d bin || mkdir bin
	$(LD) $(obj) $(LDFLAGS) -o $(MAIN)

$(MAIN_TEST): $(test_obj)
	$(LD) $(test_obj) $(LDFLAGS) -o $(MAIN_TEST)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	$(RM) $(obj)

fclean: clean
	$(RM) $(BIN_DIR)/*
	find . -type f -name "*.o" -delete

leak: $(MAIN)
	valgrind --leak-check=full $(MAIN)

generate_cc:
	bear -- make
