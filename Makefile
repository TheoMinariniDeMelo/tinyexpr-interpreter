CC := gcc
CCFLAGS := -lncurses
OBJ_DIR := ./obj
SRC_DIR := ./src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
TARGET := calc

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $^ $(CCFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
