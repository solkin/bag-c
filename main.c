#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "bag.h"

int main(int argc, char *argv[]) {
    int opt;
    enum {
        CREATE, SCAN, EXTRACT, HELP
    } mode = HELP;
    char *bag = 0;
    char *dir;
    char *ch_dir = 0;
    char *help;
    char *pattern = 0;

    while ((opt = getopt(argc, argv, "csxvC:f:")) != -1) {
        switch (opt) {
            case 'c':
                mode = CREATE;
                break;
            case 's':
                mode = SCAN;
                break;
            case 'x':
                mode = EXTRACT;
                break;
            case 'C':
                ch_dir = optarg;
                break;
            case 'f':
                bag = optarg;
                break;
            default:
                mode = HELP;
                break;
        }
    }
    switch (mode) {
        case CREATE:
            if (ch_dir != 0) {
                chdir(ch_dir);
            }
            dir = argv[optind];
            create(dir, bag);
            break;
        case SCAN:
            pattern = argv[optind];
            scan(bag, pattern);
            break;
        case EXTRACT:
            if (ch_dir != 0) {
                chdir(ch_dir);
            }
            dir = argv[optind];
            if (optind + 1 < argc) {
                pattern = argv[optind + 1];
            }
            extract(bag, dir, pattern);
            break;
        default:
            help = "bag: manipulate bag archive files\n"
                    "First option must be a mode specifier:\n"
                    "  -c Create  -s Scan  -x Extract\n"
                    "Common Options:\n"
                    "  -f <filename>  Location of archive\n"
                    "Create: bag -c [options] [-C <dir> | <dir>]\n"
                    "  -C <dir>  Change to <dir> before processing remaining files\n"
                    "  <dir>  add these items to archive\n"
                    "List: bag -s [options] [<pattern>]\n"
                    "  <pattern>  If specified, list only entries that match\n"
                    "Extract: bag -x [options] [-C <dir> | <dir> | <pattern>]\n"
                    "  -C <dir>  Change to <dir> before processing remaining files\n"
                    "  <dir>  extract items from archive to this directory\n"
                    "  <pattern>  If specified, extract only entries that match\n"
                    "bag 1.0.1";
            puts(help);
            break;
    }
}
