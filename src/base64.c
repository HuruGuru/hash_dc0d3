#include "../include/base64.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const unsigned char *data, size_t input_len, size_t *output_len) {
    *output_len = 4 * ((input_len + 2) / 3);
    char *encoded_data = malloc(*output_len + 1);
    if (!encoded_data) {
        fprintf(stderr, "Memory allocation error for %zu bytes. Error %d: %s\n", *output_len + 1,
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0, j = 0; i < input_len;) {
        unsigned char a = i < input_len ? data[i++] : 0;
        unsigned char b = i < input_len ? data[i++] : 0;
        unsigned char c = i < input_len ? data[i++] : 0;

        u_int32_t triple = (a << 16) + (b << 8) + c;

        encoded_data[j++] = base64_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = (i > input_len + 1) ? '=' : base64_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = (i > input_len) ? '=' : base64_table[triple & 0x3F];
    }

    encoded_data[*output_len] = '\0';

    return encoded_data;
}

int base64_decode(const char *input, unsigned char *output) {
    int input_len = strlen(input);
    int output_len = 0;

    char *input_clean = malloc(input_len + 1);
    if (!input_clean) {
        fprintf(stderr, "Memory allocation error for %d bytes. Error %d: %s\n", input_len + 1,
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    char *ptr = input_clean;
    for (int i = 0; i < input_len; ++i) {
        if (input[i] != '\n' && input[i] != ' ') {
            *ptr = input[i];
            ptr++;
        }
    }

    *ptr = '\0';
    input_len = ptr - input_clean;

    for (int i = 0; i < input_len; i += 4) {
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
