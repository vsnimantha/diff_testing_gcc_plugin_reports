// test_multi_canary.c
// Compile: gcc -O1 -fstack-protector-all
//          -fplugin=./arm_pertask_ssp.so
//          -fplugin-arg-arm_pertask_ssp-offset=16
//          test_multi_canary.c -o test_multi_canary

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Each function has a local buffer — each generates its own canary SET insn */

void level3(int depth) {
    char scratch[128];
    snprintf(scratch, sizeof(scratch), "depth=%d", depth);
    puts(scratch);
}

void level2(const char *tag) {
    char buf[200];
    snprintf(buf, sizeof(buf), "[%s]", tag);
    level3(strlen(buf));
}

void level1(int n) {
    char line[256];
    int i;
    for (i = 0; i < n; i++) {
        snprintf(line, sizeof(line), "item %d", i);
        level2(line);
    }
}

char *build_string(int len) {
    char tmp[512];   /* another canary frame */
    int i;
    for (i = 0; i < len && i < (int)sizeof(tmp)-1; i++)
        tmp[i] = 'a' + (i % 26);
    tmp[i] = '\0';
    return strdup(tmp);
}

int main(void) {
    char *s = build_string(80);
    level1(3);
    printf("%s\n", s);
    free(s);
    return 0;
}