#include <math.h>
#include <stdarg.h>

#include "s21_string.h"

#define INT_BUF_SIZE 64
#define FLOAT_BUF_SIZE 128

typedef struct {
  int minus;      // флаг -
  int plus;       // флаг +
  int space;      // флаг ' '
  int width;      // ширина
  int precision;  // точность
  char length;    // длина
  char spec;      // спецификаторы
} format_info;

typedef struct {
  char *pos;
  int written;
} writer;

static int process_format(writer *w, const char *fmt, va_list *args);
static void writer_put_char(writer *w, char c);
static int handle_one_specifier(writer *w, const char **fmt_ptr, va_list *args);
static const char *parse_spec(const char *fmt, format_info *info);

static long long get_signed_arg(va_list *args, char length);
static unsigned long long get_unsigned_arg(va_list *args, char length);
static int pad_and_put_str(writer *w, const char *str, int str_len,
                           const format_info *info);
static int handle_percent(writer *w, const format_info *info);
static int handle_char(writer *w, int ch, const format_info *info);
static int handle_string(writer *w, const char *s, const format_info *info);
static int handle_int(writer *w, long long value, const format_info *info);
static int handle_uint(writer *w, unsigned long long value,
                       const format_info *info);
static int s21_put_float(char *buf, long double value, int precision);
static int handle_float(writer *w, long double value, const format_info *info);

int s21_sprintf(char *str, const char *format, ...) {
  if (!str || !format) return 0;
  va_list args;
  va_start(args, format);
  writer w = {str, 0};
  int total = process_format(&w, format, &args);
  *w.pos = '\0';
  va_end(args);
  return total;
}

static int process_format(writer *w, const char *fmt, va_list *args) {
  int total = 0;
  while (*fmt) {
    if (*fmt != '%') {
      writer_put_char(w, *fmt++);
      total++;
      continue;
    }
    fmt++;  // пропуск встреченного %
    total += handle_one_specifier(w, &fmt, args);
  }
  return total;
}

// Пишет один символ в буфер, обновляет pos и written
static void writer_put_char(writer *w, char c) {
  *w->pos = c;
  w->pos++;
  w->written++;
}

static int handle_one_specifier(writer *w, const char **fmt_ptr,
                                va_list *args) {
  const char *fmt = *fmt_ptr;
  format_info info = {0};
  info.precision = -1;
  fmt = parse_spec(fmt, &info);  // читаем флаги, ширину, точность, длину, spec
  int added = 0;
  if (info.spec == '%') {
    added = handle_percent(w, &info);
  } else if (info.spec == 'c') {
    added = handle_char(w, va_arg(*args, int), &info);
  } else if (info.spec == 's') {
    added = handle_string(w, va_arg(*args, char *), &info);
  } else if (info.spec == 'd') {
    long long val = get_signed_arg(args, info.length);
    added = handle_int(w, val, &info);
  } else if (info.spec == 'u') {
    unsigned long long val = get_unsigned_arg(args, info.length);
    added = handle_uint(w, val, &info);
  } else if (info.spec == 'f') {
    long double val;
    if (info.length == 'l') {
      val = va_arg(*args, long double);
    } else {
      val = va_arg(*args, double);
    }
    added = handle_float(w, val, &info);
  } else {
    writer_put_char(w, '%');
    if (*fmt) {
      writer_put_char(w, *fmt);
      fmt++;
    }
    added = 2;
  }
  *fmt_ptr = fmt;  // возвращаем сдвинутый указатель
  return added;
}

static const char *parse_spec(const char *fmt, format_info *info) {
  while (*fmt) {  // Флаги
    if (*fmt == '-') {
      info->minus = 1;
      fmt++;
      continue;
    } else if (*fmt == '+') {
      info->plus = 1;
      fmt++;
      continue;
    } else if (*fmt == ' ') {
      info->space = 1;
      fmt++;
      continue;
    } else
      break;
  }

  if (*fmt >= '0' && *fmt <= '9') {  // Ширина
    info->width = 0;
    while (*fmt >= '0' && *fmt <= '9') {
      info->width = info->width * 10 + (*fmt - '0');
      fmt++;
    }
  }

  if (*fmt == '.') {  // Точность
    fmt++;
    info->precision = 0;
    if (*fmt >= '0' && *fmt <= '9') {
      while (*fmt >= '0' && *fmt <= '9') {
        info->precision = info->precision * 10 + (*fmt - '0');
        fmt++;
      }
    }
  }

  if (*fmt == 'h' || *fmt == 'l') {
    info->length = *fmt;
    fmt++;
  }

  if (*fmt && s21_strchr("cdiuf s%", *fmt)) {
    info->spec = *fmt;
    fmt++;
  }

  return fmt;
}

static int handle_percent(writer *w, const format_info *info) {
  return pad_and_put_str(w, "%", 1, info);
}

static int handle_char(writer *w, int ch, const format_info *info) {
  return pad_and_put_str(w, (char[]){(char)ch, 0}, 1, info);
}

// Вспомогательная функция для паддинга
static int pad_and_put_str(writer *w, const char *str, int str_len,
                           const format_info *info) {
  int padding = info->width > str_len ? info->width - str_len : 0;
  int total = str_len + padding;

  if (info->minus) {
    // слева строка → справа пробелы
    s21_memcpy(w->pos, str, str_len);
    w->pos += str_len;
    w->written += str_len;
    for (int i = 0; i < padding; i++) writer_put_char(w, ' ');
  } else {
    // слева пробелы → справа строка
    for (int i = 0; i < padding; i++) writer_put_char(w, ' ');
    s21_memcpy(w->pos, str, str_len);
    w->pos += str_len;
    w->written += str_len;
  }
  *w->pos = '\0';
  return total;
}

