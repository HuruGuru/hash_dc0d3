#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 1024

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_decode(const char *input, unsigned char *output) {
    int i_len = strlen(input);
    int o_len = 0;

    char *i_clean = malloc(i_len + 1);
    if (!i_clean) {
        return -1;
    }
    char *ptr = i_clean;
    for (size_t i = 0; i < i_len; ++i) {
        if (input[i] != '\n' && input[i] != ' ') {
            *ptr = input[i];
            ptr++;
        }
    }

    *ptr = '\0';
    i_len = ptr - i_clean;

    for (size_t i = 0; i < i_len; i += 4) {
        int a = strchr(base64_table, i_clean[i]) - base64_table;
        int b = strchr(base64_table, i_clean[i + 1]) - base64_table;
        int c = strchr(base64_table, i_clean[i + 2]) - base64_table;
        int d = strchr(base64_table, i_clean[i + 3]) - base64_table;

        output[o_len++] = (a << 2) | (b >> 4);
        if (i_clean[i + 2] != '=') {
            output[o_len++] = (b << 4) | (c >> 2);
        }
        if (i_clean[i + 3] != '=') {
            output[o_len++] = (c << 6) | d;
        }
    }

    free(i_clean);
    return o_len;
}

int main() {
    char str[N];
    unsigned char output[N];

    printf("Enter base64 string: ");
    scanf("%s", str);

    int decode_len = base64_decode(str, output);
    if (decode_len > 0) {
        output[decode_len] = '\0';
        printf("Decoded output: %s\n", output);
    } else {
        printf("Decoding failed\n");
    }

    return 0;
}
