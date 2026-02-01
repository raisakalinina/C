#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATTERNS 100
#define MAX_LINE 4096

typedef struct {
  bool e, i, v, c, l, n;
  char *patterns[MAX_PATTERNS];
  int pattern_count;
} grep_flags;

int parse_args(int argc, char *argv[], grep_flags *flags);
int process_flag(char *argv[], int *i, grep_flags *flags);
void add_pattern(grep_flags *flags, const char *pattern);
void free_patterns(grep_flags *flags);
void process_file(const char *file_name, grep_flags *flags, bool file_count);
void print_matches(FILE *fp, const char *file_name, grep_flags *flags,
                   bool file_count);
bool match_line(const char *line, grep_flags *flags);
void print_line(const char *line, const char *file_name, grep_flags *flags,
                bool file_count, int line_num);

#endif