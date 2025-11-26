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

struct pose {
    double timestamp;
    size_t frame;
    double x, y, z, q[4];
};

void
test_span_parse_csv(void) {
    // build path to odometry.csv
    char* path = path_alloc(PROJECT_ROOT);
    ASSERT(path_join(path, "test_assets", "odometry.csv"), "Failed construct path to odometry.csv");
    DBG("Path to odometry: %s", path);
    // read contents
    char* contents = read_entire_file(path);
    path_free(path);
    ASSERT(contents != NULL, "Failed to read odometry.csv");
    span_t span;
    ASSERT(span_init(&span, contents, ","), "Failed to initialize span");
    struct pose* poses = NULL, pose;
    // skip header
    ASSERT(span_next_line(&span), "Failed to skip header line");
    // parse all poses
    for(bool flag = true; flag;) {
        ASSERT(span_parse_next(&span, "%lf", &pose.timestamp), "Failed to parse timestamp: %lf", pose.timestamp);
        ASSERT(span_parse_next(&span, "%zu", &pose.frame), "Failed to parse frame: %zu", pose.frame);
        ASSERT(span_parse_next(&span, "%lf", &pose.x), "Failed to parse x: %lf", pose.x);
        ASSERT(span_parse_next(&span, "%lf", &pose.y), "Failed to parse y: %lf", pose.y);
        ASSERT(span_parse_next(&span, "%lf", &pose.z), "Failed to parse z: %lf", pose.z);
        ASSERT(span_parse_next(&span, "%lf", &pose.q[0]), "Failed to parse qx: %lf", pose.q[0]);
        ASSERT(span_parse_next(&span, "%lf", &pose.q[1]), "Failed to parse qy: %lf", pose.q[1]);
        ASSERT(span_parse_next(&span, "%lf", &pose.q[2]), "Failed to parse qz: %lf", pose.q[2]);
        if(!span_parse_next(&span, "%lf", &pose.q[3])) flag = false;
        // DBG("timestamp = %.2lf, frame = %zu, x = %.2lf, y = %.2lf, z = %.2lf, q = [%.2lf, %.2lf, %.2lf, %.2lf]", 
        //     pose.timestamp, pose.frame, pose.x, pose.y, pose.z, pose.q[0], pose.q[1], pose.q[2], pose.q[3]);
        darrput(poses, pose);
    }
    // calculate average position
    double pose_avg[3] = { 0.0, 0.0, 0.0 };
    for(size_t i = 0; i < darrlen(poses); ++i) {
        pose_avg[0] += poses[i].x;
        pose_avg[1] += poses[i].y;
        pose_avg[2] += poses[i].z;
    }
    pose_avg[0] /= (double) darrlen(poses);
    pose_avg[1] /= (double) darrlen(poses);
    pose_avg[2] /= (double) darrlen(poses);
    DBG("avg. pos. of %zu odom. entries: x = %.2lf, y = %.2lf, z = %.2lf", darrlen(poses), pose_avg[0], pose_avg[1], pose_avg[2]);
    // print first and last pose
    pose = poses[0];
    DBG("first entry: timestamp = %.2lf, frame = %zu, x = %.2lf, y = %.2lf, z = %.2lf, q = [%.2lf, %.2lf, %.2lf, %.2lf]", 
        pose.timestamp, pose.frame, pose.x, pose.y, pose.z, pose.q[0], pose.q[1], pose.q[2], pose.q[3]);
    pose = darrlast(poses);
    DBG("last entry: timestamp = %.2lf, frame = %zu, x = %.2lf, y = %.2lf, z = %.2lf, q = [%.2lf, %.2lf, %.2lf, %.2lf]", 
        pose.timestamp, pose.frame, pose.x, pose.y, pose.z, pose.q[0], pose.q[1], pose.q[2], pose.q[3]);
    // free
    darrfree(poses);
    darrfree(contents);
}

void
test_span(void) {
    const char text[] = \
        "Hello, 3.14159 World!\n";
    size_t token_index = 0;
    span_t span;
    ASSERT(span_init(&span, text, ","), "Failed to initialize span");
    char hello[32];
    ASSERT(span_parse(&span, "%s", hello), "Failed to parse token [%zu]", token_index);
    ASSERT(strlen(hello) == span.len, 
        "Mismatch between parsed string's length and the span's length: "
        "len = %zu, expected = %zu", strlen(hello), span.len);
    DBG("Token [%zu]: len = %zu, token = \"%s\", skips = %zu", token_index++, span.len, hello, span.skips);
    span.delim = NULL;
    ASSERT(span_next(&span), "Failed to parse token [%zu]", token_index);
    double pi;
    ASSERT(span_parse(&span, "%lf", &pi), "Failed to parse token [%zu]", token_index);
    DBG("Token [%zu]: len = %zu, token = %lf", token_index++, span.len, pi);
    ASSERT(span_next(&span), "Failed to parse token [%zu]", token_index);
    char world[32];
    ASSERT(span_parse(&span, "%s", world), "Failed to parse token [%zu]", token_index);
    ASSERT(strlen(world) == span.len, 
        "Mismatch between parsed string's length and the span's length: "
        "len = %zu, expected = %zu", strlen(world), span.len);
    DBG("Token [%zu]: len = %zu, token = \"%s\"", token_index++, span.len, world);
}

int
main(int argc, char* argv[]) {
    srand((unsigned int) time(NULL));
    // start of tests
    test_args(argc, argv);
    test_edition();
    test_path();
    test_darr();
    test_span();
    test_span_parse_csv();
    // end of tests
    return 0;
}
