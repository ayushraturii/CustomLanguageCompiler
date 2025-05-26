# the compiler: clang for C programs
CC = clang

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -Wall -Wextra -g -O0 -std=gnu99 -fstack-protector-all -ftrapv
# Switch on debugger
# CFLAGS += -fsanitize=address
# Check the operating system
ifeq ($(shell uname), Darwin)
	CFLAGS += -D TARGET_ARCH_M1
endif

# Linker flags
LDFLAGS = -lfl

BIN_DIR = bin
BUILD_DIR = build

all: $(BUILD_DIR)/dd

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(BUILD_DIR)/lex.yy.c: dd.l $(BUILD_DIR)/y.tab.h
	lex -t $< > $@

$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c
	$(CC) $(CFLAGS) -Wno-unused-function -c $< -o $@

$(BUILD_DIR)/y.tab.c $(BUILD_DIR)/y.tab.h: dd.y
	yacc -d $< -o $(BUILD_DIR)/y.tab.c

$(BUILD_DIR)/y.tab.o: $(BUILD_DIR)/y.tab.c syntax.c stack.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stack.o: stack.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/assembly.o: assembly.c syntax.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/syntax.o: syntax.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/list.o: list.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/env.o: env.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/context.o: context.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/semantic.o: semantic.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/dd: $(BUILD_DIR) $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/y.tab.o $(BUILD_DIR)/assembly.o $(BUILD_DIR)/syntax.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/list.o $(BUILD_DIR)/env.o $(BUILD_DIR)/context.o $(BUILD_DIR)/semantic.o main.c
	$(CC) $(CFLAGS) -o $@ main.c $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/y.tab.o $(BUILD_DIR)/assembly.o $(BUILD_DIR)/syntax.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/list.o $(BUILD_DIR)/env.o $(BUILD_DIR)/context.o $(BUILD_DIR)/semantic.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) 