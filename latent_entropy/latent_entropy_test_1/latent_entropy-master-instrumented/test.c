#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/trace_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

#define __latent_entropy __attribute__((latent_entropy))

volatile unsigned long latent_entropy __latent_entropy;

void __latent_entropy test1(int argc, char *argv[])
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/test.c\",\"function\":\"test1\",\"func_id\":2,\"num_params\":2,\"start_line\":9,\"end_line\":12,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"test1\",\"func_id\":2,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"test1\",\"func_id\":2,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
	printf("%u %s\n", argc, *argv);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test1\",\"func_id\":2}");
}

void __latent_entropy test2(int argc, char *argv[])
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/test.c\",\"function\":\"test2\",\"func_id\":1,\"num_params\":2,\"start_line\":14,\"end_line\":25,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"test2\",\"func_id\":1,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"test2\",\"func_id\":1,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
	int a;

	a = argc * argc;
	if (argc == 10)
		printf("%u %s\n", a, *argv);
	else if (argc == 8) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"test2\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/test.c\",\"line\":22,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test2\",\"func_id\":1}");
	    return;
	}
	else
		printf("AAAAAAAA %u %s\n", a, *argv);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"test2\",\"func_id\":1}");
}

int main(int argc, char *argv[])
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/test.c\",\"function\":\"main\",\"func_id\":0,\"num_params\":2,\"start_line\":27,\"end_line\":32,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argc\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"main\",\"func_id\":0,\"name\":\"argv\",\"type\":\"char *[]\",\"is_pointer\":false,\"is_const\":false}");
	test1(argc, argv);
	test2(argc, argv);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"main\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/latent_entropy-master-instrumented/test.c\",\"line\":31,\"ret_type\":\"int\",\"expr\":\"argc\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (argc));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"main\",\"func_id\":0}");
	return argc;
}
