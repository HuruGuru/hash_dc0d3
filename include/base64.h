#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>

char *base64_encode(const unsigned char *data, size_t input_len, size_t *output_len);

int base64_decode(const char *input, unsigned char *output);

#endif
