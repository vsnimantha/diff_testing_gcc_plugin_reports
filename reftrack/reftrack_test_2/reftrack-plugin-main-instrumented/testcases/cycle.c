#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define REFTRACK_TRACE
#define REFTRACK_DEBUG
#include "hrcmm.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_cycle.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"cycle.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

REFTRACK_STRUCT(node) {
    int value;
    struct node *next;
};

REFTRACK_EPILOG_WITH_DTOR(node);

typedef struct node node;

REFTRACK_DESTRUCTOR_FN void node_destroy(node *p){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/cycle.c\",\"function\":\"node_destroy\",\"func_id\":2,\"num_params\":1,\"start_line\":17,\"end_line\":21,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"p\",\"type\":\"node *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"node_destroy\",\"func_id\":2,\"name\":\"p\",\"type\":\"node *\",\"is_pointer\":true,\"is_const\":false}");
    if (!p) return;	
    printf("value:%d\n", p->value);
    p->next = NULL;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"node_destroy\",\"func_id\":2}");
}

void *list_head;

void create_list(int n){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/cycle.c\",\"function\":\"create_list\",\"func_id\":1,\"num_params\":1,\"start_line\":25,\"end_line\":52,\"metrics\":{\"num_params\":1,\"call_count\":8,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"n\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"create_list\",\"func_id\":1,\"name\":\"n\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    node *head = NULL, *prev = NULL;
    
    for(int i = 0; i < n; i++){
        node *np = node_create();
	if (!np){
		perror("Out of memory\n");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"create_list\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/cycle.c\",\"line\":32,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"create_list\",\"func_id\":1}");
		return;
	}
        np->value = 100*(i+1);

        if (!head){
            head = prev = np;
        }
        else{
	    int a = 1, b = -1;		
            prev->next = np+a+b;
            prev = np;
        }
        printf("Created entry:%d, RC:%d\n", np->value, REFTRACK_COUNT(np));        
    }
    printf("Create a cycle\n");
    prev->next = head; // create a cycle
    printf("RC of head:%d\n", REFTRACK_COUNT(head));
    printf("Break the cycle\n");
    head->next = NULL; // break the cycle
   
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"create_list\",\"func_id\":1}");
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/cycle.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":54,\"end_line\":59,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    atexit(print_mem_stats);
    create_list(5);
   
   
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}
