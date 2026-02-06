#include "s21_string.h"

void *s21_memchr(const void *str, int c, s21_size_t n) {
  const unsigned char *ptr = (const unsigned char *)str;
  unsigned char target = (unsigned char)c;
  void *res = S21_NULL;

  for (s21_size_t i = 0; i < n; i++) {
    if (ptr[i] == target) {
      res = (void *)(ptr + i);
      break;
    }
  }

  return res;
}

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  const unsigned char *s1 = (const unsigned char *)str1;
  const unsigned char *s2 = (const unsigned char *)str2;

  for (s21_size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return 0;
}

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;
  for (s21_size_t i = 0; i < n; i++) {
    d[i] = s[i];
  }
  return dest;
}

void *s21_memset(void *str, int c, s21_size_t n) {
  unsigned char *ptr = (unsigned char *)str;
  for (s21_size_t i = 0; i < n; i++) {
    ptr[i] = (unsigned char)c;
  }
  return str;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  s21_size_t i = 0;
  while (dest[i] != '\0') {
    i++;
  }

  s21_size_t j = 0;
  while (j < n && src[j] != '\0') {
    dest[i + j] = src[j];
    j++;
  }

  dest[i + j] = '\0';

  return dest;
}

char *s21_strchr(const char *str, int c) {
  char *res = S21_NULL;
  s21_size_t i = 0;
  unsigned char target = (unsigned char)c;

  while (res == S21_NULL && str[i] != '\0') {
    if ((unsigned char)str[i] == target) {
      res = (char *)(str + i);
    }
    i++;
  }

  if (res == S21_NULL && target == '\0') {
    res = (char *)(str + i);
  }

  return res;
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  s21_size_t i = 0;
  int diff = 0;

  if (n > 0) {
    while (i < n - 1 && str1[i] != '\0' && str1[i] == str2[i]) {
      i++;
    }
    diff = (unsigned char)str1[i] - (unsigned char)str2[i];
  }

  return diff;
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  int src_ended = 0;

  for (s21_size_t i = 0; i < n; i++) {
    if (!src_ended && src[i] != '\0') {
      dest[i] = src[i];
    } else {
      src_ended = 1;
      dest[i] = '\0';
    }
  }
  return dest;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  s21_size_t i = 0;
  int found = 0;

  while (str1[i] != '\0' && !found) {
    if (s21_strchr(str2, str1[i]) != S21_NULL) {
      found = 1;
    } else {
      i++;
    }
  }

  return i;
}

s21_size_t s21_strlen(const char *str) {
  s21_size_t len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

char *s21_strpbrk(const char *str1, const char *str2) {
  char *res = S21_NULL;
  s21_size_t i = 0;

  while (res == S21_NULL && str1[i] != '\0') {
    if (s21_strchr(str2, str1[i]) != S21_NULL) {
      res = (char *)(str1 + i);
    }
    i++;
  }

  return res;
}

char *s21_strrchr(const char *str, int c) {
  char *res = S21_NULL;
  unsigned char target = (unsigned char)c;
  s21_size_t i = 0;

  while (1) {
    if ((unsigned char)str[i] == target) {
      res = (char *)(str + i);
    }
    if (str[i] == '\0') {
      break;
    }
    i++;
  }
  return res;
}

char *s21_strstr(const char *haystack, const char *needle) {
  char *res = S21_NULL;

  if (*needle == '\0') {
    res = (char *)haystack;
  }

  for (s21_size_t i = 0; haystack[i] != '\0' && res == S21_NULL; i++) {
    s21_size_t j = 0;
    while (haystack[i + j] == needle[j] && needle[j] != '\0') {
      j++;
    }

    if (needle[j] == '\0') {
      res = (char *)(haystack + i);
    }
  }

  return res;
}

char *s21_strtok(char *str, const char *delim) {
  static char *last = S21_NULL;
  char *res = S21_NULL;

  if (str != S21_NULL) {
    last = str;
  }

  if (last != S21_NULL) {
    while (*last != '\0' && s21_strchr(delim, *last) != S21_NULL) {
      last++;
    }

    if (*last == '\0') {
      res = S21_NULL;
      last = S21_NULL;
    } else {
      res = last;
      last += s21_strcspn(last, delim);

      if (*last != '\0') {
        *last = '\0';
        last++;
      } else {
        last = S21_NULL;
      }
    }
  }

  return res;
}