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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_nested.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"nested.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}


REFTRACK_PROLOG(A);

struct REFTRACK_CUSTOM(A) A {
    int ia;
};

REFTRACK_EPILOG_WITH_DTOR(A);

typedef struct A A;

REFTRACK_PROLOG(B);

struct REFTRACK_CUSTOM(B) B{
    A *ap;
    int ib;
};

REFTRACK_EPILOG_WITH_DTOR(B);

typedef struct B B;

void A_destroy(A *const p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"function\":\"A_destroy\",\"func_id\":3,\"num_params\":1,\"start_line\":30,\"end_line\":33,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct A *const\",\"is_pointer\":true,\"is_const\":true}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"A_destroy\",\"func_id\":3,\"name\":\"p\",\"type\":\"struct A *const\",\"is_pointer\":true,\"is_const\":true}");
    printf("A_destroy(A *const)\n");
    fflush(stdout);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"A_destroy\",\"func_id\":3}");
}

void B_destroy(B *const p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"function\":\"B_destroy\",\"func_id\":2,\"num_params\":1,\"start_line\":35,\"end_line\":39,\"metrics\":{\"num_params\":1,\"call_count\":3,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct B *const\",\"is_pointer\":true,\"is_const\":true}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"B_destroy\",\"func_id\":2,\"name\":\"p\",\"type\":\"struct B *const\",\"is_pointer\":true,\"is_const\":true}");
    printf("B_destroy(B *const)\n");
    fflush(stdout);
    A_removeref(p->ap);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"B_destroy\",\"func_id\":2}");
}

B *fill(B *p, int i){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"function\":\"fill\",\"func_id\":1,\"num_params\":2,\"start_line\":41,\"end_line\":52,\"metrics\":{\"num_params\":2,\"call_count\":3,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct B *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"i\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"fill\",\"func_id\":1,\"name\":\"p\",\"type\":\"struct B *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"fill\",\"func_id\":1,\"name\":\"i\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    
	if (i & 1){
        p->ap = A_create();
        p->ap->ia = 0xff;
        printf("fill:RC:%d\n", REFTRACK_COUNT(p));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"fill\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"line\":47,\"ret_type\":\"struct B *\",\"expr\":\"p\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"fill\",\"func_id\":1}");
		return p;
    }
	else{
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"fill\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"line\":50,\"ret_type\":\"struct B *\",\"expr\":\"NULL\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"fill\",\"func_id\":1}");
		return NULL;
    }
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/nested.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":54,\"end_line\":64,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    B *rp = B_create();
    memset(rp, 0,sizeof(B));
    rp->ib = 0xab;
    B *fp = fill(rp, 1);
    fill(rp, 2);
    
    printf("main:RC:B:%d,A:%d %x\n", REFTRACK_COUNT(fp), REFTRACK_COUNT(fp->ap), fp->ap->ia);
    
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
