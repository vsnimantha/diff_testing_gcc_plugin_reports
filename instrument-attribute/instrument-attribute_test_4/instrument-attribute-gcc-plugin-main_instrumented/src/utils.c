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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/trace_utils.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"utils.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

char * strdup_(const char * str)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"function\":\"strdup_\",\"func_id\":4,\"num_params\":1,\"start_line\":22,\"end_line\":28,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"str\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"strdup_\",\"func_id\":4,\"name\":\"str\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
  size_t len = strlen(str) + 1;
  char * dup_str = new char[len];
  memcpy(dup_str, str, len);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"strdup_\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"line\":27,\"ret_type\":\"char *\",\"expr\":\"dup_str\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"strdup_\",\"func_id\":4}");
  return dup_str;
}

size_t count(const char * str, const char * character)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"function\":\"count\",\"func_id\":3,\"num_params\":2,\"start_line\":30,\"end_line\":41,\"metrics\":{\"num_params\":2,\"call_count\":3,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"str\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"character\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"count\",\"func_id\":3,\"name\":\"str\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"count\",\"func_id\":3,\"name\":\"character\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
  assert(1 == strlen(character));
  const char * p = str;
  size_t count = 0;

  do {
    if(*p == *character) count++;
  } while (*(p++));

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"count\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"line\":40,\"ret_type\":\"size_t\",\"expr\":\"count\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"count\",\"func_id\":3}");
  return count;
}

void split_str(char * str, const char * sep, struct string_list * list)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"function\":\"split_str\",\"func_id\":2,\"num_params\":3,\"start_line\":43,\"end_line\":69,\"metrics\":{\"num_params\":3,\"call_count\":9,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"str\",\"type\":\"char *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"sep\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"split_str\",\"func_id\":2,\"name\":\"str\",\"type\":\"char *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"split_str\",\"func_id\":2,\"name\":\"sep\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"split_str\",\"func_id\":2,\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}");
  assert(0 < strlen(sep));
  if (0 == strlen(str)) {
    list->len = 0u;
    list->data_len = 0u;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"split_str\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"line\":49,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"split_str\",\"func_id\":2}");
    return;
  }

  // Could be wrong, e.g. "a,,b"
  size_t data_len = count(str, sep) + 1;
  list->data = new char*[data_len];

  char * rest = NULL;
  char * token;
  size_t len = 0;
  for (token = strtok_r(str, sep, &rest); NULL != token; token = strtok_r(NULL, sep, &rest)) {
    // Only use it if it's not empty
    if (strlen(token) > 0) {
      list->data[len] = strdup_(token);
      len++;
    }
  }

  list->len = len;
  list->data_len = data_len;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"split_str\",\"func_id\":2}");
}

void free_list(struct string_list * list)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"function\":\"free_list\",\"func_id\":1,\"num_params\":1,\"start_line\":71,\"end_line\":79,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"free_list\",\"func_id\":1,\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}");
  for (size_t i = 0; i < list->data_len; i++) {
    free(list->data[i]);
  }
  free(list->data);
  list->len = 0;
  list->data_len = 0;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"free_list\",\"func_id\":1}");
}

const char * list_strstr(struct string_list * list, const char * str1)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"function\":\"list_strstr\",\"func_id\":0,\"num_params\":2,\"start_line\":81,\"end_line\":89,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"str1\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"list_strstr\",\"func_id\":0,\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"list_strstr\",\"func_id\":0,\"name\":\"str1\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
  for (size_t i = 0; i < list->len; i++) {
    if (NULL != strstr(str1, list->data[i])) {
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"list_strstr\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"line\":85,\"ret_type\":\"const char *\",\"expr\":\"list->data[i]\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"list_strstr\",\"func_id\":0}");
      return list->data[i];
    }
  }
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"list_strstr\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/utils.c\",\"line\":88,\"ret_type\":\"const char *\",\"expr\":\"NULL\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"list_strstr\",\"func_id\":0}");
  return NULL;
}
