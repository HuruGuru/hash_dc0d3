#include "base64.h"
#include <stdio.h>

#define N 1024

int main() {
    char str[N];
    unsigned char output[N];
    int choice;
    int decode_len = base64_decode(str, output);

    // TODO: add func for encode

    do {
        printf("Enter base64 string: ");
        scanf("%s", str);

        decode_len = base64_decode(str, output);
        if (decode_len > 0) {
            output[decode_len] = '\0';
            printf("Decoded output: %s\n", output);
        } else {
            printf("Decoding failed\n");
        }

        printf("Choose an option:\n");
        printf("1. Continue\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

    } while (choice == 1);

    return 0;
}
