#include <time.h>
#include <stdio.h>
#define REFTRACK_DEBUG
#define REFTRACK_TRACE

#include "hrcmm.h"
#include "uthash.h"
#include <assert.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/trace_hash_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"hash_test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

typedef int key_t;

REFTRACK_STRUCT(my_struct){
    key_t key;
    float value;
    UT_hash_handle hh;
};
REFTRACK_EPILOG(my_struct);

typedef struct my_struct my_struct;

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/testcases/hash_test.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":19,\"end_line\":69,\"metrics\":{\"num_params\":2,\"call_count\":27,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":true,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
    my_struct *ht = NULL;

    int entries = 30;
    
    if (argc == 2){
        entries=atoi(argv[1]);
    }
    
    atexit(print_mem_stats);
    
    for(int i = 0; i < entries; i++){
        my_struct *vo = my_struct_create();
        vo->key = i+1;
        vo->value = vo->key*0.5;
        // need to add reference manually as uthash doesn't preserve the type of my_struct internally.
        my_struct_addref(vo); 
        HASH_ADD_INT(ht, key, vo);
    }

    assert(HASH_COUNT(ht) == entries);
    
    my_struct *tmp = NULL;
    key_t search_key = entries;
    
    printf("Before HASH_FIND_INT\n");
    HASH_FIND_INT(ht, &search_key, tmp);
    printf("After HASH_FIND_INT\n");
    
    if (tmp){
        assert(tmp->value == (entries)*0.5);
    }
    else{
        printf("Entry not found:%d\n", search_key);
    }
      
    // delete all entries

    my_struct *cur_elem = NULL, *it_tmp = NULL;
    
    HASH_ITER(hh, ht, cur_elem, it_tmp){
        printf("---BEGIN LOOP BODY---\n");
        printf("Deleting entry:|%p|:key:%d, RC:%d\n", cur_elem, cur_elem->key, REFTRACK_COUNT(cur_elem));
        HASH_DEL(ht, cur_elem);
        my_struct_removeref(cur_elem);
        printf("---END LOOP BODY---\n");
    }
   

    
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
}

