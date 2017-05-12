#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *flat_text(char **words, int n) {
    char *result = NULL;
    int total_length = 0;
    for (int i = 0; i < n; ++i) {
        int length = (int)strlen(words[i]);
        total_length += length;
        result = realloc(result, (size_t) total_length + 1);
        if (result == NULL) {
            return NULL;
        }
        strcpy(result+ (total_length - length), words[i]);
    }
    return result;
}

int main() {
    char *words[4];
    words[0] = "Hello";
    words[1] = "To";
    words[2] = "234122";
    words[3] = "Matam";
    char *p = flat_text(words, 4);
    if (p!=NULL)
    {
        printf("\n%s\n", p);
        free(p);
    }
    return 0;
}