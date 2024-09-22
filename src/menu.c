#include "../include/base64.h"
#include "../include/md5.h"
#include "../include/md5_brute.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define N 1024

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
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
               // TODO: add sha256, bcrypt
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
                printf("=============================\n");
                printf("Encoded output: %s", encoded_data);
                printf("\n=============================\n\n");
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
                printf("=============================\n");
                printf("Decoded output: %s", output);
                printf("\n=============================\n\n");
            } else {
                printf("Decoding failed\n\n");
            }
            break;

        case 3:
            printf("Enter string to hash: ");
            fgets(str, sizeof(str), stdin);
            str[strcspn(str, "\n")] = '\0';

            uint8_t md5_hash[16];
            md5((const uint8_t *)str, strlen(str), md5_hash);
            printf("=============================\n");
            printf("MD5 hash: ");
            for (size_t i = 0; i < sizeof(md5_hash); ++i) {
                printf("%02x", md5_hash[i]);
            }
            printf("\n=============================\n\n");
            break;

        case 4:
            printf("Enter MD5 hash to crack: ");
            fgets(str, sizeof(str), stdin);
            str[strcspn(str, "\n")] = '\0';
            char dict_path[256];
            while (1) {
                printf("Enter path to the dictionary file (or enter '0' to go back): ");
                fgets(dict_path, sizeof(dict_path), stdin);
                dict_path[strcspn(dict_path, "\n")] = '\0';

                if (strcmp(dict_path, "0") == 0) {
                    printf("\n");
                    break;
                }

                if (file_exists(dict_path)) {
                    break;
                } else {
                    printf("Error: File not found at path: %s. Please try again.\n", dict_path);
                }
            }

            if (strcmp(dict_path, "0") == 0) {
                break;
            }
            char *result = brute_force_md5(str, dict_path);

            if (result) {
                printf("\n=========================\n");
                printf("Found match: %s\n", result);
                printf("=========================\n\n");
                free(result);
            } else {
                printf("Not found.\n");
            }
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
