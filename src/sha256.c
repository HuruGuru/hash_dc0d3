#include <sha256.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 32

// SHA256 functions
#define ROTRIGHT(word, bits) (((word) >> (bits)) | ((word) << (32 - (bits))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void sha256_transform(uint32_t state[8], const uint8_t data[]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

void sha256(const uint8_t *data, size_t len, uint8_t hash[SHA256_BLOCK_SIZE]) {
    uint32_t state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                         0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    uint64_t bitlen = 0;
    uint32_t i;
    uint8_t buf[64];

    for (i = 0; i < len; ++i) {
        buf[bitlen / 8 % 64] = data[i];
        bitlen += 8;
        if ((bitlen / 8) % 64 == 0)
            sha256_transform(state, buf);
    }

    i = bitlen / 8 % 64;
    buf[i++] = 0x80;
    if (i > 56) {
        memset(buf + i, 0, 64 - i);
        sha256_transform(state, buf);
        i = 0;
    }
    memset(buf + i, 0, 56 - i);
    bitlen = __builtin_bswap64(bitlen);
    memcpy(buf + 56, &bitlen, 8);
    sha256_transform(state, buf);

    for (i = 0; i < 8; ++i)
        state[i] = __builtin_bswap32(state[i]);
    memcpy(hash, state, SHA256_BLOCK_SIZE);
}

void sha256_to_string(const uint8_t hash[SHA256_BLOCK_SIZE], char output[65]) {
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = 0;
}

char *brute_force_sha256(const char *target_hash, const char *dict_file) {
    FILE *file = fopen(dict_file, "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", dict_file);
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint8_t hash[32];
    char hash_string[65];
    char *result = NULL;

    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0'; // Remove newline
        }

        sha256((uint8_t *)line, strlen(line), hash);
        sha256_to_string(hash, hash_string);

        if (strcmp(hash_string, target_hash) == 0) {
            result = strdup(line);
            break;
        }
    }

    free(line);
    fclose(file);
    return result;
}
