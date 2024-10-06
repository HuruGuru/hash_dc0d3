#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_BLOCK_SIZE 32

void sha256(const uint8_t *data, size_t len, uint8_t hash[SHA256_BLOCK_SIZE]);
void sha256_to_string(const uint8_t hash[SHA256_BLOCK_SIZE], char output[65]);
char *brute_force_sha256(const char *target_hash, const char *dict_file);

#endif // SHA256_H
