#ifndef MD5_H
#define MD5_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t state[4];  // A, B, C, D
    uint32_t count[2];  // 64-bit count of bits (mod 2^64)
    uint8_t buffer[64];
} MD5_CTX;

void md5_init(MD5_CTX *ctx);
void md5_update(MD5_CTX *ctx, const uint8_t *input, size_t length);
void md5_final(MD5_CTX *ctx, uint8_t digest[16]);
void md5(const uint8_t *input, size_t length, uint8_t digest[16]);

#endif
