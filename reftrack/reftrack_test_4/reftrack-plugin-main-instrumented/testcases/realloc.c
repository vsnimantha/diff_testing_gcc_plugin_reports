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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_realloc.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"realloc.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}


REFTRACK_PROLOG(R_)

struct REFTRACK_CUSTOM(R_) R_ {
    int im;
};

REFTRACK_EPILOG(R_);


typedef struct R_ R;

R *srp;

void test_realloc(){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/realloc.c\",\"function\":\"test_realloc\",\"func_id\":1,\"num_params\":0,\"start_line\":22,\"end_line\":30,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	R *p = rc_malloc(sizeof(R));
	R *q = rc_realloc(p, sizeof(R)*2);
	if (q){
		p = q;
		q->im = 0xac;
		printf("%d\n", q->im);
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test_realloc\",\"func_id\":1}");
}

int main(int argc, char *argv[]) {
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/realloc.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":32,\"end_line\":44,\"metrics\":{\"num_params\":2,\"call_count\":4,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    test_realloc();

    R *rp = rc_malloc(sizeof(R));
    printf("Resizing object:%p\n", rp);
    rp = rc_realloc(rp, sizeof(R)*1000);

    memset(rp, 0,sizeof(R));
    rp->im = 0xab;
    printf("main:RC:%d %x\n", REFTRACK_COUNT(rp), rp->im);

    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
