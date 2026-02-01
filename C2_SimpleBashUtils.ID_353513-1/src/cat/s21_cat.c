#include "s21_cat.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
  cat_flags flags = {0};
  int file_name = parse_args(argc, argv, &flags);
  bool has_error = false;
  if (file_name == -1) {  // некорректный аргумент
    fprintf(stderr, "s21_cat: unrecognized option\n");
    has_error = true;
  } else if (file_name >= argc) {  // нет файлов
    fprintf(stderr, "s21_cat: no input files\n");
    has_error = true;
  } else {
    for (int i = file_name; i < argc; i++) {
      if (!process_file(argv[i], flags)) {
        has_error = true;
        break;
      }
    }
  }
  return has_error ? 1 : 0;
}

int parse_args(int argc, char* argv[], cat_flags* flags) {
  int file_name = argc;
  bool is_error = false;
  for (int i = 1; i < argc && !is_error; i++) {
    char* arg = argv[i];
    if (strcmp(arg, "--") == 0) {
      file_name = i + 1;
      break;
    } else if (arg[0] == '-' && arg[1] == '-') {
      is_error = !process_gnu_style(arg, flags);
    } else if (arg[0] == '-') {
      is_error = !process_flags(arg, flags);
    } else {
      file_name = i;
      break;
    }
  }
  return is_error ? -1 : file_name;
}

bool process_gnu_style(const char* arg, cat_flags* flags) {
  bool has_error = true;
  if (strcmp(arg, "--number") == 0) {
    if (!flags->b) flags->n = true;
  } else if (strcmp(arg, "--squeeze-blank") == 0) {
    flags->s = true;
  } else if (strcmp(arg, "--number-nonblank") == 0) {
    flags->b = true;
    flags->n = false;
  } else {
    has_error = false;
  }
  return has_error;
}

bool process_flags(const char* arg, cat_flags* flags) {
  bool has_error = true;
  for (int i = 1; arg[i] != '\0'; i++) {
    switch (arg[i]) {
      case 'b':
        flags->b = true;
        flags->n = false;
        break;
      case 'n':
        if (!flags->b) flags->n = true;
        break;
      case 'e':
        flags->e = true;
        flags->v = true;
        break;
      case 'E':
        flags->E = true;
        break;
      case 's':
        flags->s = true;
        break;
      case 't':
        flags->t = true;
        flags->v = true;
        break;
      case 'T':
        flags->T = true;
        break;
      case 'v':
        flags->v = true;
        break;
      default:
        has_error = false;
        break;
    }
  }
  return has_error;
}

bool process_file(const char* file_name, cat_flags flags) {
  FILE* fp = fopen(file_name, "r");
  bool success = true;
  if (fp == NULL) {
    fprintf(stderr, "s21_cat: %s: No such file or directory\n", file_name);
    success = false;
  } else {
    int current_ch = fgetc(fp);
    int prev_ch = '\n';
    bool squeeze = false;
    int line_num = 1;
    while (current_ch != EOF) {
      if (flags.s && current_ch == '\n' && prev_ch == '\n') {
        if (squeeze) {
          current_ch = fgetc(fp);
          continue;
        }
        squeeze = true;
      } else {
        squeeze = false;
      }
      if ((flags.b && prev_ch == '\n' && current_ch != '\n') ||
          (flags.n && prev_ch == '\n')) {
        printf("%6d\t", line_num);
        line_num++;
      }
      if (current_ch == '\n') {
        if (flags.e || flags.E) {
          putchar('$');
        }
        putchar('\n');
      } else {
        print_char(current_ch, flags);
      }
      prev_ch = current_ch;
      current_ch = fgetc(fp);
    }
    fclose(fp);
  }
  return success;
}

void print_char(int current_ch, cat_flags flags) {
  if ((flags.t || flags.T) && current_ch == '\t') {
    printf("^I");
  } else if (flags.v) {
    if (current_ch >= 0 && current_ch < 32 && current_ch != '\n' &&
        current_ch != '\t') {
      printf("^%c", current_ch + 64);
    } else if (current_ch == 127) {
      printf("^?");
    } else {
      putchar(current_ch);
    }
  } else {
    putchar(current_ch);
  }
}