#include <check.h>
#include "calc.h"

START_TEST(calc_test_plus) {
    char *expr = "1.25+3.45";
    char *expected = "4.7000000";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_minus) {
    char *expr = "0.003-15.34";
    char *expected = "-15.3370000";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_mult) {
    char *expr = "0.25*5";
    char *expected = "1.2500000";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_div) {
    char *expr = "0.2/5";
    char *expected = "0.0400000";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_mod) {
    char *expr = "60mod3.5";
    char *expected = "0.5000000";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_pow) {
    char *expr = "5^3.45";
    char *expected = "257.8971335";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_brackets) {
    char *expr = "(5.23+1.25)*(0.25+0.001)";
    char *expected = "1.6264800";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_sin) {
    char *expr = "sin(0.3*0.6)";
    char *expected = "0.1790296";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_cos) {
    char *expr = "cos(0.6-0.1+0.2/0.5)";
    char *expected = "0.6216100";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_tan) {
    char *expr = "tan(0.8/2)";
    char *expected = "0.4227932";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_sqrt) {
    char *expr = "sqrt(97.3456)";
    char *expected = "9.8663874";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_ln) {
    char *expr = "ln(2.5+99)";
    char *expected = "4.6200588";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_log) {
    char *expr = "log(1.8+256.34)";
    char *expected = "2.4118553";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_asin) {
    char *expr = "asin(0.34)";
    char *expected = "0.3469169";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_acos) {
    char *expr = "acos(-0.83)";
    char *expected = "2.5499040";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_atan) {
    char *expr = "atan(-0.669)";
    char *expected = "-0.5896162";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_1) {
    char *expr = "1-1";
    char *expected = "0";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_2) {
    char *expr = "1-2-3";
    char *expected = "-4";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_3) {
    char *expr = "4*(-5)+2.01/sin(0.01)";
    char *expected = "181.0033500";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_4) {
    char *expr = "sin(0.1)";
    char *expected = "0.0998334";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_5) {
    char *expr = "cos(sin(tan(-0.005)))";
    char *expected = "0.9999875";
    char *actual = calculate(expr);
    ck_assert_str_eq(expected, actual);
    free(actual);
}
END_TEST

START_TEST(calc_test_null) {
    char *expr = "125/0";
    char *actual = calculate(expr);
    ck_assert_ptr_null(actual);
    free(actual);
}
END_TEST

int main(void) {
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, calc_test_plus);
    tcase_add_test(tc1_1, calc_test_minus);
    tcase_add_test(tc1_1, calc_test_mult);
    tcase_add_test(tc1_1, calc_test_div);
    tcase_add_test(tc1_1, calc_test_mod);
    tcase_add_test(tc1_1, calc_test_pow);
    tcase_add_test(tc1_1, calc_test_brackets);
    tcase_add_test(tc1_1, calc_test_sin);
    tcase_add_test(tc1_1, calc_test_cos);
    tcase_add_test(tc1_1, calc_test_tan);
    tcase_add_test(tc1_1, calc_test_sqrt);
    tcase_add_test(tc1_1, calc_test_ln);
    tcase_add_test(tc1_1, calc_test_log);
    tcase_add_test(tc1_1, calc_test_asin);
    tcase_add_test(tc1_1, calc_test_acos);
    tcase_add_test(tc1_1, calc_test_atan);
    tcase_add_test(tc1_1, calc_test_1);
    tcase_add_test(tc1_1, calc_test_2);
    tcase_add_test(tc1_1, calc_test_3);
    tcase_add_test(tc1_1, calc_test_4);
    tcase_add_test(tc1_1, calc_test_5);
    tcase_add_test(tc1_1, calc_test_null);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
