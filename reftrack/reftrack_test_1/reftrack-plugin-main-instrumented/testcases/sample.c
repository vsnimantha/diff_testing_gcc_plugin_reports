#include <time.h>
#include <stdio.h>
#define REFTRACK_DEBUG
#include <string.h>
#include "hrcmm.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_sample.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"sample.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}
/*
  REFTRACK_STRUCT macro defines the following declarations

  struct foo;
  void foo_addref(const struct foo *const);
  void foo_removeref(const struct foo *const);
*/

REFTRACK_STRUCT(foo){
    int bar;
};

/*
  REFTRACK_EPILOG_WITH_DTOR macro calls foo_destroy when reference count is zero.
  This is optional, if there is no special cleanup to be done, use REFTRACK_EPILOG. 
*/
REFTRACK_EPILOG_WITH_DTOR(foo);

// This function is called when the reference count for object pointed to by p is zero
void foo_destroy(struct foo *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/sample.c\",\"function\":\"foo_destroy\",\"func_id\":2,\"num_params\":1,\"start_line\":23,\"end_line\":26,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"struct foo *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"foo_destroy\",\"func_id\":2,\"name\":\"p\",\"type\":\"struct foo *\",\"is_pointer\":true,\"is_const\":false}");
    if (p)
        printf("foo destroyed:%p\n", p);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"foo_destroy\",\"func_id\":2}");
}

typedef struct foo foo;

// statements commented out are injected by the plugin
     
void baz(foo *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/sample.c\",\"function\":\"baz\",\"func_id\":1,\"num_params\":1,\"start_line\":32,\"end_line\":35,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"foo *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"baz\",\"func_id\":1,\"name\":\"p\",\"type\":\"foo *\",\"is_pointer\":true,\"is_const\":false}");
    printf("%d\n", p->bar);
    // foo_removeref(p);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"baz\",\"func_id\":1}");
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/sample.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":37,\"end_line\":50,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    foo *p = rc_malloc(sizeof(foo)); // rc_malloc is a default wrapper provided
    // foo_addref(p); 
    p->bar = 123;
    
    // foo_addref(p); 
    foo *q = p;

    // foo_addref(q); 
    baz(q);

    // foo_removeref(p); 
    // foo_removeref(q); 
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
