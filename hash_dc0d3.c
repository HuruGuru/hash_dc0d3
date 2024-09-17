#include "base64.h"
#include <stdio.h>

#define N 1024

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
