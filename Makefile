# Sets the C compiler to be used. In this case, it's 'gcc'.
CC = gcc

# Sets the flags to be passed to the compiler during compilation.
# -Iinclude: Tells the compiler to look for header files in the 'include' directory.
# -Wall: Enables all standard warnings for common programming errors.
# -Wextra: Enables additional warnings not covered by -Wall.
# -g: Includes debugging information in the final executable, which is useful for tools like gdb.
CFLAGS = -Iinclude -Wall -Wextra -g

# Sets flags for the linker. This is empty now but could be used to link external libraries (e.g., -lm for the math library).
LDFLAGS =

# --- Directory Variables ---

# Defines the directory where your source (.c) files are located.
SRC_DIR = src
# Defines the directory where compiled object (.o) files will be stored.
OBJ_DIR = obj
# Defines the directory where the final executable will be placed.
BIN_DIR = bin

# --- File Generation ---

# SRCS: Creates a list of all files ending in .c within the SRC_DIR.
# For example, this will expand to "src/main.c src/pcap_reader.c ...".
SRCS = $(wildcard $(SRC_DIR)/*.c)

# OBJS: Creates a corresponding list of object file paths.
# It takes the list from SRCS and replaces each "src/%.c" with "obj/%.o".
# For example, "src/main.c" becomes "obj/main.o".
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# TARGET: Defines the full path and name of the final executable file.
TARGET = $(BIN_DIR)/pcap_parser

# --- Phony Targets ---

# Declares 'all' and 'clean' as "phony" targets. This tells 'make' that these are
# just command names, not actual files. This ensures 'make' will always run the
# associated commands, regardless of whether a file named 'all' or 'clean' exists.
.PHONY: all clean

# --- Build Rules ---

# This is the default rule that runs when you just type 'make'.
# It states that the 'all' target depends on the 'TARGET' file. So, to satisfy 'all',
# 'make' must first successfully build the file defined by $(TARGET).
all: $(TARGET)

# This rule defines how to build the final executable ($(TARGET)).
# It depends on all the object files listed in $(OBJS). This rule will only run if
# the target executable doesn't exist or if any of the object files are newer.
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# This is a pattern rule that tells 'make' how to create an object file (e.g., obj/main.o)
# from a source file (e.g., src/main.c).
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c

	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
