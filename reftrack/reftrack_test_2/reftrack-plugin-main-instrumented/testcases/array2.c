#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REFTRACK_DEBUG
#include "hrcmm.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_array2.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"array2.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}


REFTRACK_STRUCT(S) { int im; };

REFTRACK_EPILOG(S);

typedef struct S S;

void set_S(S *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/array2.c\",\"function\":\"set_S\",\"func_id\":1,\"num_params\":1,\"start_line\":15,\"end_line\":17,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"S *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"set_S\",\"func_id\":1,\"name\":\"p\",\"type\":\"S *\",\"is_pointer\":true,\"is_const\":false}");
    p->im=0x1234;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"set_S\",\"func_id\":1}");
}


int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/array2.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":20,\"end_line\":35,\"metrics\":{\"num_params\":2,\"call_count\":7,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");

    S *array[] = {S_create(), S_create(), S_create()};

    for(int i = 0; i < sizeof(array)/sizeof(array[0]); i++){
        S_addref(array[i]);
        set_S(array[i]);
        printf("Object initialized\n");
    }

    atexit(print_mem_stats);

    for(int i = 0; i < sizeof(array)/sizeof(array[0]); i++){
        S_removeref(array[i]);
    }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
