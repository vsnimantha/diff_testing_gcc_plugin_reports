#include <time.h>
// Copyright (C) 2021 Christophe Bedard
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/trace_test_utils.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"test_utils.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

#define TESTS_START() printf("Starting tests...\n")
#define TESTS_END() printf("All tests passed!\n")
#define TEST_START() printf("\t%s\n", __FUNCTION__)

void test_strdup_()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"function\":\"test_strdup_\",\"func_id\":4,\"num_params\":0,\"start_line\":26,\"end_line\":36,\"metrics\":{\"num_params\":0,\"call_count\":8,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  TEST_START();

  const char * str = "mystring";
  char * str2 = strdup_(str);
  assert(str2 != str);
  assert(0 == strcmp(str, str2));
  assert(NULL != str2);
  free(str2);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test_strdup_\",\"func_id\":4}");
}

void test_count()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"function\":\"test_count\",\"func_id\":3,\"num_params\":0,\"start_line\":38,\"end_line\":45,\"metrics\":{\"num_params\":0,\"call_count\":10,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  TEST_START();

  assert(1u == count("abc", "a"));
  assert(3u == count("abaac", "a"));
  assert(0u == count("", "a"));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test_count\",\"func_id\":3}");
}

void test_string_list()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"function\":\"test_string_list\",\"func_id\":2,\"num_params\":0,\"start_line\":47,\"end_line\":101,\"metrics\":{\"num_params\":0,\"call_count\":43,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  TEST_START();

  const char * sep = ",";
  {
    char str[] = "some,random,list";
    struct string_list list = {};
    split_str(str, sep, &list);
    assert(3u == list.data_len);
    assert(3u == list.len);
    assert(0 == strcmp("some", list.data[0]));
    assert(0 == strcmp("random", list.data[1]));
    assert(0 == strcmp("list", list.data[2]));
    free_list(&list);
    assert(0u == list.data_len);
    assert(0u == list.len);
  }

  {
    char str[] = "some,random,list,";
    struct string_list list = {};
    split_str(str, sep, &list);
    assert(4u == list.data_len);
    assert(3u == list.len);
    assert(0 == strcmp("some", list.data[0]));
    assert(0 == strcmp("random", list.data[1]));
    free_list(&list);
    assert(0u == list.data_len);
    assert(0u == list.len);
  }

  {
    char str[] = "abc";
    struct string_list list = {};
    split_str(str, sep, &list);
    assert(1u == list.data_len);
    assert(1u == list.len);
    assert(0 == strcmp("abc", list.data[0]));
    free_list(&list);
    assert(0u == list.data_len);
    assert(0u == list.len);
  }

  {
    char str[] = "";
    struct string_list list = {};
    split_str(str, sep, &list);
    assert(0u == list.data_len);
    assert(0u == list.len);
    free_list(&list);
    assert(0u == list.data_len);
    assert(0u == list.len);
  }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test_string_list\",\"func_id\":2}");
}

void test_list_strstr()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"function\":\"test_list_strstr\",\"func_id\":1,\"num_params\":0,\"start_line\":103,\"end_line\":138,\"metrics\":{\"num_params\":0,\"call_count\":21,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  TEST_START();

  const char * sep = ",";
  {
    char str[] = "/path/to/,random_,mysuperfunction";
    const char * test1 = "/path/to/something";
    const char * test2 = "random_function";
    const char * test3 = "mysuperawesomefunction";
    struct string_list list = {};
    split_str(str, sep, &list);
    const char * match1 = list_strstr(&list, test1);
    assert(NULL != match1);
    assert(0 == strcmp(match1, "/path/to/"));
    const char * match2 = list_strstr(&list, test2);
    assert(NULL != match2);
    assert(0 == strcmp(match2, "random_"));
    const char * match3 = list_strstr(&list, test3);
    assert(NULL == match3);
    free_list(&list);
  }

  {
    char str[] = "";
    const char * test1 = "/path/to/something";
    const char * test2 = "";
    struct string_list list = {};
    split_str(str, sep, &list);
    const char * match1 = list_strstr(&list, test1);
    assert(NULL == match1);
    const char * match2 = list_strstr(&list, test2);
    assert(NULL == match2);
    free_list(&list);
  }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test_list_strstr\",\"func_id\":1}");
}

int main()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":0,\"start_line\":140,\"end_line\":151,\"metrics\":{\"num_params\":0,\"call_count\":6,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  TESTS_START();

  test_strdup_();
  test_count();
  test_string_list();
  test_list_strstr();

  TESTS_END();
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"main\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/utils/test_utils.c\",\"line\":150,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
  return 0;
}
