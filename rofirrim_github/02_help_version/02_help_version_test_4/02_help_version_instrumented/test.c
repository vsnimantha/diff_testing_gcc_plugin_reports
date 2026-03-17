#include <time.h>
#include <stdio.h>

static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/02_help_version_instrumented/trace_test.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"test.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}
// Taken from http://www.geeksforgeeks.org/iterative-quick-sort/

// A utility function to swap two elements
void swap ( int* a, int* b )
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/02_help_version_instrumented/test.c\",\"function\":\"swap\",\"func_id\":2,\"num_params\":2,\"start_line\":5,\"end_line\":10,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"a\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"b\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"swap\",\"func_id\":2,\"name\":\"a\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] a = %d\n", a);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"swap\",\"func_id\":2,\"name\":\"b\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] b = %d\n", b);
    int t = *a;
    *a = *b;
    *b = t;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"swap\",\"func_id\":2}");
}
 
/* This function is same in both iterative and recursive*/
int partition (int arr[], int l, int h)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/02_help_version_instrumented/test.c\",\"function\":\"partition\",\"func_id\":1,\"num_params\":3,\"start_line\":13,\"end_line\":29,\"metrics\":{\"num_params\":3,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"arr\",\"type\":\"int[]\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"l\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"h\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition\",\"func_id\":1,\"name\":\"arr\",\"type\":\"int[]\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] arr = %d\n", arr);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition\",\"func_id\":1,\"name\":\"l\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] l = %d\n", l);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition\",\"func_id\":1,\"name\":\"h\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] h = %d\n", h);
    int x = arr[h];
    int i = (l - 1);

    int j;
    for (j = l; j <= h- 1; j++)
    {
        if (arr[j] <= x)
        {
            i++;
            swap (&arr[i], &arr[j]);
        }
    }
    swap (&arr[i + 1], &arr[h]);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"partition\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/02_help_version_instrumented/test.c\",\"line\":28,\"ret_type\":\"int\",\"expr\":\"(i + 1)\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"partition\",\"func_id\":1}");
    return (i + 1);
}

/* A[] --> Array to be sorted, l  --> Starting index, h  --> Ending index */
void quickSortIterative (int arr[], int l, int h)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/02_help_version_instrumented/test.c\",\"function\":\"quickSortIterative\",\"func_id\":0,\"num_params\":3,\"start_line\":32,\"end_line\":70,\"metrics\":{\"num_params\":3,\"call_count\":1,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"arr\",\"type\":\"int[]\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"l\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"h\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"quickSortIterative\",\"func_id\":0,\"name\":\"arr\",\"type\":\"int[]\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] arr = %d\n", arr);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"quickSortIterative\",\"func_id\":0,\"name\":\"l\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] l = %d\n", l);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"quickSortIterative\",\"func_id\":0,\"name\":\"h\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] h = %d\n", h);
    // Create an auxiliary stack
    int stack[ h - l + 1 ];
 
    // initialize top of stack
    int top = -1;
 
    // push initial values of l and h to stack
    stack[ ++top ] = l;
    stack[ ++top ] = h;
 
    // Keep popping from stack while is not empty
    while ( top >= 0 )
    {
        // Pop h and l
        h = stack[ top-- ];
        l = stack[ top-- ];
 
        // Set pivot element at its correct position in sorted array
        int p = partition( arr, l, h );
 
        // If there are elements on left side of pivot, then push left
        // side to stack
        if ( p-1 > l )
        {
            stack[ ++top ] = l;
            stack[ ++top ] = p - 1;
        }
 
        // If there are elements on right side of pivot, then push right
        // side to stack
        if ( p+1 < h )
        {
            stack[ ++top ] = p + 1;
            stack[ ++top ] = h;
        }
    }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"quickSortIterative\",\"func_id\":0}");
}
