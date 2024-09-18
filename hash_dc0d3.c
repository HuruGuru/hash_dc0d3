#include "base64.h"
#include <stdio.h>

#define N 1024

int main() {
    char str[N];
    unsigned char output[N];
    int choice;
    int decode_len = base64_decode(str, output);

    do {
        printf("Choose an option:\n"
               "1. Encode string to base64\n"
               "2. Decode base64 string\n"
               "3. Exit\n\n"
               "Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Encoding function is not ready.\n\n");
            break;
        case 2:
            printf("Enter base64 string: ");
            scanf("%s", str);

            decode_len = base64_decode(str, output);
            if (decode_len > 0) {
                output[decode_len] = '\0';
                printf("Decoded output: %s\n\n", output);
            } else {
                printf("Decoding failed\n\n");
            }
            break;

        case 3:
            printf("Exiting the program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n\n");
            break;
        }

    } while (choice != 3);

    return 0;
}
