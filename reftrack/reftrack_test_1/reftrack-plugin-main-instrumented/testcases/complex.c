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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_complex.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"complex.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}


REFTRACK_PROLOG(R)

struct REFTRACK_CUSTOM(R) R {
    int im;
};

REFTRACK_EPILOG(R)


typedef struct R R;

typedef R R2;

struct C {
    struct R *rp;
};

//R *srp;

R *foo(R2 *p, int k){
    p->im++;
    
    
    R *obp;
    {
        R *p1, *p2 = NULL, *pa[8];
        p1=p;
        obp = p;
        p1->im=0xab;
        p1 = p1;
        // pa[3] = p1;
    }
    printf("foo:RC:%d\n", REFTRACK_COUNT(p));
    return p;
}

int even(R *p, int i){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex.c\",\"function\":\"even\",\"func_id\":1,\"num_params\":2,\"start_line\":45,\"end_line\":56,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"i\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"even\",\"func_id\":1,\"name\":\"p\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] p = %d\n", p);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"even\",\"func_id\":1,\"name\":\"i\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] i = %d\n", i);
    R *rp = NULL;
    
	if (i & 1){
        rp = p;
        //srp = rp;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"even\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex.c\",\"line\":51,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"even\",\"func_id\":1}");
		return 0;
    }
	else{
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"even\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex.c\",\"line\":54,\"ret_type\":\"int\",\"expr\":\"1\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (1));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"even\",\"func_id\":1}");
		return 1;
    }
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":58,\"end_line\":69,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    
    R *rp = (R*)malloc(sizeof(R)), *dummy = (R*)malloc(sizeof(R)), *unused = NULL;
    memset(rp, 0,sizeof(R));
    rp->im = 0xab;
    even(rp, 1);
    R *rv = foo(rp, 10);
    int offset = 1, offset2 = -1;
    R *rv2 = offset+rp+offset2;
    printf("main:RC:%d %x %x\n", REFTRACK_COUNT(rv), rv->im, rv2->im);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
