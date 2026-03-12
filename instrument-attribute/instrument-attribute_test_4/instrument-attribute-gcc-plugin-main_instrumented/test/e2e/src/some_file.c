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

#include "some_dir/some_header.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/trace_some_file.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"some_file.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

void some_header_instrumented_with_file_list()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/test/e2e/src/some_file.c\",\"function\":\"some_header_instrumented_with_file_list\",\"func_id\":0,\"num_params\":0,\"start_line\":25,\"end_line\":30,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  // Using a partial path to the source file since
  // that's the file that contains the implementation
  printf("instrumented via include-file-list with a 'dir/file_' prefix\n");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"some_header_instrumented_with_file_list\",\"func_id\":0}");
}
