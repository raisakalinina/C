#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_string.h"

#define BUF_SIZE 512

#define ASSERT_SPRINTF(fmt, ...)                      \
  do {                                                \
    char my[BUF_SIZE] = {0};                          \
    char std[BUF_SIZE] = {0};                         \
    int my_len = s21_sprintf(my, fmt, ##__VA_ARGS__); \
    int std_len = sprintf(std, fmt, ##__VA_ARGS__);   \
    ck_assert_int_eq(my_len, std_len);                \
    ck_assert_str_eq(my, std);                        \
  } while (0)

START_TEST(test_memory_funcs) {
  char s1[20] = "Hello";
  char s2[20] = "Hello";
  char s3[20] = "Hello";

  ck_assert_ptr_eq(s21_memchr(s1, 'e', 5), memchr(s1, 'e', 5));
  ck_assert_int_eq(s21_memcmp(s1, "Hel", 3), memcmp(s1, "Hel", 3));

  void *ptr_my_cp = s21_memcpy(s1, "Aaaa", 4);
  memcpy(s2, "Aaaa", 4);
  ck_assert_ptr_eq(ptr_my_cp, s1);
  ck_assert_mem_eq(s1, s2, 4);

  void *ptr_my_set = s21_memset(s1, 'b', 5);
  memset(s3, 'b', 5);
  ck_assert_ptr_eq(ptr_my_set, s1);
  ck_assert_mem_eq(s1, s3, 5);
}
END_TEST

// 5, 8. strncat, strncpy
START_TEST(test_copy_cat) {
  char src[] = "123";
  char d1[20] = "Base";
  char d2[20] = "Base";

  s21_strncat(d1, src, 2);
  strncat(d2, src, 2);
  ck_assert_str_eq(d1, d2);

  s21_strncpy(d1, src, 2);
  strncpy(d2, src, 2);
  ck_assert_str_eq(d1, d2);
}
END_TEST

// 6, 13. strchr, strrchr
START_TEST(test_search_char) {
  char *s = "Hello World";
  int ch[] = {'l', 'H', 'd', 'z', '\0'};
  for (int i = 0; i < 5; i++) {
    ck_assert_ptr_eq(s21_strchr(s, ch[i]), strchr(s, ch[i]));
    ck_assert_ptr_eq(s21_strrchr(s, ch[i]), strrchr(s, ch[i]));
  }
}
END_TEST

// 7. strncmp
START_TEST(test_strncmp) {
  char *s1 = "Apple";
  char *s2 = "Apply";
  ck_assert_int_eq(s21_strncmp(s1, s2, 3) == 0, strncmp(s1, s2, 3) == 0);
  ck_assert_int_eq(s21_strncmp(s1, s2, 5) < 0, strncmp(s1, s2, 5) < 0);
}
END_TEST

// 9, 12. strcspn, strpbrk
START_TEST(test_spn_pbrk) {
  char *s1 = "0123456789";
  char *s2 = "987";
  ck_assert_int_eq(s21_strcspn(s1, s2), strcspn(s1, s2));
  ck_assert_ptr_eq(s21_strpbrk(s1, s2), strpbrk(s1, s2));
}
END_TEST

// 11. strlen
START_TEST(test_strlen) {
  ck_assert_int_eq(s21_strlen("Hello"), strlen("Hello"));
  ck_assert_int_eq(s21_strlen(""), strlen(""));
}
END_TEST

// 14. strstr
START_TEST(test_strstr) {
  char *h = "Hello world";
  ck_assert_ptr_eq(s21_strstr(h, "world"), strstr(h, "world"));
  ck_assert_ptr_eq(s21_strstr(h, ""), strstr(h, ""));
  ck_assert_ptr_eq(s21_strstr(h, "bye"), strstr(h, "bye"));
}
END_TEST

// 15. strtok
START_TEST(test_strtok) {
  char str1[] = "one/two/three";
  char str2[] = "one/two/three";
  char *delim = "/";
  ck_assert_str_eq(s21_strtok(str1, delim), strtok(str2, delim));
  ck_assert_str_eq(s21_strtok(NULL, delim), strtok(NULL, delim));
}
END_TEST

// strerror
START_TEST(test_strerror) {
#ifdef __APPLE__
  int limit = 107;
#elif __linux__
  int limit = 134;
#endif

  for (int i = 0; i < limit; i++) {
    ck_assert_str_eq(s21_strerror(i), strerror(i));
  }

  ck_assert_str_eq(s21_strerror(-1), strerror(-1));
  ck_assert_str_eq(s21_strerror(0), strerror(0));
  ck_assert_str_eq(s21_strerror(200), strerror(200));
}
END_TEST

START_TEST(sprintf_persent_and_text) {
  ASSERT_SPRINTF("Hello World");
  ASSERT_SPRINTF("100%% sure");
  ASSERT_SPRINTF("%% %% %%");
  ASSERT_SPRINTF("Begin%%End");
  ASSERT_SPRINTF("%%%%");
  ASSERT_SPRINTF("No percent here");
}
END_TEST

START_TEST(sprintf_char) {
  ASSERT_SPRINTF("%c", 'A');
  ASSERT_SPRINTF("%5c", 'B');
  ASSERT_SPRINTF("%-5c", 'C');
  ASSERT_SPRINTF("Chars: %c %c", 'X', 'Y');
  ASSERT_SPRINTF("%10c", '!');
  ASSERT_SPRINTF("%-10c", '!');
  ASSERT_SPRINTF("%c%c%c", '1', '2', '3');
}
END_TEST

START_TEST(sprintf_string) {
  ASSERT_SPRINTF("%s", "Hello");
  ASSERT_SPRINTF("%s", "");
  ASSERT_SPRINTF("%.5s", "HelloWorld");
  ASSERT_SPRINTF("%.0s", "Anything");
  ASSERT_SPRINTF("%.10s", "Short");
  ASSERT_SPRINTF("%10s", "Hi");
  ASSERT_SPRINTF("%-10s", "Hi");
  ASSERT_SPRINTF("%-8.3s", "Superman");
}
END_TEST

START_TEST(sprintf_signed_int) {
  ASSERT_SPRINTF("%d", 0);
  ASSERT_SPRINTF("%d", 123);
  ASSERT_SPRINTF("%d", -456);

  ASSERT_SPRINTF("%+d", 42);
  ASSERT_SPRINTF("% d", 42);
  ASSERT_SPRINTF("%+d", -42);

  ASSERT_SPRINTF("%5d", 7);
  ASSERT_SPRINTF("%-5d", 7);
  ASSERT_SPRINTF("%+5d", 7);
  ASSERT_SPRINTF("% 5d", -7);

  ASSERT_SPRINTF("%.5d", 123);
  ASSERT_SPRINTF("%.0d", 0);
  ASSERT_SPRINTF("%.0d", 123);
}
END_TEST

START_TEST(sprintf_unsigned_int) {
  ASSERT_SPRINTF("%u", 0);
  ASSERT_SPRINTF("%u", 12345);
  ASSERT_SPRINTF("%u", 4294967295U);

  ASSERT_SPRINTF("%5u", 42);
  ASSERT_SPRINTF("%-5u", 42);

  ASSERT_SPRINTF("%.5u", 123);
  ASSERT_SPRINTF("%.0u", 0);
  ASSERT_SPRINTF("%.0u", 123);
}
END_TEST

START_TEST(sprintf_float) {
  ASSERT_SPRINTF("%f", -0.5);
  ASSERT_SPRINTF("%.3f", 3.14159);
  ASSERT_SPRINTF("%.0f", 3.7);
  ASSERT_SPRINTF("%10.2f", 3.14);
  ASSERT_SPRINTF("%-10.2f", 3.14);
  ASSERT_SPRINTF("%+f", 42.0);
  ASSERT_SPRINTF("% f", 42.0);
  ASSERT_SPRINTF("%10f", 123.456);
  ASSERT_SPRINTF("%-10f", 123.456);
  ASSERT_SPRINTF("%10.2f", 3.14);
  ASSERT_SPRINTF("%-10.2f", 3.14);
  ASSERT_SPRINTF("%+10.2f", 3.14);
  ASSERT_SPRINTF("% 10.2f", 3.14);
  ASSERT_SPRINTF("%-+10.2f", 3.14);
  ASSERT_SPRINTF("%10.2f", -3.14);
  ASSERT_SPRINTF("%.0f", 3.7);
  ASSERT_SPRINTF("%.0f", 3.4);
  ASSERT_SPRINTF("%.0f", -3.7);
  ASSERT_SPRINTF("%.0f", 0.0);
  ASSERT_SPRINTF("%.0f", 0.5);
  ASSERT_SPRINTF("%.2f", 1.995);
  ASSERT_SPRINTF("%.2f", 1.996);
  ASSERT_SPRINTF("%.0f", 2.5);
  ASSERT_SPRINTF("% f", 0.0);
  ASSERT_SPRINTF("%+f", 0.0);
  ASSERT_SPRINTF("%-+10.2f", -3.14);
}
END_TEST

START_TEST(sprintf_mixed) {
  ASSERT_SPRINTF("Hello %s %c %d%%!", "World", '!', 100);
  ASSERT_SPRINTF("%-5c %10s %+d %%", 'X', "Hi", 42);
  ASSERT_SPRINTF("Num: %d, Str: %s, Char: %c", 123, "test", 'Z');
  ASSERT_SPRINTF("%u %f %s", 999, 2.71828, "pi");
  ASSERT_SPRINTF("ID:%+8d | %-10s | %c | %%", 42, "user", 'X');
  ASSERT_SPRINTF("[%5d][%-5u][%.3s][%c]", -7, 99, "Hello", '!');
}
END_TEST

START_TEST(sprintf_flag_comitations) {
  ASSERT_SPRINTF("%-5d", 42);
  ASSERT_SPRINTF("%-5u", 42);
  ASSERT_SPRINTF("%-5s", "hi");
  ASSERT_SPRINTF("%8.5d", 123);
  ASSERT_SPRINTF("%+8.5d", 123);
  ASSERT_SPRINTF("% 8.5d", 123);
  ASSERT_SPRINTF("%-8.5d", 123);
  ASSERT_SPRINTF("%8.5d", -123);
}
END_TEST

START_TEST(sprintf_edge_situations_precision) {
  ASSERT_SPRINTF("%.0d", 0);
  ASSERT_SPRINTF("%.0u", 0);
  ASSERT_SPRINTF("%.0s", "abc");
}
END_TEST

START_TEST(test_plus_space_priority) {
  char buf[BUF_SIZE];
  s21_sprintf(buf, "%+ d", 42);
  ck_assert_str_eq(buf, "+42");
}
END_TEST

START_TEST(sprintf_length) {
  ASSERT_SPRINTF("%hd", (short)32767);
  ASSERT_SPRINTF("%hu", (unsigned short)65535);
  ASSERT_SPRINTF("%ld", (long)123456789L);
  ASSERT_SPRINTF("%lu", (unsigned long)123456789UL);
}
END_TEST

START_TEST(sprintf_float_rounding_carry) {
  ASSERT_SPRINTF("%.2f", 9.999);
  ASSERT_SPRINTF("%.3f", 99.9999);
  ASSERT_SPRINTF("%.1f", 0.99);
  ASSERT_SPRINTF("%.1f", 9.95);
  ASSERT_SPRINTF("%.0f", 99.6);
}
END_TEST

START_TEST(sprintf_float_negative_zero) {
  ASSERT_SPRINTF("%.2f", -0.0);
  ASSERT_SPRINTF("%.2f", -0.0001);
  ASSERT_SPRINTF("%+.2f", -0.0);
  ASSERT_SPRINTF("% .2f", -0.0);
  ASSERT_SPRINTF("%-10.2f", -0.0);
}
END_TEST

START_TEST(sprintf_float_half_cases) {
  ASSERT_SPRINTF("%.0f", 0.499999);
  ASSERT_SPRINTF("%.0f", 0.5);
  ASSERT_SPRINTF("%.0f", 0.500001);
  ASSERT_SPRINTF("%.1f", 2.05);
  ASSERT_SPRINTF("%.1f", 2.15);
}
END_TEST

START_TEST(sprintf_sign_width_precision) {
  ASSERT_SPRINTF("%+8.2f", 3.14);
  ASSERT_SPRINTF("% 8.2f", 3.14);
  ASSERT_SPRINTF("%+8.2f", -3.14);
  ASSERT_SPRINTF("%-+8.2f", 3.14);
  ASSERT_SPRINTF("%+-8.2f", 3.14);
}
END_TEST

START_TEST(sprintf_precision_zero) {
  ASSERT_SPRINTF("%.0d", 0);
  ASSERT_SPRINTF("%.0d", -0);
  ASSERT_SPRINTF("%.0u", 0);
  ASSERT_SPRINTF("%.0f", 0.0);
  ASSERT_SPRINTF("%.0f", -0.0);
  ASSERT_SPRINTF("%+.0f", 0.0);
}
END_TEST

START_TEST(sprintf_string_extreme) {
  ASSERT_SPRINTF("%.3s", "(null)");
  ASSERT_SPRINTF("%.0s", "(null)");
  ASSERT_SPRINTF("%10.3s", "(null)");
  ASSERT_SPRINTF("%-10.3s", "(null)");
}
END_TEST

START_TEST(sprintf_stress) {
  ASSERT_SPRINTF("ID:%+6d | VAL:%-8.2f | STR:%5.3s | %% | %c", -42, 9.999,
                 "HELLO", 'X');

  ASSERT_SPRINTF("%-10s%+8d% 6.1f%%", "TEST", 123, 0.5);

  ASSERT_SPRINTF("[%8.4f][%-5d][%6u][%.2s]", -0.0001, 7, 99U, "OK");
}
END_TEST

Suite *s21_string_suite(void) {
  Suite *s = suite_create("s21_string");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_memory_funcs);
  tcase_add_test(tc, test_copy_cat);
  tcase_add_test(tc, test_search_char);
  tcase_add_test(tc, test_strncmp);
  tcase_add_test(tc, test_spn_pbrk);
  tcase_add_test(tc, test_strlen);
  tcase_add_test(tc, test_strstr);
  tcase_add_test(tc, test_strtok);
  tcase_add_test(tc, test_strerror);

  tcase_add_test(tc, sprintf_persent_and_text);
  tcase_add_test(tc, sprintf_char);
  tcase_add_test(tc, sprintf_string);
  tcase_add_test(tc, sprintf_signed_int);
  tcase_add_test(tc, sprintf_unsigned_int);
  tcase_add_test(tc, sprintf_mixed);
  tcase_add_test(tc, sprintf_float);
  tcase_add_test(tc, sprintf_flag_comitations);
  tcase_add_test(tc, sprintf_edge_situations_precision);
  tcase_add_test(tc, sprintf_length);
  tcase_add_test(tc, test_plus_space_priority);
  tcase_add_test(tc, sprintf_float_rounding_carry);
  tcase_add_test(tc, sprintf_float_negative_zero);
  tcase_add_test(tc, sprintf_float_half_cases);
  tcase_add_test(tc, sprintf_sign_width_precision);
  tcase_add_test(tc, sprintf_precision_zero);
  tcase_add_test(tc, sprintf_string_extreme);
  tcase_add_test(tc, sprintf_stress);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  Suite *s = s21_string_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}