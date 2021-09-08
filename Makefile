# -*- Makefile -*-
HEADERS_DIR = header
SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj

CC = gcc
CFLAGS = -g -Wall -Wextra -std=c17 -O2 -I$(HEADERS_DIR) -Wswitch-default -Wconversion -fsanitize=address -fsanitize=undefined -fstack-protector -pedantic # -fsanitize=thread 
DEBUG = -Wswitch-default -Wconversion -fsanitize={address,thread,undefined} -fstack-protector -pedantic

HEADERS_PATHLESS = shell.h utils.h
HEADERS = $(patsubst %, $(HEADERS_DIR)/%, $(HEADERS_PATHLESS))

OBJ_PATHLESS = shell.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(OBJ_PATHLESS))

ash: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

debug: $(OBJ)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o ash