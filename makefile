SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

CC = gcc

CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL2

C_SRC = $(wildcard $(SRC_DIR)/*.c)

DEBUG_DEFINES = -D DEBUG

.PHONY: all clean debug

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(C_SRC) -I$(INC_DIR) -o $(BIN_DIR)/chip-8 -O2

debug:
	$(CC) $(CFLAGS) -g $(LDFLAGS) $(C_SRC) -I$(INC_DIR) -o $(BIN_DIR)/chip-8 $(DEBUG_DEFINES)

clean:
	rm -f $(BIN_DIR)/*
