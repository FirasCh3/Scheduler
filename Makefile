CC = gcc
CFLAGS = -Wall -Wextra -MMD -Iinclude
SRCS = $(wildcard src/*.c) $(wildcard policies/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
RAYLIB_DIR = ./libs/raylib
RAYGUI_DIR = ./libs/raygui
RAYLIB_TARGET = $(RAYLIB_DIR)/src/libraylib.a
CFLAGS += -I$(RAYLIB_DIR)/src -I$(RAYGUI_DIR)/src -I$(RAYGUI_DIR)/styles -I$(RAYGUI_DIR)/examples -L$(RAYLIB_DIR)/src -lm
TARGET = bin/scheduler

all: $(TARGET)

$(TARGET): $(OBJS) $(RAYLIB_TARGET) | bin
	$(CC) $(OBJS) $(RAYLIB_TARGET) -o $(TARGET) $(CFLAGS)
bin:
	mkdir -p bin
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	$(MAKE) -C $(RAYLIB_DIR)/src clean
	rm -f $(OBJS) $(DEPS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(RAYLIB_TARGET):
	$(MAKE) -C $(RAYLIB_DIR)/src
