#include "../include/md5.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MD5_LEN 16
#define MAX_LINE_LENGTH 1024

void md5_to_string(uint8_t *md5_hash, char *output) {
    for (int i = 0; i < MD5_LEN; ++i) {
        sprintf(&output[i * 2], "%02x", md5_hash[i]);
    }
    output[32] = '\0';
}

char *generate_combinations(const char *charset, int charset_length, int max_length,
                            const char *target_hash) {
    char *current = malloc(max_length + 1);
    uint8_t md5_result[16];
    char md5_string[33];
    int i, j;

    for (i = 1; i <= max_length; i++) {
        memset(current, 0, max_length + 1);

        while (1) {
            md5((uint8_t *)current, strlen(current), md5_result);
            md5_to_string(md5_result, md5_string);

            if (strcmp(md5_string, target_hash) == 0) {
                return current;
            }

            // Generate next combination
            for (j = 0; j < i; j++) {
                if (current[j] == charset[charset_length - 1]) {
                    if (j == i - 1) {
                        goto next_length;
                    }
                    current[j] = charset[0];
                } else {
                    current[j] = charset[strchr(charset, current[j]) - charset + 1];
                    break;
                }
            }
        }
    next_length:;
    }

    free(current);
    return NULL;
}

char *brute_force_md5(const char *target_hash, const char *dict_file) {
    FILE *file = fopen(dict_file, "r");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    uint8_t md5_result[16];
    char md5_string[33];
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        md5((uint8_t *)line, strlen(line), md5_result);
        md5_to_string(md5_result, md5_string);

        if (strcmp(md5_string, target_hash) == 0) {
            fclose(file);
            return strdup(line);
        }
    }

    fclose(file);
    return NULL;
}
