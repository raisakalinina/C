CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_cat

CAT_SRC = s21_cat.c

all: $(TARGET)

$(TARGET): $(CAT_SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(CAT_SRC)

clean:
	rm -f $(TARGET)