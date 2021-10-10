# -*- Makefile -*-
HEADERS_DIR = header
SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj

CC = clang
CFLAGS = -I$(HEADERS_DIR) -g -std=gnu11 # -fsanitize=thread 

HEADERS_PATHLESS = shell.h utils.h header.h prompt.h builtin.h ls.h vector.h history.h fg_bg.h io.h
HEADERS = $(patsubst %, $(HEADERS_DIR)/%, $(HEADERS_PATHLESS))

OBJ_PATHLESS = shell.o prompt.o utils.o main.o builtin.o ls.o vector.o history.o fg_bg.o io.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(OBJ_PATHLESS))

ash: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

debug: CFLAGS += -DDEBUG=1 -Wall -Wextra -O2 -Wswitch-default -Wconversion -Wundef -fsanitize=address -fsanitize=undefined -fstack-protector # -Werror
debug: ash

dev: CFLAGS += -Wall -Wextra -O2 -Wswitch-default -Wconversion -Wundef -fsanitize=address -fsanitize=undefined -fstack-protector
dev: ash

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o ash