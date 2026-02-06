CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
LDFLAGS = -lcheck -lpthread -lm 

# Определение ОС
OS = $(shell uname)
ifeq ($(OS), Darwin)
    LDFLAGS = -lcheck
endif
ifeq ($(OS), Linux)
    LDFLAGS += -lrt -lsubunit
endif

LIB = s21_string.a
SRC = s21_string.c s21_sprintf.c s21_strerror.c
OBJ = $(SRC:.c=.o)
TEST_SRC = test_main.c

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(LIB)
	$(CC) $(CFLAGS) $(TEST_SRC) $(LIB) $(LDFLAGS) -o test_exe
	./test_exe

gcov_report: clean
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(SRC) $(TEST_SRC) $(LDFLAGS) -o gcov_test
	./gcov_test
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory report

clean:
	rm -rf *.o *.a *.gcno *.gcda *.info test_exe gcov_test report

.PHONY: all clean test gcov_report