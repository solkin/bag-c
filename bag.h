//
// Created by Igor Solkin on 13.07.17.
//

#ifndef BAG_BAG_H
#define BAG_BAG_H

#include <stdio.h>
#include "utils.h"

#define    BUFSIZE    1024 * 1024
char buffer[BUFSIZE];
char short_buffer[2];
char long_long_buffer[8];

struct Record {
    char *file_path;
    off_t offset;
    unsigned long long file_size;
};

void create(char *dir, char *file);

void scan(char *file, char *pattern);

void extract(char *file, char *dir, char *pattern);

struct Record read_file(FILE *bag, long long offset);

void write_file(const char *path, const char *name, FILE *bag);

void write_dir(const char *dir, FILE *bag, const char *dir_rel);

void read_buffer(FILE *file, char *buf, size_t buf_size);

void copy_to_file(FILE *in, const char *path, long long length);

#endif //BAG_BAG_H
