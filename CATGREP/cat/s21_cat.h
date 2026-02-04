#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdbool.h>
#include <stdio.h>

typedef struct{
    bool b, e, E, n, s, t, v, T;
} cat_flags;

int parse_args(int argc, char* argv[], cat_flags* opts);
bool process_gnu_style(const char* arg, cat_flags* opts);
bool process_flags(const char* arg, cat_flags* opts);
bool process_file(const char* filename, cat_flags opts);
void print_char(int ch, cat_flags opts);

#endif