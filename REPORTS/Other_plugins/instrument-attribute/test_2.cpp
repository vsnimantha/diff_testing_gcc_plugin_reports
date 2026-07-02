
// test2_file_list.c
// Exercises the include-file-list plugin argument path.
// When compiled with:
//   gcc -fplugin=./plugin.so \
//       -fplugin-arg-plugin-include-file-list=test2_file_list.c \
//       test2_file_list.c -o test2_file_list
// ALL functions in this file should be instrumented because the file
// name appears in the include-file-list.
//
// The plugin uses strstr(), so a partial path match is enough —
// just the filename suffix is sufficient.

#include <stdio.h>
#include <string.h>

// All of these should be instrumented via the file-list match.

int string_length(const char *s)
{
    int n = 0;
    while (s[n]) n++;
    return n;
}

int count_vowels(const char *s)
{
    int n = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i] | 0x20;  // to lower
        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u') n++;
    }
    return n;
}

void reverse_string(char *s)
{
    int len = string_length(s);
    for (int i = 0; i < len / 2; i++) {
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}

int is_palindrome(const char *s)
{
    int len = string_length(s);
    for (int i = 0; i < len / 2; i++) {
        if (s[i] != s[len - 1 - i]) return 0;
    }
    return 1;
}

int main(void)
{
    char word[] = "racecar";
    printf("length('%s')   = %d\n", word, string_length(word));
    printf("vowels('%s')   = %d\n", word, count_vowels(word));
    printf("palindrome     = %d\n", is_palindrome(word));

    char hello[] = "hello";
    reverse_string(hello);
    printf("reverse(hello) = %s\n", hello);
    return 0;
}