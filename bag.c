//
// Created by Igor Solkin on 13.07.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <regex.h>
#include "bag.h"

void scan(char *file, char *pattern) {
    regex_t regex;
    int reti = 0;
    if (pattern != 0) {
        reti = regcomp(&regex, pattern, 0);
        if (reti) {
            fprintf(stderr, "Could not compile regex\n");
            exit(1);
        }
    }
    FILE *bag = fopen(file, "r");
    clock_t time;
    time = clock();
    off_t offset = 0;
    fseek(bag, 0L, SEEK_END);
    long long file_size = ftell(bag);
    rewind(bag);
    printf("LENGTH %lli bytes\n", file_size);
    while (offset < file_size) {
        struct Record record = read_file(bag, offset);
        offset = record.offset + record.file_size;
        if (pattern != 0) {
            reti = regexec(&regex, record.file_path, 0, NULL, 0);
        }
        if (!reti) {
            printf("FILE %s (%llu bytes)\n", record.file_path, record.file_size);
        }
        free(record.file_path);
    }
    fclose(bag);
    if (pattern != 0) {
        regfree(&regex);
    }
    float diff = ((float) (clock() - time) / 1000000.0F);
    printf("TIME %.4f ms\n", diff);
    exit(EXIT_SUCCESS);
}

void extract(char *file, char *dir, char *pattern) {
    regex_t regex;
    int reti = 0;
    if (pattern != 0) {
        reti = regcomp(&regex, pattern, 0);
        if (reti) {
            fprintf(stderr, "Could not compile regex\n");
            exit(1);
        }
    }
    FILE *bag = fopen(file, "r");
    clock_t time;
    time = clock();
    off_t offset = 0;
    fseek(bag, 0L, SEEK_END);
    long long file_size = ftell(bag);
    rewind(bag);
    printf("LENGTH %lli bytes\n", file_size);
    while (offset < file_size) {
        struct Record record = read_file(bag, offset);
        offset = record.offset + record.file_size;

        if (pattern != 0) {
            reti = regexec(&regex, record.file_path, 0, NULL, 0);
        }
        if (!reti) {
            printf("FILE %s (%llu bytes)\n", record.file_path, record.file_size);

            size_t path_length = strlen(record.file_path);
            size_t abs_path_length = strlen(dir) + path_length + 1;
            char abs_path[abs_path_length];
            strcpy(abs_path, dir);
            strncat(abs_path, record.file_path, path_length);
            make_path(abs_path, S_IRWXU);
            copy_to_file(bag, abs_path, record.file_size);
        }

        free(record.file_path);
    }
    fclose(bag);
    if (pattern != 0) {
        regfree(&regex);
    }
    float diff = ((float) (clock() - time) / 1000000.0F);
    printf("TIME %.4f ms\n", diff);
    exit(EXIT_SUCCESS);
}

void create(char *dir, char *file) {
    FILE *bag = fopen(file, "w+");
    clock_t time;
    time = clock();
    write_dir(dir, bag, "");
    fclose(bag);
    float diff = ((float) (clock() - time) / 1000000.0F);
    printf("TIME %.4f ms\n", diff);
    exit(EXIT_SUCCESS);
}

void write_file(const char *path, const char *name, FILE *bag) {
    FILE *file;
    file = fopen(name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
        return;
    }

    fseek(file, 0L, SEEK_END);
    long long file_size = ftell(file);
    rewind(file);

    size_t path_with_slash_length = 1 + strlen(path) + strlen(name) + 1;
    char path_with_slash[path_with_slash_length];
    strcpy(path_with_slash, path);
    strcat(path_with_slash, "/");
    strncat(path_with_slash, name, path_with_slash_length);
    unsigned short path_length = (unsigned short) strlen(path_with_slash);
    put_unsigned_short_be(short_buffer, path_length);
    fwrite(short_buffer, sizeof(char), 2, bag);
    fwrite(path_with_slash, sizeof(char), path_length, bag);
    put_long_long_be(long_long_buffer, file_size);
    fwrite(long_long_buffer, sizeof(char), 8, bag);
    size_t read;
    while ((read = fread(buffer, sizeof(char), BUFSIZE, file)) > 0) {
        fwrite(buffer, sizeof(char), read, bag);
    }

    fclose(file);
}

void write_dir(const char *dir, FILE *bag, const char *dir_rel) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 ||
                strcmp("..", entry->d_name) == 0) {
                continue;
            }
            size_t dir_len = strlen(dir_rel) + strlen(entry->d_name) + 1 + 1;
            char dir_rel_n[dir_len];
            strcpy(dir_rel_n, dir_rel);
            strcat(dir_rel_n, "/");
            strncat(dir_rel_n, entry->d_name, strlen(entry->d_name));
            printf("DIR %s\n", dir_rel_n);
            write_dir(entry->d_name, bag, dir_rel_n);
        } else {
            printf("FILE %s/%s... ", dir_rel, entry->d_name);
            write_file(dir_rel, entry->d_name, bag);
            printf("done.\n");
        }
    }
    chdir("..");
    closedir(dp);
    free(entry);
}

void read_buffer(FILE *file, char *buf, size_t buf_size) {
    size_t total = 0;
    size_t read;
    size_t bytes = buf_size;
    while ((read = fread(buf, sizeof(char), bytes, file)) > 0) {
        total += read;
        if (total == buf_size) {
            break;
        } else if (total + bytes > buf_size) {
            bytes = (buf_size - total);
        }
    }
}

void copy_to_file(FILE *in, const char *path, long long length) {
    FILE *out;
    out = fopen(path, "w+");

    if (length > 0) {
        size_t total = 0;
        size_t read;
        size_t bytes = BUFSIZE;
        if (bytes > length) {
            bytes = (size_t) length;
        }
        while ((read = fread(buffer, sizeof(char), bytes, in)) > 0) {
            fwrite(buffer, sizeof(char), read, out);
            total += read;
            if (total == length) {
                break;
            } else if (total + bytes > length) {
                bytes = (length - total);
            }
        }
    }

    fclose(out);
}

struct Record read_file(FILE *bag, off_t offset) {
    fseek(bag, offset, SEEK_SET);

    read_buffer(bag, short_buffer, 2);
    unsigned short path_length = get_unsigned_short_be(short_buffer);
    char path[path_length + 1];
    read_buffer(bag, path, path_length);
    path[path_length] = '\0';
    read_buffer(bag, long_long_buffer, 8);
    unsigned long long file_length = get_long_long_be(long_long_buffer);

    struct Record record;
    record.file_path = malloc(path_length + 1);
    strncpy(record.file_path, path, path_length + 1);
    record.offset = offset + 2 + path_length + 8;
    record.file_size = file_length;
    return record;
}