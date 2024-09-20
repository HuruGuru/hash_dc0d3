#include "md5.h"
#include <string.h>

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

static const uint32_t k[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0x0bab3b35, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

static const uint32_t s[] = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

void md5_init(MD5_CTX *ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;

    ctx->count[0] = ctx->count[1] = 0;
}

void md5_process_block(MD5_CTX *ctx) {
    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];

    uint32_t *M = (uint32_t *)ctx->buffer;

    for (int i = 0; i < 64; i++) {
        uint32_t f;
        int g;

        if (i < 16) {
            f = (b & c) | (~b & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | (~d & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | ~d);
            g = (7 * i) % 16;
        }

        f += a + k[i] + M[g];
        a = d;
        d = c;
        c = b;
        b += LEFTROTATE(f, s[i % sizeof(s) / sizeof(s[0])]);
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
}

void md5_update(MD5_CTX *ctx, const uint8_t *input, size_t length) {
    size_t i = (ctx->count[1] >> 3) & ~63;
    size_t j = (ctx->count[1] >> 3) & ~63;

    ctx->count[1] += length << 3;
    if (ctx->count[1] < (length << 3)) {
        ctx->count[0]++;
    }

    ctx->count[0] += length >> (29);

    while (length--) {
        ctx->buffer[j++] = *input++;
        if (j == sizeof(ctx->buffer)) {
            md5_process_block(ctx);
            j = i;
        }
    }
}
void md5_final(MD5_CTX *ctx, uint8_t digest[16]) {
    size_t index = (ctx->count[1] >> 3) & ~63;

    ctx->buffer[index++] = 0x80;

    if (index > sizeof(ctx->buffer) - sizeof(ctx->count)) {
        while (index < sizeof(ctx->buffer)) {
            ctx->buffer[index++] = 0;
        }
        md5_process_block(ctx);
        index = sizeof(ctx->buffer);
    }

    while (index < sizeof(ctx->buffer)) {
        ctx->buffer[index++] = 0;
    }

    memcpy(&ctx->buffer[56], ctx->count, sizeof(ctx->count));

    md5_process_block(ctx);

    memcpy(digest, ctx->state, sizeof(ctx->state));
}
void md5(const uint8_t *input, size_t length, uint8_t digest[16]) {
    MD5_CTX ctx;
    md5_init(&ctx);
    md5_update(&ctx, input, length);
    md5_final(&ctx, digest);
}
