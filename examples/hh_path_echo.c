#define HH_IMPLEMENTATION
#define HH_STRIP_PREFIXES
#include "h.h"

int
main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }
    char* path = path_alloc(argv[1]);
    MSG_BLOCK {
        LOG_APPEND("\n");
        LOG_APPEND("  path: %s\n", path);
        char* parent = path_parent(path_alloc(path));
        LOG_APPEND("  path_parent: %s\n", parent);
        path_free(parent);
        LOG_APPEND("  exists:  %s\n", STRINGIFY_BOOL(path_exists(path)));
        LOG_APPEND("  is_file: %s\n", STRINGIFY_BOOL(path_is_file(path)));
        LOG_APPEND("  is_root: %s\n", STRINGIFY_BOOL(path_is_root(path)));
    }
    path_free(path);
    return 0;
}
