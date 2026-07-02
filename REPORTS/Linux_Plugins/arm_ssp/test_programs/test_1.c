// test_canary_basic.c
// Compile: gcc -O1 -fstack-protector-all
//          -fplugin=./arm_pertask_ssp.so
//          -fplugin-arg-arm_pertask_ssp-offset=16
//          test_canary_basic.c -o test_canary_basic

#include <stdio.h>
#include <string.h>

void process_input(const char *input) {
    char buf[256];   /* large buffer — guarantees canary emission */
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    printf("%s\n", buf);
}

int accumulate(int n) {
    int arr[64];     /* another canary-triggering frame */
    int i, sum = 0;
    for (i = 0; i < n && i < 64; i++) {
        arr[i] = i * i;
        sum += arr[i];
    }
    return sum;
}

int main(int argc, char *argv[]) {
    process_input(argc > 1 ? argv[1] : "hello");
    printf("%d\n", accumulate(10));
    return 0;
}