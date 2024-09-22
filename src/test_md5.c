#include "../include/md5.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_md5() {
    const char *test_strings[] = {
        "", "m", "hello1", "alala K0ala", "abcdefghijklmnopqrstuvwxyz",
    };
    const char *expected_hashes[] = {
        "d41d8cd98f00b204e9800998ecf8427e", "6f8f57715090da2632453988d9a1501b",
        "203ad5ffa1d7c650ad681fdff3965cd2", "86d3c902e9616803aab6f3a13e98cb5d",
        "c3fcd3d76192e4007dfb496cca67e13b",
    };

    for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++) {
        uint8_t digest[16];
        md5((const uint8_t *)test_strings[i], strlen(test_strings[i]), digest);

        char hash[33];
        for (int j = 0; j < 16; j++)
            sprintf(&hash[j * 2], "%02x", (unsigned int)digest[j]);

        printf("Input: \"%s\"\n", test_strings[i]);
        printf("Calculated: %s\n", hash);
        printf("Expected:   %s\n", expected_hashes[i]);
        printf("Result: %s\n\n", strcmp(hash, expected_hashes[i]) == 0 ? "PASS" : "FAIL");
    }
}

int main() {
    test_md5();
    return 0;
}
