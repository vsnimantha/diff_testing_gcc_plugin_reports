#include <time.h>
#define REFTRACK_TRACE
#define REFTRACK_DEBUG
#include "hrcmm.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_block_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"block_test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

#define CLEAR(x) memset(&(x), 0, sizeof(x))

REFTRACK_STRUCT(R){
    int im;
};

REFTRACK_EPILOG_WITH_DTOR(R);
typedef struct R R;

void R_destroy(struct R *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/block_test.c\",\"function\":\"R_destroy\",\"func_id\":4,\"num_params\":1,\"start_line\":17,\"end_line\":20,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct R *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"R_destroy\",\"func_id\":4,\"name\":\"p\",\"type\":\"struct R *\",\"is_pointer\":true,\"is_const\":false}");
    if (p)
        printf("R_destroy():%p\n", p);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"R_destroy\",\"func_id\":4}");
}

REFTRACK_STRUCT(P){
    struct R *pr;
};

REFTRACK_EPILOG_WITH_DTOR(P);

typedef struct P P;

typedef struct {
    P *pp;
} S;

REFTRACK_DESTRUCTOR_FN void P_destroy(P *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/block_test.c\",\"function\":\"P_destroy\",\"func_id\":2,\"num_params\":1,\"start_line\":34,\"end_line\":39,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"P_destroy\",\"func_id\":2,\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}");
    if (p){
        p->pr = NULL;
        printf("P_destroy():%p\n", p);
    }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"P_destroy\",\"func_id\":2}");
}


static inline void foo(P *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/block_test.c\",\"function\":\"foo\",\"func_id\":1,\"num_params\":1,\"start_line\":42,\"end_line\":62,\"metrics\":{\"num_params\":1,\"call_count\":8,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"foo\",\"func_id\":1,\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}");
    printf("foo() ENTER\n");
    P *a = p;
    {
        printf("{A} ENTER\n");
        S s;
        CLEAR(s);
        s.pp = a;
        {
            printf("{B} ENTER\n");
            
            P *c = s.pp;
            c->pr = R_create();
            c->pr->im = 0xbeef;
            
            printf("{B} EXIT\n");
        }
        printf("{A} EXIT\n");
    }
    printf("foo() EXIT\n");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"foo\",\"func_id\":1}");
}


int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/block_test.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":65,\"end_line\":72,\"metrics\":{\"num_params\":2,\"call_count\":3,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    P *p = P_create();
    
    foo(p);
    
    printf("%x\n", p->pr->im);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
