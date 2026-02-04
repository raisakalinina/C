CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

TARGET = s21_grep
SRCS = s21_grep.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRC_DIR)/s21_grep.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

rebuild: clean all

clean:
	rm -f *.o $(TARGET)