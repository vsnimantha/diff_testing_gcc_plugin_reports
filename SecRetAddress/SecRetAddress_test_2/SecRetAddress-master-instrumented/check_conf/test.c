#include <time.h>
#include <stdio.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/trace_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

int main(){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/test.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":0,\"start_line\":3,\"end_line\":7,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	printf("Hello World!\n");
	
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"main\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/test.c\",\"line\":6,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
	return 0;
}
