#include <time.h>
#include <stdio.h>
int f(int x)
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/trace_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/test.c\",\"function\":\"f\",\"func_id\":0,\"num_params\":1,\"start_line\":1,\"end_line\":4,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"x\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"f\",\"func_id\":0,\"name\":\"x\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] x = %d\n", x);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"f\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/test.c\",\"line\":3,\"ret_type\":\"int\",\"expr\":\"x + 1\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (x + 1));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"f\",\"func_id\":0}");
    return x + 1;
}
