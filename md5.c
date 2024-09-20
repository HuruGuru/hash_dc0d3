#include "md5.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

static const uint8_t PADDING[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const uint32_t k[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const uint32_t r[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

void md5_init(MD5_CTX *ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;

    ctx->count[0] = 0;
    ctx->count[1] = 0;
}

void md5_process_block(MD5_CTX *ctx) {
    uint32_t a = ctx->state[0], b = ctx->state[1], c = ctx->state[2], d = ctx->state[3];
    uint32_t w[16];

    for (int i = 0; i < 16; i++) {
        w[i] = (uint32_t)(ctx->buffer[i * 4]) |
               ((uint32_t)(ctx->buffer[i * 4 + 1]) << 8) |
               ((uint32_t)(ctx->buffer[i * 4 + 2]) << 16) |
               ((uint32_t)(ctx->buffer[i * 4 + 3]) << 24);
    }

    for (int i = 0; i < 64; i++) {
        uint32_t f, g;

        if (i < 16) {
            f = (b & c) | ((~b) & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        uint32_t temp = d;
        d = c;
        c = b;
        b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
        a = temp;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
}

void md5_update(MD5_CTX *ctx, const uint8_t *input, size_t length) {
    size_t index = (size_t)((ctx->count[0] >> 3) & 0x3F);
    
    if ((ctx->count[0] += ((uint32_t)length << 3)) < ((uint32_t)length << 3))
        ctx->count[1]++;
    ctx->count[1] += ((uint32_t)length >> 29);

    size_t partLen = 64 - index;

    size_t i = 0;
    if (length >= partLen) {
        memcpy(&ctx->buffer[index], input, partLen);
        md5_process_block(ctx);

        for (i = partLen; i + 63 < length; i += 64)
            md5_process_block(ctx);

        index = 0;
    }

    memcpy(&ctx->buffer[index], &input[i], length - i);
}

void md5_final(MD5_CTX *ctx, uint8_t digest[16]) {
    uint8_t bits[8];
    uint32_t index, padLen;

    for (int i = 0; i < 8; i++)
        bits[i] = (ctx->count[i >> 2] >> ((i & 3) << 3)) & 0xFF;

    index = (uint32_t)((ctx->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    md5_update(ctx, PADDING, padLen);

    md5_update(ctx, bits, 8);

    for (int i = 0; i < 4; i++) {
        digest[i]      = (uint8_t)(ctx->state[0] >> (i * 8));
        digest[i + 4]  = (uint8_t)(ctx->state[1] >> (i * 8));
        digest[i + 8]  = (uint8_t)(ctx->state[2] >> (i * 8));
        digest[i + 12] = (uint8_t)(ctx->state[3] >> (i * 8));
    }
}

void md5(const uint8_t *input, size_t length, uint8_t digest[16]) {
    MD5_CTX ctx;
    md5_init(&ctx);
    md5_update(&ctx, input, length);
    md5_final(&ctx, digest);
}
