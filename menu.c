#include "menu.h"
#include "base64.h"
#include "md5.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 1024

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void show_menu() {
    char str[N];
    unsigned char output[N];
    size_t encode_len;
    size_t decode_len;
    int choice;

    do {
        printf("Choose an option:\n"
               "1. Encode string to base64\n"
               "2. Decode base64 string\n"
               // TODO: add md5-decode, sha256, bcrypt
               "3. Encode string to md5\n"
               "4. Decode md5 string\n"
               "5. Exit\n\n"
               "Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

        switch (choice) {
        case 1:
            printf("Enter string to encode: ");
            fgets(str, sizeof(str), stdin);
            str[strcspn(str, "\n")] = '\0';
            char *encoded_data = base64_encode((unsigned char *)str, strlen(str), &encode_len);
            if (encoded_data) {
                printf("Encoded output: %s\n\n", encoded_data);
                free(encoded_data);
            } else {
                printf("Encoding failed\n\n");
            }
            break;
        case 2:
            printf("Enter base64 string: ");
            scanf("%s", str);
            clear_input_buffer();

            decode_len = base64_decode(str, output);
            if (decode_len > 0) {
                output[decode_len] = '\0';
                printf("Decoded output: %s\n\n", output);
            } else {
                printf("Decoding failed\n\n");
            }
            break;

        case 3:
            printf("Enter string to hash: ");
            fgets(str, sizeof(str), stdin);
            str[strcspn(str, "\n")] = '\0';
            printf("===============\n%s\n================\n", str);

            uint8_t md5_hash[16];
            md5((const uint8_t *)str, strlen(str), md5_hash); // FIXME: wrong hash
            printf("MD5 hash: ");
            for (size_t i = 0; i < sizeof(md5_hash); ++i) {
                printf("%02x", md5_hash[i]);
            }
            printf("\n");
            break;

        case 4:
            printf("Not ready yet\n");
            break;

        case 5:
            printf("Exiting the program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n\n");
            break;
        }

    } while (choice != 5);
}
