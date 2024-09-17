#include "base64.h"
#include <stdlib.h>
#include <string.h>

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_decode(const char *input, unsigned char *output) {
    int input_len = strlen(input);
    int output_len = 0;

    char *input_clean = malloc(input_len + 1);
    if (!input_clean) {
        return -1;
    }
    char *ptr = input_clean;
    for (size_t i = 0; i < input_len; ++i) {
        if (input[i] != '\n' && input[i] != ' ') {
            *ptr = input[i];
            ptr++;
        }
    }

    *ptr = '\0';
    input_len = ptr - input_clean;

    for (size_t i = 0; i < input_len; i += 4) {
        int a = strchr(base64_table, input_clean[i]) - base64_table;
        int b = strchr(base64_table, input_clean[i + 1]) - base64_table;
        int c = strchr(base64_table, input_clean[i + 2]) - base64_table;
        int d = strchr(base64_table, input_clean[i + 3]) - base64_table;

        output[output_len++] = (a << 2) | (b >> 4);
        if (input_clean[i + 2] != '=') {
            output[output_len++] = (b << 4) | (c >> 2);
        }
        if (input_clean[i + 3] != '=') {
            output[output_len++] = (c << 6) | d;
        }
    }

    free(input_clean);
    return output_len;
}
