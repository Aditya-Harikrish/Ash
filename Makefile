# -*- Makefile -*-
HEADERS_DIR = header
SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj

CC = clang
CFLAGS = -g -Wall -Wextra -std=gnu11 -O2 -I$(HEADERS_DIR) -Wswitch-default -Wconversion -fsanitize=address -fsanitize=undefined -fstack-protector # -fsanitize=thread 

HEADERS_PATHLESS = shell.h utils.h header.h prompt.h builtin.h ls.h
HEADERS = $(patsubst %, $(HEADERS_DIR)/%, $(HEADERS_PATHLESS))

OBJ_PATHLESS = shell.o prompt.o utils.o main.o builtin.o ls.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(OBJ_PATHLESS))

ash: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

debug: CFLAGS += -DDEBUG=1 -Werror
debug: ash

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o ash