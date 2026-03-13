#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REFTRACK_TRACE

#include "hrcmm.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_simple.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"simple.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

typedef struct REFTRACK  {
    int im;
} R;


R *srp;


int main(int argc, char *argv[]) {
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/simple.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":17,\"end_line\":28,\"metrics\":{\"num_params\":2,\"call_count\":4,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    R *rp = rc_malloc(sizeof(R));
    rp->im = 0xab;
    R  *dummy = rc_malloc(sizeof(R));
    R  *ptr_unused = NULL, **pp = NULL;
    ptr_unused = rp;
    pp = &rp;
    *pp = dummy;
    rp->im = 0xab;
    printf("main:RC:%d %x\n", REFTRACK_COUNT(rp), rp->im);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
