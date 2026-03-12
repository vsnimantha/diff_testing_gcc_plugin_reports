#include <time.h>
// Copyright (c) 2021 Christophe Bedard
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
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/trace_a_file.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"a_file.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

void myawesomelib_function_a()
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/a_file.c","function":"myawesomelib_function_a","func_id":2,"num_params":0,"start_line":25,"end_line":28,"metrics":{"num_params":0,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
  printf("instrumented via include-function-list with 'myawesomelib_' prefix\n");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"myawesomelib_function_a","func_id":2})JSON");
}

void myawesomelib_function_b()
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/a_file.c","function":"myawesomelib_function_b","func_id":1,"num_params":0,"start_line":30,"end_line":33,"metrics":{"num_params":0,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
  printf("instrumented via include-function-list with 'myawesomelib_' prefix\n");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"myawesomelib_function_b","func_id":1})JSON");
}

void mynotawesomelib_function_c()
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/a_file.c","function":"mynotawesomelib_function_c","func_id":0,"num_params":0,"start_line":35,"end_line":38,"metrics":{"num_params":0,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
  printf("not instrumented\n");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"mynotawesomelib_function_c","func_id":0})JSON");
}
