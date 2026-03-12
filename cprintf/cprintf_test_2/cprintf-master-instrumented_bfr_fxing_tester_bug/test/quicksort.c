#include <time.h>
#include <stdio.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/trace_quicksort.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"quicksort.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

/* not a macro, not static, not inline just for test */
void swap(int *a, int *b)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","function":"swap","func_id":3,"num_params":2,"start_line":4,"end_line":9,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"a","type":"int *","is_pointer":true,"is_const":false},{"name":"b","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"swap","func_id":3,"name":"a","type":"int *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] a = %d\n", a);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"swap","func_id":3,"name":"b","type":"int *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] b = %d\n", b);
	int tmp = *a;
	*a = *b;
	*b = tmp;
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"swap","func_id":3})JSON");
}

int partition(int *arr, int l, int h)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","function":"partition","func_id":2,"num_params":3,"start_line":11,"end_line":25,"metrics":{"num_params":3,"call_count":2,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"arr","type":"int *","is_pointer":true,"is_const":false},{"name":"l","type":"int","is_pointer":false,"is_const":false},{"name":"h","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"partition","func_id":2,"name":"arr","type":"int *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] arr = %d\n", arr);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"partition","func_id":2,"name":"l","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] l = %d\n", l);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"partition","func_id":2,"name":"h","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] h = %d\n", h);
	int p = arr[h]; /* pivot element */
	int i = (l - 1);
	int j;

	for (j = l; j <= h - 1; j++) {
		if (arr[j] <= p) {
			i++; /* index of the lastest small element */
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[h]); /* first high element with pivot */
    log_with_timestamp(R"JSON({"event":"RETURN","function":"partition","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","line":24,"ret_type":"int","expr":"(i + 1)"})JSON");
    printf("[LOG][RETURN_VALUE] %d\n", ((i + 1)));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"partition","func_id":2})JSON");
	return (i + 1);
}

void quick_sort(int *m, int l, int h)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","function":"quick_sort","func_id":1,"num_params":3,"start_line":27,"end_line":35,"metrics":{"num_params":3,"call_count":3,"has_recursion":true,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"m","type":"int *","is_pointer":true,"is_const":false},{"name":"l","type":"int","is_pointer":false,"is_const":false},{"name":"h","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"quick_sort","func_id":1,"name":"m","type":"int *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] m = %d\n", m);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"quick_sort","func_id":1,"name":"l","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] l = %d\n", l);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"quick_sort","func_id":1,"name":"h","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] h = %d\n", h);
	if (l < h) {
		int p = partition(m, l, h);

		quick_sort(m, l, p - 1);
		quick_sort(m, p + 1, h);
	}
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"quick_sort","func_id":1})JSON");
}

int main(int argc, char **argv)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","function":"main","func_id":0,"num_params":2,"start_line":37,"end_line":54,"metrics":{"num_params":2,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"argc","type":"int","is_pointer":false,"is_const":false},{"name":"argv","type":"char **","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"main","func_id":0,"name":"argc","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"main","func_id":0,"name":"argv","type":"char **","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] argv = %s\n", argv);
	int array[] = {6, 2, 8, 3, 9, 4, 0, 3, 1, 6};
	size_t array_sz = sizeof(array)/sizeof(int);
	int i;

	puts("Before:");
	for (i = 0; i < array_sz; i++)
		printf("%d ", array[i]);

	puts("\nAfter:");
	quick_sort(array, 0, array_sz - 1);
	for (i = 0; i < array_sz; i++)
		printf("%d ", array[i]);
	puts("");

    log_with_timestamp(R"JSON({"event":"RETURN","function":"main","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/quicksort.c","line":53,"ret_type":"int","expr":"0"})JSON");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"main","func_id":0})JSON");
	return 0;
}
