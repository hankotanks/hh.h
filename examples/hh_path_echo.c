#define HH_IMPLEMENTATION
#define HH_STRIP_PREFIXES
#include "hh.h"

int
main(int argc, char* argv[]) {
    (void) argc;
    char* path = path_alloc(argv[1]);
    MSG("%s", path);
    path_free(path);
    return 0;
}
