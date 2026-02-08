# Toolchain
CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld

# Directories
SRC_DIR = src
BUILD_DIR = build
OUTPUT_DIR = output

# Flags
# Added -O0 for easier debugging and -g for symbols
CFLAGS = -Wall -Wextra -ffreestanding -nostdlib -mcmodel=medany -Iinclude -O0 -g
LDFLAGS = -T linker.ld

# 1. Find sources
SRCS_C = $(shell find $(SRC_DIR) -name "*.c")
SRCS_S = $(shell find $(SRC_DIR) -name "*.S")

# 2. Object mapping
OBJS_C = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS_C))
OBJS_S = $(patsubst $(SRC_DIR)/%.S, $(BUILD_DIR)/%.o, $(SRCS_S))
# Put Assembly first to help the linker find _start
OBJS   = $(OBJS_S) $(OBJS_C)

# 3. List of all required build subdirectories
OBJ_DIRS = $(sort $(dir $(OBJS)))

TARGET = $(OUTPUT_DIR)/kernel.elf

.PHONY: all clean run

all: $(TARGET)

# The final link step
# Added -Wl, to pass LDFLAGS to the actual linker
$(TARGET): $(OBJS) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -Wl,$(LDFLAGS) $(OBJS) -o $(TARGET)

# Rule for C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for Assembly files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(OBJ_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories using order-only prerequisites (|)
$(OBJ_DIRS) $(OUTPUT_DIR):
	mkdir -p $@

run: all
	clear
	qemu-system-riscv64 -M virt -bios none -kernel $(TARGET) -nographic

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
