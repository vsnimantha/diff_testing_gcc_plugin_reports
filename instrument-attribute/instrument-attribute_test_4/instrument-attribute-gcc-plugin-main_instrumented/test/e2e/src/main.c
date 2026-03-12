#include <time.h>
// Copyright (c) 2019-2021 Christophe Bedard
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>

#include "a_header.h"
#include "other/other_file.h"
#include "some_dir/some_header.h"
#include "some_dir/some_other_header.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/trace_main.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"main.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

void __attribute__((instrument_function)) instrumented_function()
{
  printf("instrumented with attribute\n");
}

void not_instrumented_function()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/main.c\",\"function\":\"not_instrumented_function\",\"func_id\":2,\"num_params\":0,\"start_line\":33,\"end_line\":36,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  printf("not instrumented\n");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"not_instrumented_function\",\"func_id\":2}");
}

void instrumented_with_function_list()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/main.c\",\"function\":\"instrumented_with_function_list\",\"func_id\":1,\"num_params\":0,\"start_line\":38,\"end_line\":41,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  printf("instrumented via include-function-list\n");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"instrumented_with_function_list\",\"func_id\":1}");
}

int __attribute__((instrument_function)) main()
{
  printf("instrumented with attribute\n");

  instrumented_function();
  not_instrumented_function();
  instrumented_with_function_list();

  other_file_instrumented_with_file_list();

  some_header_instrumented_with_file_list();
  some_other_header_instrumented_with_file_list_and_attribute();

  myawesomelib_function_a();
  myawesomelib_function_b();
  mynotawesomelib_function_c();
  return 0;
}
