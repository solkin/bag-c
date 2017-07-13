//
// Created by Igor Solkin on 13.07.17.
//

#ifndef BAG_UTILS_H
#define BAG_UTILS_H

#include <stdlib.h>

void put_unsigned_short_be(char buf[2], unsigned short val);

void put_long_long_be(char buf[8], long long val);

unsigned short get_unsigned_short_be(char buf[2]);

unsigned long long get_long_long_be(char buf[8]);

int make_path(char *file_path, mode_t mode);

#endif //BAG_UTILS_H
