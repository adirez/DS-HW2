#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *flat_text(char **words, int n) {
    char *result = NULL;
    char *tmp_result = NULL;
    int total_length = 0;
    for (int i = 0; i < n; ++i) {
        int length = (int)strlen(words[i]);
        total_length += length;
        tmp_result = malloc((size_t) total_length + 1);
        if (tmp_result == NULL) {
            free(result);
            return NULL;
        }
        if (result != NULL) {
            strcpy(tmp_result, result);
        }
        strcpy(tmp_result + (total_length - length), words[i]);
        free(result);
        result = tmp_result;
    }
    return result;
}

void initialize_words_arr (char **words) {
    words[0] = "Hello";
    words[1] = "To";
    words[2] = "234122";
    words[3] = "Matam";
}

int main() {
    char *words[4];
    initialize_words_arr(words);
    char *p = flat_text(words, 4);
    if (p!=NULL)
    {
        printf("\n%s\n", p);
        free(p);
    }
    return 0;
}