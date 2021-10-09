# -*- Makefile -*-
HEADERS_DIR = header
SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj

CC = clang
CFLAGS = -I$(HEADERS_DIR) -g -Wall -Wextra -std=gnu11 -O2 -Wswitch-default -Wconversion -Wundef -fsanitize=address -fsanitize=undefined -fstack-protector # -fsanitize=thread 

HEADERS_PATHLESS = shell.h utils.h header.h prompt.h builtin.h ls.h vector.h history.h fg_bg.h
HEADERS = $(patsubst %, $(HEADERS_DIR)/%, $(HEADERS_PATHLESS))

OBJ_PATHLESS = shell.o prompt.o utils.o main.o builtin.o ls.o vector.o history.o fg_bg.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(OBJ_PATHLESS))

ash: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

debug: CFLAGS += -DDEBUG=1 # -Werror
debug: ash

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o ash