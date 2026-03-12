#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/trace_crlog.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"crlog.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

void __puts(const char *str)
{
	fputs(str, stdout);
}

void __putlong(long num)
{
	int neg = 0;
	char buf[22], *s;

	buf[21] = '\0';
	s = &buf[20];

	if (num < 0) {
		neg = 1;
		num = -num;
	} else if (num == 0) {
		*s = '0';
		s--;
		goto done;
	}

	while (num > 0) {
		*s = (num % 10) + '0';
		s--;
		num /= 10;
	}

	if (neg) {
		*s = '-';
		s--;
	}
done:
	s++;
	fputs(s, stdout);
}

void __putshort(short num)
{
	int neg = 0;
	char buf[12], *s;

	buf[11] = '\0';
	s = &buf[10];

	if (num < 0) {
		neg = 1;
		num = -num;
	} else if (num == 0) {
		*s = '0';
		s--;
		goto done;
	}

	while (num > 0) {
		*s = (num % 10) + '0';
		s--;
		num /= 10;
	}

	if (neg) {
		*s = '-';
		s--;
	}
done:
	s++;
	fputs(s, stdout);
}

void __putulong(unsigned long num)
{
	/* XXX: make ul version */
	__putlong((long)num);
}

void __putwrite(const char *str, size_t size, size_t nmemb)
{
	fwrite(str, size, nmemb, stdout);
}

int timeval_subtract(struct timeval *result,
		struct timeval *a, struct timeval *b)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/crlog.c","function":"timeval_subtract","func_id":1,"num_params":3,"start_line":85,"end_line":107,"metrics":{"num_params":3,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"result","type":"struct timeval *","is_pointer":true,"is_const":false},{"name":"a","type":"struct timeval *","is_pointer":true,"is_const":false},{"name":"b","type":"struct timeval *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"timeval_subtract","func_id":1,"name":"result","type":"struct timeval *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] result = %p\n", result);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"timeval_subtract","func_id":1,"name":"a","type":"struct timeval *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] a = %p\n", a);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"timeval_subtract","func_id":1,"name":"b","type":"struct timeval *","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] b = %p\n", b);
	/* Perform the carry for the later subtraction by updating b. */
	if (a->tv_usec < b->tv_usec) {
		int nsec = (a->tv_usec - b->tv_usec) / 1000000 + 1;
		b->tv_usec -= 1000000 * nsec;
		b->tv_sec += nsec;
	}
	if (a->tv_usec - b->tv_usec > 1000000) {
		int nsec = (a->tv_usec - b->tv_usec) / 1000000;
		b->tv_usec += 1000000 * nsec;
		b->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = a->tv_sec - b->tv_sec;
	result->tv_usec = a->tv_usec - b->tv_usec;

	/* Return 1 if result is negative. */
    log_with_timestamp(R"JSON({"event":"RETURN","function":"timeval_subtract","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/crlog.c","line":106,"ret_type":"int","expr":"a->tv_sec < b->tv_sec"})JSON");
    printf("[LOG][RETURN_VALUE] %d\n", (a->tv_sec < b->tv_sec));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"timeval_subtract","func_id":1})JSON");
	return a->tv_sec < b->tv_sec;
}


int main(int argc, char **argv)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/crlog.c","function":"main","func_id":0,"num_params":2,"start_line":110,"end_line":142,"metrics":{"num_params":2,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"argc","type":"int","is_pointer":false,"is_const":false},{"name":"argv","type":"char **","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"main","func_id":0,"name":"argc","type":"int","is_pointer":false,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] argc = %d\n", argc);
    log_with_timestamp(R"JSON({"event":"PARAM","function":"main","func_id":0,"name":"argv","type":"char **","is_pointer":true,"is_const":false})JSON");
    printf("[LOG][PARAM_VALUE] argv = %s\n", argv);
	size_t niter = 10000000;
	size_t i;
	static const char str1[] = "String1 String1";
	static const char str2[] = "string2 string2 string2";
	struct rusage usage;
	struct timeval system_start, system_end;
	struct timeval user_start, user_end;
	struct timeval user, sys;

	getrusage(RUSAGE_SELF, &usage);
	user_start = usage.ru_utime;
	system_start = usage.ru_stime;

	for (i = 0; i < niter; i++)
		printf("Some message %s %s %c %li %d %lu\n",
				str1, str2, 'c', (long)-4,
				(short)2, (unsigned long)2);

	getrusage(RUSAGE_SELF, &usage);
	user_end = usage.ru_utime;
	system_end = usage.ru_stime;

	timeval_subtract(&user, &user_end, &user_start);
	timeval_subtract(&sys, &system_end, &system_start);

	fprintf(stderr, "user\t%lu.%u\nsys\t%lu.%u\n",
			user.tv_sec, user.tv_usec,
			sys.tv_sec, sys.tv_usec);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"main","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/test/crlog.c","line":141,"ret_type":"int","expr":"0"})JSON");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"main","func_id":0})JSON");
	return 0;
}
