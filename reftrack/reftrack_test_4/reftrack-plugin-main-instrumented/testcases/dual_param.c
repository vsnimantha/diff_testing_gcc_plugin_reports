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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_dual_param.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"dual_param.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

REFTRACK_STRUCT(R){
    int im;
};

REFTRACK_EPILOG(R);
typedef struct R R;

R *add(R *p, R *q){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/dual_param.c\",\"function\":\"add\",\"func_id\":1,\"num_params\":2,\"start_line\":15,\"end_line\":18,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"R *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"q\",\"type\":\"R *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add\",\"func_id\":1,\"name\":\"p\",\"type\":\"R *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add\",\"func_id\":1,\"name\":\"q\",\"type\":\"R *\",\"is_pointer\":true,\"is_const\":false}");
    p->im += q->im;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"add\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/dual_param.c\",\"line\":17,\"ret_type\":\"R *\",\"expr\":\"q\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"add\",\"func_id\":1}");
    return q;
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/dual_param.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":20,\"end_line\":29,\"metrics\":{\"num_params\":2,\"call_count\":3,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    R *rp = (R*)rc_malloc(sizeof(R));
    memset(rp, 0,sizeof(R));
    rp->im = 2;
    R *tmp = add(rp, rp);

    printf("main:RC:%d %x\n", REFTRACK_COUNT(rp), rp->im);

    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
