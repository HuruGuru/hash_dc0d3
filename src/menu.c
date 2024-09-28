#include <base64.h>
#include <md5.h>
#include <md5_brute.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define N 1024

typedef enum {
    MENU_ENCODE_BASE64 = 1,
    MENU_DECODE_BASE64,
    MENU_ENCODE_MD5,
    MENU_DECODE_MD5,
    MENU_EXIT = 0,
} MenuOptions;

typedef struct {
    char str[N];
    unsigned char output[N];
    size_t encode_len;
    size_t decode_len;

} AppState;

static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void show_menu() {
    AppState state;
    int choice;

    do {
        printf("Choose an option:\n"
               "1. Encode string to base64\n"
               "2. Decode base64 string\n"
               // TODO: add sha256, bcrypt
               "3. Encode string to md5\n"
               "4. Decode md5 string\n"
               "0. Exit\n\n"
               "Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

        switch (choice) {
        case MENU_ENCODE_BASE64:
            printf("Enter string to encode: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';
            char *encoded_data =
                base64_encode((unsigned char *)state.str, strlen(state.str), &state.encode_len);
            if (encoded_data) {
                printf("=============================\n");
                printf("Encoded output: %s", encoded_data);
                printf("\n=============================\n\n");
                free(encoded_data);
            } else {
                printf("Encoding failed\n\n");
            }
            break;
        case MENU_DECODE_BASE64:
            printf("Enter base64 string: ");
            scanf("%s", state.str);
            clear_input_buffer();

            state.decode_len = base64_decode(state.str, state.output);
            if (state.decode_len > 0) {
                state.output[state.decode_len] = '\0';
                printf("=============================\n");
                printf("Decoded output: %s", state.output);
                printf("\n=============================\n\n");
            } else {
                printf("Decoding failed\n\n");
            }
            break;

        case MENU_ENCODE_MD5:
            printf("Enter string to hash: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';

            uint8_t md5_hash[16];
            md5((const uint8_t *)state.str, strlen(state.str), md5_hash);
            printf("=============================\n");
            printf("MD5 hash: ");
            for (size_t i = 0; i < sizeof(md5_hash); ++i) {
                printf("%02x", md5_hash[i]);
            }
            printf("\n=============================\n\n");
            break;

        case MENU_DECODE_MD5:
            printf("Enter MD5 hash to crack: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';
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
            clock_t start_time = clock();
            char *result = brute_force_md5(state.str, dict_path);
            clock_t end_time = clock();
            double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            if (result) {
                printf("\n=========================\n");
                printf("Found match: %s\n", result);
                printf("=========================\n\n");
                free(result);
            } else {
                printf("Not found.\n");
            }
            printf("Time taken: %.2f sec\n\n", time_spent);
            break;

        case MENU_EXIT:
            printf("Exiting the program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n\n");
            break;
        }

    } while (choice != MENU_EXIT);
}
