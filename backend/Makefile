# Variables
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lcjson
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SCHEDULING_SRC = $(SRC_DIR)/main_scheduling.c
SYNC_SRC       = $(SRC_DIR)/main_synchronization.c

COMMON_SOURCES = $(filter-out $(SCHEDULING_SRC) $(SYNC_SRC), $(wildcard $(SRC_DIR)/*.c))
COMMON_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(COMMON_SOURCES))

SCHEDULING_OBJ = $(OBJ_DIR)/main_scheduling.o
SYNC_OBJ       = $(OBJ_DIR)/main_synchronization.o

SCHEDULING_BIN = $(BIN_DIR)/scheduling-simulator
SYNC_BIN       = $(BIN_DIR)/synchronization-simulator

# Default target
all: $(SCHEDULING_BIN) $(SYNC_BIN)

# Compilación de ejecutables
$(SCHEDULING_BIN): $(SCHEDULING_OBJ) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SYNC_BIN): $(SYNC_OBJ) $(COMMON_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regla general para objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
