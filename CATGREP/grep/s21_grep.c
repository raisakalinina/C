#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grep_flags flags = {0};
  int result = 0;
  int file_name = parse_args(argc, argv, &flags);

  bool file_count = argc - file_name > 1;
  if (file_name != -1) {
    for (int i = file_name; i < argc; i++) {
      process_file(argv[i], &flags, file_count);
    }
  } else {
    result = 1;
  }

  free_patterns(&flags);
  return result;
}

int parse_args(int argc, char *argv[], grep_flags *flags) {
  int i = 1, status = 0;
  int file_name = argc;
  while (i < argc && status == 0) {
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      status = process_flag(argv, &i, flags);
    } else {
      break;
    }
  }

  file_name = i;
  if (flags->e == 0 && file_name < argc) {
    add_pattern(flags, argv[(file_name)++]);
  }
  return status ? 1 : file_name;
}

int process_flag(char *argv[], int *i, grep_flags *flags) {
  int status = 0;
  char *arg = argv[*i];

  if (strcmp(arg, "-e") == 0 && argv[*i + 1]) {
    add_pattern(flags, argv[++(*i)]);
    flags->e = true;
  } else if (strcmp(arg, "-i") == 0) {
    flags->i = true;
  } else if (strcmp(arg, "-v") == 0) {
    flags->v = true;
  } else if (strcmp(arg, "-c") == 0) {
    flags->c = true;
  } else if (strcmp(arg, "-l") == 0) {
    flags->l = true;
  } else if (strcmp(arg, "-n") == 0) {
    flags->n = true;
  } else {
    status = 1;
  }

  (*i)++;
  return status;
}

void add_pattern(grep_flags *flags, const char *pattern) {
  if (flags->pattern_count < MAX_PATTERNS) {
    flags->patterns[flags->pattern_count] = malloc(strlen(pattern) + 1);
    if (flags->patterns[flags->pattern_count]) {
      strcpy(flags->patterns[flags->pattern_count], pattern);
      flags->pattern_count++;
    }
  }
}

void free_patterns(grep_flags *flags) {
  for (int i = 0; i < flags->pattern_count; i++) {
    free(flags->patterns[i]);
  }
}

void process_file(const char *file_name, grep_flags *flags, bool file_count) {
  FILE *fp = fopen(file_name, "r");

  if (!fp) {
    fprintf(stderr, "s21_grep: %s: No such file\n", file_name);
    return;
  }

  print_matches(fp, file_name, flags, file_count);
  fclose(fp);
}

void print_matches(FILE *fp, const char *file_name, grep_flags *flags,
                   bool file_count) {
  char line[MAX_LINE];
  int match_count = 0, line_num = 0;

  while (fgets(line, MAX_LINE, fp)) {
    line_num++;
    bool match = match_line(line, flags);

    if (flags->v) match = !match;
    if (match) {
      match_count++;
      if (!flags->c && !flags->l) {
        print_line(line, file_name, flags, file_count, line_num);
      }
    }
  }

  if (flags->c) {
    if (file_count) printf("%s:", file_name);
    printf("%d\n", match_count);
  } else if (flags->l && match_count > 0) {
    printf("%s\n", file_name);
  }
}

bool match_line(const char *line, grep_flags *flags) {
  bool matched = false;

  for (int i = 0; i < flags->pattern_count && !matched; i++) {
    regex_t regex;
    int reg_flags = flags->i ? REG_ICASE : 0;

    if (regcomp(&regex, flags->patterns[i], reg_flags) == 0) {
      matched = regexec(&regex, line, 0, NULL, 0) == 0;
      regfree(&regex);
    }
  }
  return matched;
}

void print_line(const char *line, const char *file_name, grep_flags *flags,
                bool file_count, int line_num) {
  if (file_count) {
    printf("%s:", file_name);
  }

  if (flags->n) {
    printf("%d:", line_num);
  }

  fputs(line, stdout);
}