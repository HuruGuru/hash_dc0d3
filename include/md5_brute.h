#ifndef MD5_BRUTE_H
#define MD5_BRUTE_H

#include <stdint.h>

void md5_to_string(uint8_t *md5_hash, char *output);
char *brute_force_md5(const char *target_hash, const char *dict_file);

#endif
