#include <time.h>
#include <stdio.h>
#include <string.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/test_plugin/trace_vuln.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"vuln.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

int main(int argc, char *argv[]){
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/test_plugin/vuln.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":4,\"end_line\":18,\"metrics\":{\"num_params\":2,\"call_count\":6,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");


	char name[0xdf];
        char surname[0x10];
        printf("Enter your name: \n");
        fgets(name, 0xff, stdin);
	printf(name);
        printf("Enter your surname: \n");
        fgets(surname, 0xff+0x50, stdin);
        printf("Welcome!\n");

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"main\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/test_plugin/vuln.c\",\"line\":16,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
	return 0;

}