static int handle_string(writer *w, const char *s, const format_info *info) {
  if (!s) {
    s = "(null)";
  }

  s21_size_t full_len = s21_strlen(s);
  s21_size_t print_len = full_len;

  // Если точность == 0 → ничего не выводим (даже для "(null)")
  if (info->precision == 0) {
    print_len = 0;
  }
  // Если точность > 0 и меньше длины строки → обрезаем
  if (info->precision > 0 && (s21_size_t)info->precision < print_len) {
    print_len = info->precision;
  }

  int str_len = (int)print_len;
  return pad_and_put_str(w, s, str_len, info);
}

// Возвращает длину строки в buf (включая знак и ведущие нули)
static int format_integer_to_buf(char *buf, unsigned long long value,
                                 int is_signed, long long signed_value,
                                 const format_info *info) {
  if (info->precision == 0 && value == 0) {
    buf[0] = '\0';
    return 0;
  }

  char *ptr = buf + INT_BUF_SIZE - 1;
  *ptr = '\0';

  // Формируем цифры числа (обратный порядок)
  char *digits_start = ptr;
  do {
    *--ptr = '0' + (value % 10);
    value /= 10;
  } while (value > 0);

  // Ведущие нули по точности
  int digits_len = digits_start - ptr;
  int zeros = info->precision - digits_len;
  if (zeros > 0) {
    while (zeros--) *--ptr = '0';
  }

  // Знак (если signed)
  if (is_signed) {
    char sign = '\0';
    if (signed_value < 0)
      sign = '-';
    else if (info->plus)
      sign = '+';
    else if (info->space)
      sign = ' ';

    if (sign) *--ptr = sign;
  }

  // Копируем результат в начало buf
  int len = digits_start - ptr;
  s21_memcpy(buf, ptr, len);
  buf[len] = '\0';

  return len;
}

static long long get_signed_arg(va_list *args, char length) {
  if (length == 'h') return (short)va_arg(*args, int);  //%hd
  if (length == 'l') return va_arg(*args, long);        //%ld
  return va_arg(*args, int);                            //%d
}

static unsigned long long get_unsigned_arg(va_list *args, char length) {
  if (length == 'h') return (unsigned short)va_arg(*args, unsigned int);  //%hu
  if (length == 'l') return va_arg(*args, unsigned long);                 //%lu
  return va_arg(*args, unsigned int);                                     //%u
}

static int handle_int(writer *w, long long value, const format_info *info) {
  char buf[INT_BUF_SIZE];
  int len = format_integer_to_buf(
      buf, (unsigned long long)(value < 0 ? -value : value), 1, value, info);
  return pad_and_put_str(w, buf, len, info);
}

static int handle_uint(writer *w, unsigned long long value,
                       const format_info *info) {
  char buf[INT_BUF_SIZE];
  int len = format_integer_to_buf(buf, value, 0, 0, info);
  return pad_and_put_str(w, buf, len, info);
}

static int handle_float(writer *w, long double value, const format_info *info) {
  int precision = info->precision >= 0 ? info->precision : 6;
  char buf[FLOAT_BUF_SIZE] = {0};
  int len = s21_put_float(buf, fabsl(value), precision);

  char sign_buf[FLOAT_BUF_SIZE + 1];
  char *sign_ptr = sign_buf;
  int negative = signbit(value);
  char sign = '\0';
  if (negative) {
    sign = '-';
  } else if (info->plus) {
    sign = '+';
  } else if (info->space) {
    sign = ' ';
  }

  if (sign) {
    *sign_ptr++ = sign;
    s21_memcpy(sign_ptr, buf, len);
    sign_ptr += len;
    *sign_ptr = '\0';
    len++;  // +1 за знак
  } else {
    s21_memcpy(sign_buf, buf, len);
    sign_buf[len] = '\0';
  }

  // Выводим с паддингом
  return pad_and_put_str(w, sign_buf, len, info);
}

static int s21_put_float(char *buf, long double value, int precision) {
  if (precision < 0) precision = 6;
  char *ptr = buf;
  int count = 0;
  // Масштаб для округления
  long double scale = 1.0L;
  for (int i = 0; i < precision; i++) scale *= 10.0L;

  // Округляем ВСЁ число
  long double scaled = value * scale;
  unsigned long long rounded = (unsigned long long)scaled;

  if (scaled - rounded > 0.5L) {
    rounded++;
  }

  // Делим обратно
  unsigned long long int_part = rounded / (unsigned long long)scale;
  unsigned long long frac_part = rounded % (unsigned long long)scale;

  // --- целая часть ---
  char int_buf[32];
  char *p = int_buf + sizeof(int_buf) - 1;
  *p = '\0';

  do {
    *--p = '0' + (int_part % 10);
    int_part /= 10;
    count++;
  } while (int_part);

  ptr = s21_memcpy(ptr, p, count) + count;

  // --- дробная часть ---
  if (precision > 0) {
    *ptr++ = '.';
    count++;

    // ведущие нули
    for (int i = precision - 1; i >= 0; i--) {
      ptr[i] = '0' + (frac_part % 10);
      frac_part /= 10;
    }
    ptr += precision;
    count += precision;
  }

  *ptr = '\0';
  return count;
}
