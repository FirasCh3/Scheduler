CC = gcc
CFLAGS = -Wall -Wextra -MMD -Iinclude
SRCS = $(wildcard src/*.c) $(wildcard policies/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
TARGET = bin/scheduler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

run: $(TARGET)
	./$(TARGET)