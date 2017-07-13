//
// Created by Igor Solkin on 13.07.17.
//

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "utils.h"

void put_unsigned_short_be(char buf[2], unsigned short val) {
    buf[0] = (char) ((val >> 8) & 0xFF);
    buf[1] = (char) (val & 0xFF);
}

void put_long_long_be(char buf[8], long long val) {
    buf[0] = (char) ((val >> 56) & 0xFF);
    buf[1] = (char) ((val >> 48) & 0xFF);
    buf[2] = (char) ((val >> 40) & 0xFF);
    buf[3] = (char) ((val >> 32) & 0xFF);
    buf[4] = (char) ((val >> 24) & 0xFF);
    buf[5] = (char) ((val >> 16) & 0xFF);
    buf[6] = (char) ((val >> 8) & 0xFF);
    buf[7] = (char) (val & 0xFF);
}

unsigned short get_unsigned_short_be(char buf[2]) {
    unsigned short val;
    val = (uint8_t) buf[1];
    val |= (uint8_t) buf[0] << 8;
    return val;
}

unsigned long long get_long_long_be(char buf[8]) {
    unsigned long long val;
    val = (uint8_t) buf[7];
    val |= (uint8_t) buf[6] << 8;
    val |= (uint8_t) buf[5] << 16;
    val |= (uint8_t) buf[4] << 24;
    val |= (uint64_t) buf[3] << 32;
    val |= (uint64_t) buf[2] << 40;
    val |= (uint64_t) buf[1] << 48;
    val |= (uint64_t) buf[0] << 56;
    return val;
}

int make_path(char *file_path, mode_t mode) {
    char *p;
    for (p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/')) {
        *p = '\0';
        if (mkdir(file_path, mode) == -1) {
            if (errno != EEXIST) {
                *p = '/';
                return -1;
            }
        }
        *p = '/';
    }
    free(p);
    return 0;
}