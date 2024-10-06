#include <base64.h>
#include <md5.h>
#include <md5_brute.h>
#include <sha256.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define N 1024

typedef enum {
    MENU_EXIT = 0,
    MENU_ENCODE_BASE64,
    MENU_DECODE_BASE64,
    MENU_HASH_MD5,
    MENU_CRACK_MD5,
    MENU_HASH_SHA256,
    MENU_CRACK_SHA256,
} MenuOptions;

typedef struct {
    char str[N];
    char dict_path[256];
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

static void clear_screen() {
    system("clear");
    fflush(stdout);
}

void show_menu() {
    AppState state;
    int choice;
    clock_t start_time, end_time;
    double time_spent;
    char *result;

    do {
        clear_screen();
        printf("Choose an option:\n"
               "1. Encode string to base64\n"
               "2. Decode base64 string\n"
               "3. String to hash md5\n"
               "4. Crack md5 hash\n"
               "5. String to hash sha256\n"
               "6. Crack sha256 hash\n"
               // TODO: add bcrypt
               "0. Exit\n\n"
               "Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();
        clear_screen();

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

        case MENU_HASH_MD5:
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

        case MENU_CRACK_MD5:
            printf("Enter MD5 hash to crack: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';
            while (1) {
                printf("Enter path to the dictionary file (or enter '0' to go back): ");
                fgets(state.dict_path, sizeof(state.dict_path), stdin);
                state.dict_path[strcspn(state.dict_path, "\n")] = '\0';

                if (strcmp(state.dict_path, "0") == 0) {
                    printf("\n");
                    break;
                }

                if (file_exists(state.dict_path)) {
                    break;
                } else {
                    printf("Error: File not found at path: %s. Please try again.\n",
                           state.dict_path);
                }
            }

            if (strcmp(state.dict_path, "0") == 0) {
                break;
            }
            start_time = clock();
            result = brute_force_md5(state.str, state.dict_path);
            end_time = clock();
            time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
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

        case MENU_HASH_SHA256:
            printf("Enter string to hash: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';

            uint8_t sha256_hash[32];
            char sha256_string[65];
            sha256(state.str, strlen(state.str), sha256_hash);
            sha256_to_string(sha256_hash, sha256_string);

            printf("=============================\n");
            printf("SHA256 hash: %s\n", sha256_string);
            printf("=============================\n");
            break;

        // TODO: add threades
        case MENU_CRACK_SHA256:
            printf("Enter SHA256 hash to crack: ");
            fgets(state.str, sizeof(state.str), stdin);
            state.str[strcspn(state.str, "\n")] = '\0';

            printf("Enter path to the dictionary file: ");
            fgets(state.dict_path, sizeof(state.dict_path), stdin);
            state.dict_path[strcspn(state.dict_path, "\n")] = '\0';

            start_time = clock();
            result = brute_force_sha256(state.str, state.dict_path);
            end_time = clock();
            time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

            if (result) {
                printf("\n=========================\n");
                printf("Found match: %s\n", result);
                printf("=========================\n");
                free(result);
            } else {
                printf("Not found.\n");
            }
            printf("Time taken: %.2f sec\n", time_spent);
            break;

        case MENU_EXIT:
            clear_screen();
            printf("Exiting the program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n\n");
            break;
        }
        if (choice != MENU_EXIT) {
            printf("Press Enter to continue...");
            getchar();
        }

    } while (choice != MENU_EXIT);
}
