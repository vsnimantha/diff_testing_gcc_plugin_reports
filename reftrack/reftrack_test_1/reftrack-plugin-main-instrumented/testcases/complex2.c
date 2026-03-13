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
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_complex2.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"complex2.c\",\"msg\":%s}\n",
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
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex2.c\",\"function\":\"R_destroy\",\"func_id\":5,\"num_params\":1,\"start_line\":17,\"end_line\":20,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct R *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"R_destroy\",\"func_id\":5,\"name\":\"p\",\"type\":\"struct R *\",\"is_pointer\":true,\"is_const\":false}");
    if (p)
        printf("R_destroy():%p\n", p);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"R_destroy\",\"func_id\":5}");
}

REFTRACK_STRUCT(P){
    struct R *pr;
};

REFTRACK_EPILOG_WITH_DTOR(P);

typedef struct P P;

REFTRACK_DESTRUCTOR_FN void P_destroy(P *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex2.c\",\"function\":\"P_destroy\",\"func_id\":3,\"num_params\":1,\"start_line\":30,\"end_line\":35,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"P_destroy\",\"func_id\":3,\"name\":\"p\",\"type\":\"P *\",\"is_pointer\":true,\"is_const\":false}");
    if (p){
        p->pr = NULL;
        printf("P_destroy():%p\n", p);
    }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"P_destroy\",\"func_id\":3}");
}

typedef struct {
    R *br;
    P *bp;
} B;

typedef struct {
    B *abv[2];
    B ab;
} A;


R *gpr;

void foo(int flag){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex2.c\",\"function\":\"foo\",\"func_id\":2,\"num_params\":1,\"start_line\":50,\"end_line\":88,\"metrics\":{\"num_params\":1,\"call_count\":7,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"flag\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"foo\",\"func_id\":2,\"name\":\"flag\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    static B sb;
    static R *spr;

    B b;
    R *arp[10];
    R ar[20];
    R *rau[0];
    R **rap;

    int ia[10];

    CLEAR(sb);
    CLEAR(b);

    if (!flag){
        printf("Clear statics\n");
        spr = NULL;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"foo\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex2.c\",\"line\":68,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"foo\",\"func_id\":2}");
        return;
    }

    {
        A a;
        memset(&a, 0, sizeof(A));

        a.abv[0] = &b;
        a.abv[1] = &sb;
        spr = R_create();
        gpr = spr;
        a.abv[0]->br = NULL;

        a.abv[1]->br = a.abv[0]->br;
        a.ab.bp = P_create();
        a.ab.bp->pr = spr;
        a.ab.bp->pr->im++;
        printf("BLOCK END\n");
    }

    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"foo\",\"func_id\":2}");
}

__attribute__((destructor)) static void cleanup(){

    printf("cleanup\n");
    foo(0);
    gpr = NULL;
    print_mem_stats();
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/complex2.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":98,\"end_line\":101,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    foo(1);

    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
