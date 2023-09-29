SRC_DIR := src
BIN_DIR := bin
NAME := $(BIN_DIR)/kilo

CC := clang++
LANG := -x c++
STD := -std=c++20
WARNINGS := -Wall -Wextra -Wpedantic -Weffc++ -Wconversion -Wshadow \
	    -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual \
	    -Wnull-dereference -Wno-unused-parameter -Wno-unused-function \
	    -Wno-unused-variable
OPTM := -O0
CFLAGS := $(LANG) $(STD) $(WARNINGS) $(DEBUG) $(OPTM)

LIB := -lfmt
SAN := -fsanitize=address,undefined

src := $(shell find $(SRC_DIR) -type f -name "*.cpp")
obj := $(src:.cpp=.o)

.PHONY: all run init debug compile clean fclean leak generate_cc test

all: compile

run: compile
	./$(NAME) $(arg1)

test: $(NAME)
	./$(NAME) file

debug: CFLAGS += -g

debug: fclean compile

compile: $(NAME)

$(NAME): $(obj)
	@test -d bin || mkdir bin
	$(CC) $(obj) $(LIB) $(SAN) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(RM) $(obj)

fclean: clean
	$(RM) $(BIN_DIR)/*
	find . -type f -name "*.o" -delete

leak: $(NAME)
	valgrind --leak-check=full $(NAME)

generate_cc:
	bear -- make
