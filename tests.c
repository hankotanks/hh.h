#define HH_IMPLEMENTATION
#define HH_STRIP_PREFIXES
#define HH_LOG HH_LOG_DBG
#include "hh.h"

#include <time.h>
#include <stdlib.h>

void
test_path(void) {
    // create a new path
    char* path_root = path_alloc(PROJECT_ROOT);
    DBG("Created path_root: "
        "path = \"%s\", len = %zu, cap = %zu", path_root, darrlen(path_root), darrcap(path_root));
    // check joining
    const char* name_header = "hh.h";
    char* path_header = path_alloc(path_root);
    path_join(path_header, name_header);
    ASSERT(path_header != NULL, "hh_path_join returned NULL");
    ASSERT(strcmp(path_name(path_header), name_header) == 0, 
        "hh_path_name returned incorrect filename: "
        "path = %s, len = %zu, cap = %zu", path_header, darrlen(path_header), darrcap(path_header));
    DBG("Joined \"hh.h\" to path_root: "
        "path = %s, len = %zu, cap = %zu", path_header, darrlen(path_header), darrcap(path_header));
    // take parent
    hh_path_parent(path_header);
    ASSERT(path_header != NULL, "hh_path_parent returned NULL before system root");
    DBG("Took parent of path_header: "
        "path = %s, len = %zu, cap = %zu", path_header, darrlen(path_header), darrcap(path_header));
    // assert that adding and removing path components returns identical strings
    ASSERT(strcmp(path_root, path_header) == 0, 
        "hh_path_parent returned unexpected result: "
        "path = %s, len = %zu, cap = %zu", path_header, darrlen(path_header), darrcap(path_header));
    // free path_header
    path_free(path_header);
    ASSERT(path_header == NULL, 
        "hh_path_free (hh_darrfree) did not set NULL after free: "
        "path = %s, len = %zu, cap = %zu",  path_header, darrlen(path_header), darrcap(path_header));
    // descend from project root to system root
    DBG("Traversing from path_root to system root: "
        "path = %s, len = %zu, cap = %zu", path_root, darrlen(path_root), darrcap(path_root));
    while(!path_is_root(path_root)) {
        ASSERT(path_exists(path_parent(path_root)), 
            "hh_path_parent returned non-existent path while not at system root: "
            "path = %s, len = %zu, cap = %zu",  path_root, darrlen(path_root), darrcap(path_root));
        ASSERT(strlen(path_root) + 1 == darrlen(path_root), 
            "hh_path_parent broke parity between hh_darrlen and strlen: "
            "path = %s, len = %zu, cap = %zu",  path_root, darrlen(path_root), darrcap(path_root));
        DBG("Took parent of path_root: "
            "path = %s, len = %zu, cap = %zu", path_root, darrlen(path_root), darrcap(path_root));
    }
    // validate system root properties
    ASSERT(path_parent(path_root) == NULL, 
        "hh_path_parent returned non-NULL while hh_path_is_root was truthy: "
        "path = %s, len = %zu, cap = %zu",  path_root, darrlen(path_root), darrcap(path_root));
    ASSERT(path_exists(path_root), 
        "hh_path_exists was false while hh_path_is_root was truthy: "
        "path = %s, len = %zu, cap = %zu",  path_root, darrlen(path_root), darrcap(path_root));
    ASSERT(!path_is_file(path_root), 
        "hh_path_is_file reported that system root is a file, not a directory: "
        "path = %s, len = %zu, cap = %zu",  path_root, darrlen(path_root), darrcap(path_root));
    // free path_root
    path_free(path_root);
    ASSERT(path_root == NULL, "hh_path_free (hh_darrfree) did not set NULL after free");
}

void
test_darr_print(char** arr) {
    printf("\"");
    for(size_t i = 0; i < darrlen(arr); ++i) printf("%s ", arr[i]);
    printf("\"\n");
}

void
test_darr(void) {
    char** arr = NULL, fst[6], snd[6];
    darrput(arr, "Hello");
    darrput(arr, "World");
    darrput(arr, "This");
    darrput(arr, "is");
    darrput(arr, "a");
    darrput(arr, "test");
    DBG_BLOCK {
        printf("Original: arr = ");
        test_darr_print(arr);
    }
    // randomly swap a few times
    size_t i, j, k;
    for(i = 0; i < darrlen(arr); ++i) {
        j = (size_t) ((uint64_t) rand() * darrlen(arr) / (RAND_MAX + 1ULL));
        k = (size_t) ((uint64_t) rand() * darrlen(arr) / (RAND_MAX + 1ULL));
        strcpy(fst, arr[j]);
        strcpy(snd, arr[k]);
        darrswap(arr, j, k);
        DBG_BLOCK {
            printf("Swapped \"%s\" and \"%s\": arr = ", fst, snd);
            test_darr_print(arr);
        }
        ASSERT(strcmp(fst, arr[k]) == 0 && strcmp(snd, arr[j]) == 0, 
            "hh_darrswap failed to swap elements");
    }
    // pop swap
    j = darrlen(arr);
    for(i = 0; i < j; ++i) {
        k = (size_t) ((uint64_t) rand() * darrlen(arr) / (RAND_MAX + 1ULL));
        char* tmp = darrswapdel(arr, k);
        DBG_BLOCK {
            printf("Removed \"%s\": len = %zu, idx = %zu, arr = ", tmp, darrlen(arr), k);
            test_darr_print(arr);
        }
        ASSERT(darrlen(arr) + 1 == (j - i), 
            "hh_darrswapdel failed to remove element: len = %zu, expected = %zu", darrlen(arr), j - i - 1);
    }
    ASSERT(darrlen(arr) == 0, 
        "hh_darrswapdel failed to remove all elements");
    ASSERT(darrswapdel(arr, 0) == NULL, 
        "hh_darrswapdel did not return NULL on empty array");
    darrfree(arr);
}

void
test_misc(void) {

}

void
test_args(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
}

void
test_edition(void) {
    DBG("C89 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_89)));
    DBG("C90 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_90)));
    DBG("C94 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_94)));
    DBG("C99 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_99)));
    DBG("C11 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_11)));
    DBG("C17 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_17)));
    DBG("C23 support: %s", STRINGIFY_BOOL(edition_supported(EDITION_23)));
}

void
test_util(void) {

}

void
test_span(void) {

}

int
main(int argc, char* argv[]) {
    srand((unsigned int) time(NULL));
    // start of tests
    test_args(argc, argv);
    test_edition();
    test_path();
    test_darr();
    test_util();
    // end of tests
    return 0;
}
