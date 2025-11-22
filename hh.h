#ifndef HH_H__
#define HH_H__

#ifndef _WIN32
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif // not _WIN32

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

// log only errors
// #define HH_LOG HH_LOG_ERR
// log errors and messages
// #define HH_LOG HH_LOG_MSG
// log everything
// #define HH_LOG HH_LOG_DBG
enum {
    HH_LOG_ERR = 1 << 0,
    HH_LOG_MSG = 1 << 1,
    HH_LOG_DBG = 1 << 2
};

// all logging functions have the same behavior as printf,
// HH_ERR logs to stderr instead of stdout
#ifdef HH_LOG
#if HH_LOG >= HH_LOG_DBG
#define HH_DBG_BLOCK if(printf("DEBUG [%s:%d]: ", __FILE__, __LINE__), true)
#define HH_DBG(...) do { \
	printf("DEBUG [%s:%d]: ", __FILE__, __LINE__); \
	printf(__VA_ARGS__); \
	printf("\n"); \
} while(0)
#else
#define HH_DBG_BLOCK false
#define HH_DBG(...)
#endif // HH_DBG
#if HH_LOG >= HH_LOG_MSG
#define HH_MSG_BLOCK if(printf("INFO [%s:%d]: ", __FILE__, __LINE__), true)
#define HH_MSG(...) do { \
    printf("INFO [%s:%d]: ", __FILE__, __LINE__); \
	printf(__VA_ARGS__); \
	printf("\n"); \
} while(0)
#else
#define HH_MSG_BLOCK if(false)
#define HH_MSG(...)
#endif // HH_MSG
#if HH_LOG >= HH_LOG_ERR
#define HH_ERR_BLOCK if(fprintf(stderr, "ERROR [%s:%d]: ", __FILE__, __LINE__), true)
#define HH_ERR(...) do { \
	fprintf(stderr, "ERROR [%s:%d]: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
} while(0)
#else
#define HH_ERR_BLOCK if(false)
#define HH_ERR(...)
#endif // HH_LOG
#else
#define HH_DBG_BLOCK if(false)
#define HH_MSG_BLOCK if(false)
#define HH_ERR_BLOCK if(false)
#define HH_DBG(...)
#define HH_MSG(...)
#define HH_ERR(...)
#endif

// min and max
#define HH_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define HH_MIN(x, y) (((x) < (y)) ? (x) : (y))

// array length (ONLY for stack-allocated arrays)
#define HH_ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

// useful attributes for function declarations
#if defined(__GNUC__) || defined(__clang__)
#define HH_UNUSED __attribute__((unused))
#else
#define HH_UNUSED
#endif
#if defined(__GNUC__) || defined(__clang__)
#define HH_FALLTHROUGH __attribute__((fallthrough))
#else
#define HH_FALLTHROUGH
#endif

// stringify
#define HH_STRINGIFY_HELPER(x) #x
#define HH_STRINGIFY(x) HH_STRINGIFY_HELPER(x)
// stringify booleans
// example: printf("flag: %s\n", HH_STRINGIFY_BOOL(flag));
#define HH_STRINGIFY_BOOL(x) ((x) ? "true" : "false")

// standard assertion with variadic error message
#define HH_ASSERT(cond, ...) do { if(cond) break; HH_ERR(__VA_ARGS__); assert(cond); } while(0)
#define HH_ASSERT_UNREACHABLE(cond) HH_ASSERT(cond, "Unreachable!")
// this is an assertion that will execute the statement 
// or block that follows it before exiting
#define HH_ASSERT_BEFORE(cond) for(; !(cond); assert(cond))
// use this for unreachable branches
// for example, default cases in switch blocks
#define HH_UNREACHABLE HH_ASSERT_UNREACHABLE(false)

// wrappers that assert allocation success
void*
hh_malloc_checked(size_t size);
void*
hh_calloc_checked(size_t num, size_t size);

// Adapted from...
// stb_ds.h - v0.67 - public domain data structures - Sean Barrett 2019

#define hh_darrheader(arr)     (((hh_darrheader_t*) arr) - 1)
#define hh_darrnew(arr)        ((arr) = HH_H__impl_darrnew(HH_ARR_CAP_DEFAULT, sizeof(*arr)))
#define hh_darrgrow(arr, n)    (HH_H__impl_darrgrow((void**) &(arr), (n), sizeof(*(arr))), (arr))
// PUBLIC API
#define hh_darrclear(arr)      ((arr == NULL) ? 0 : (hh_darrheader(arr)->len = 0))
#define hh_darrfree(arr)       ((void) ((arr) ? free(hh_darrheader(arr)) : (void) 0), (arr) = NULL)
#define hh_darrlast(arr)       ((arr)[hh_darrheader(arr)->len - 1])
#define hh_darrput(arr, val)   ((void) hh_darrgrow(arr, 1), (arr)[(hh_darrheader(arr)->len)++] = (val))
#define hh_darrpop(arr)        ((arr)[--(hh_darrheader(arr)->len)])
#define hh_darradd(arr, n)     (HH_H__impl_darradd((void**)&(arr), (n), sizeof *(arr)))
#define hh_darrlen(arr)        ((arr == NULL) ? 0 : hh_darrheader(arr)->len)
#define hh_darrcap(arr)        ((arr == NULL) ? 0 : hh_darrheader(arr)->cap)
// returns truthy if swap succeeded
// swap fails on empty dynamic arrays
#define hh_darrswap(arr, i, j) (HH_H__impl_darrswap((arr), (i), (j)))
#define hh_darrswapdel(arr, i) ((i) < hh_darrlen(arr) ? (HH_H__impl_darrswap((arr), (i), hh_darrlen(arr) - 1), hh_darrpop(arr)) : NULL)

#define hh_darrputstr(arr, str) do { \
		if(hh_darrlen(arr) == 0 || (hh_darrlen(arr) != 0 && hh_darrlast(arr) != '\0')) hh_darrput(arr, '\0'); \
		assert(hh_darrlast(arr) == '\0'); \
		size_t _tmp = hh_darradd(arr, strlen(str)) - 1; \
		strcpy((arr) + _tmp, (str)); \
	} while(0)

// hh_path_alloc
// [in const] raw: a cstr representing a raw path
// return: heap-allocated dynamic array containing the normalized path
// This function creates a normalized path where...
// * relative paths are made absolute // TODO: Currently, only initial . and .. are considered
// * backslashes "\\" are converted to forward slashes "/"
// * (WINDOWS ONLY) volume names are capitalized "c:" -> "C:"
// * final slashes are stripped
char*
hh_path_alloc(const char* raw);
// hh_path_exists
// [in const] path: a path originally constructed with hh_path_alloc
// return: truthy if path exists, false otherwise
bool
hh_path_exists(const char* path);
// hh_path_is_file
// [in const] path: a path originally constructed with hh_path_alloc
// return: truthy if path exists and points to a file, false otherwise
bool
hh_path_is_file(const char* path);
// hh_path_is_root
// [in const] path: a path originally constructed with hh_path_alloc
// return: truthy if path is a root path (eg. "C:/" or "/"), false otherwise
bool 
hh_path_is_root(const char* path);
// hh_path_join
// [in] path: a path originally constructed with hh_path_alloc
// [in] ...: any number of path elements that should be joined
// return: same as [in] path
// This is structued as a macro to avoid the case where a reallocation
// causes the input and output pointers to differ.
// NULL is returned when the provided path is NULL
#define hh_path_join(path, ...) ((path) = HH_H__impl_path_join((path), __VA_ARGS__, NULL))
// hh_path_name
// [in const] path: a path originally constructed with hh_path_alloc
// return: a pointer to the first character of the last path element
// This function does not allocate
const char*
hh_path_name(const char* path);
// hh_path_parent
// [in] path: a path originally constructed with hh_path_alloc
// return: same as [in] path
// Removes the final element from the path
// The return pointer is always equal to the given path
char*
hh_path_parent(char* path);
// hh_path_free
// [in] path: a path originally constructed with hh_path_alloc
// return: void
// Frees the path and sets it to NULL
#define hh_path_free hh_darrfree

// each enumeration represents a major release of the C standard
// this allows you to check the standard at runtime
enum hh_edition {
	HH_EDITION_89 = 0,
	HH_EDITION_90 = 1,
	HH_EDITION_94 = 199409,
	HH_EDITION_99 = 199901,
	HH_EDITION_11 = 201112,
	HH_EDITION_17 = 201710,
	HH_EDITION_23 = 202311
};

// hh_edition_supported
// [in] ed: the standard you want to check
// return: truthy if the standard is supported, false otherwise
bool
hh_edition_supported(enum hh_edition ed);

// represents a non-owning view into a char buffer
typedef struct {
	const char* ptr;
	const char* delim;
	size_t len;
	size_t skips;
} hh_span_t;

// initialize a span and advance to the first token
// NOTE: the delimiter can be changed at any point without causing issues
bool
hh_span_init(hh_span_t* span, const char* ptr, const char* delim);
// advances the span to the start of the next token
// returns truthy unless the end of the buffer has been reached 
// and no more tokens remain
// delim represents the expected dividers between tokens (excluding whitespace)
bool
hh_span_next(hh_span_t* span);
// advance to the next line, skipping all remaining tokens
bool
hh_span_next_line(hh_span_t* span);
// returns true if the span's current token equals `other`
bool
hh_span_equals(const hh_span_t span, const char* other);
// parse a given type from the span
// does not advance to the next token
bool
hh_span_parse(const hh_span_t* span, const char* fmt, void* out);
// same as above, but advances the span ot the next token
// so hh_span_parse_next can be called in succession
bool
hh_span_parse_next(hh_span_t* span, const char* fmt, void* out);

// reads an entire file given by path
// returns a dynamic array with file contents (free with hh_darrfree)
// returns NULL on failure
char* 
hh_read_entire_file(const char* path);
// returns a pointer to the same string that 
// has been advanced past any initial whitespace
const char*
hh_skip_whitespace(const char* str);
// returns truthy when the `str` starts with `prefix`
bool
hh_has_prefix(const char* str, const char* prefix);
// returns truthy when the `str` ens with `suffix`
bool
hh_has_suffix(const char* str, const char* suffix);
//
#endif // HH_H__

//
//
//

//
//
//

//
//
//

//
//
//

//
//
//

#ifdef HH_H__
// remove definition of this helper macro
#undef HH_STRINGIFY_HELPER

// initial capacity of dynamic array
#ifndef HH_ARR_CAP_DEFAULT
#define HH_ARR_CAP_DEFAULT 16
#endif // not HH_ARR_CAP_DEFAULT

// internal array components
typedef struct { 
	size_t len, cap, elem_size; 
} hh_darrheader_t;

// helper functions for dynamic array
void*
HH_H__impl_darrnew(size_t cap, size_t elem_size);
void 
HH_H__impl_darrgrow(void** arrp, size_t n, size_t elem_size);
size_t
HH_H__impl_darradd(void** arrp, size_t n, size_t elem_size);
bool 
HH_H__impl_darrswap(void* arrp, size_t i, size_t j);

// helper functions for hh_path
char*
HH_H__impl_path_join(char* path, ...);

// size of the span format specifier buffer
// should not have to modify
#ifndef HH_SPAN_BUF_LEN
#define HH_SPAN_BUF_LEN 32
#endif // not HH_SPAN_BUF_LEN

// NetBSD: getline.c,v 1.2 2014/09/16 17:23:50 christos Exp
ptrdiff_t // NO PREFIX STRIPPING
hh_getdelim(char** buf, size_t* bufsiz, int delimiter, FILE* fp);
ptrdiff_t // NO PREFIX STRIPPING
hh_getline(char** buf, size_t* bufsiz, FILE* fp);
//
#endif // HH_H__

#ifdef HH_IMPLEMENTATION
// platform-dependent includes
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#endif // _WIN32

void*
hh_malloc_checked(size_t size) {
	void* ptr = malloc(size);
	HH_ASSERT(ptr != NULL, "Failed to allocate %zu bytes.", size);
	return ptr;
}

void*
hh_calloc_checked(size_t num, size_t size) {
	void* ptr = calloc(num, size);
	HH_ASSERT(ptr != NULL, "Failed to allocate %zu bytes.", size);
	return ptr;
}

void*
HH_H__impl_darrnew(size_t cap, size_t elem_size) {
    void* arr = (((hh_darrheader_t*) calloc(1, sizeof(hh_darrheader_t) + elem_size * cap)) + 1);
    hh_darrheader(arr)->len = 0;
    hh_darrheader(arr)->cap = cap;
    hh_darrheader(arr)->elem_size = elem_size;
    return arr;
}

void 
HH_H__impl_darrgrow(void** arrp, size_t n, size_t elem_size) {
    if(*arrp == NULL) {
        hh_darrheader_t* hdr = calloc(1, sizeof(hh_darrheader_t) + elem_size * HH_MAX(n, HH_ARR_CAP_DEFAULT));
        assert(hdr != NULL);
        hdr->len = 0;
        hdr->cap = HH_MAX(n, HH_ARR_CAP_DEFAULT);
        hdr->elem_size = elem_size;
        *arrp = (void*) (hdr + 1);
        return;
    }
    hh_darrheader_t* hdr = hh_darrheader(*arrp);
	if(hdr->len + n >= hdr->cap) {
		while(hdr->len + n >= hdr->cap) hdr->cap *= 2;
		hdr = realloc(hdr, sizeof(hh_darrheader_t) + hdr->cap * hdr->elem_size);
        assert(hdr != NULL);
        *arrp = (void*) (hdr + 1);
	}
}

size_t
HH_H__impl_darradd(void** arrp, size_t n, size_t elem_size) {
	HH_H__impl_darrgrow(arrp, n, elem_size);
    size_t len = hh_darrlen(*arrp);
    if(n) {
        memset((char*) (*arrp) + len * elem_size, 0, elem_size * n);
        hh_darrheader(*arrp)->len = len + n;
    }
    return len;
}

bool 
HH_H__impl_darrswap(void* arrp, size_t i, size_t j) {
    if(i == j) return true;
	if(arrp == NULL) return false;
    size_t elem_size = hh_darrheader(arrp)->elem_size;
    char tmp[elem_size];
    memcpy(tmp, ((char*) arrp) + i * elem_size, elem_size);
    memcpy(((char*) arrp) + i * elem_size, ((char*) arrp) + j * elem_size, elem_size);
    memcpy(((char*) arrp) + j * elem_size, tmp, elem_size);
	return true;
}


char*
hh_path_alloc_impl(const char* raw) {
	char* path = NULL;
	hh_darrputstr(path, raw);
	if(path == NULL) return NULL;
	for(char* curr = path; *curr != '\0'; ++curr) if(*curr == '\\') *curr = '/';
	return path;
}

char*
hh_path_alloc(const char *raw) {
	char* raw_abs = NULL;
	char* path = NULL;
#ifdef _WIN32
    DWORD len = GetFullPathNameA(raw, 0, NULL, NULL);
    if(len == 0) return NULL;
    raw_abs = malloc(len);
    if(!raw_abs) return NULL;
    if(GetFullPathNameA(raw, len, raw_abs, NULL) == 0) {
        free(raw_abs);
        return NULL;
    }
	path = hh_path_alloc_impl(raw_abs);
	if(path && path[0] >= 'a' && path[0] <= 'z') path[0] -= ('a' - 'A');
#else
	if(raw[0] == '.' && (raw[1] == '/' || raw[1] == '\\' || raw[1] == '\0')) {
		raw_abs = realpath(".", NULL);
		path = hh_path_alloc_impl(raw_abs);
		hh_darrputstr(path, raw + 1);
	} else if(raw[0] == '.' && raw[1] == '.' && (raw[2] == '/' || raw[2] == '\\' || raw[2] == '\0')) {
		raw_abs = realpath("..", NULL);
		path = hh_path_alloc_impl(raw_abs);
		hh_darrputstr(path, raw + 2);
	} else path = hh_path_alloc_impl(raw);
#endif
	if(raw_abs) free(raw_abs);
	if(path == NULL) return NULL;
	for(char* curr = path; *curr != '\0'; ++curr) if(*curr == '\\') *curr = '/';
	(void) hh_darrpop(path);
	if(hh_darrlen(path) > 2 && hh_darrlast(path) == '/') (void) hh_darrpop(path);
	hh_darrput(path, '\0');
	return path;
}

bool
hh_path_exists(const char* path) {
	if(path == NULL) return false;
#ifdef _WIN32
	return _access(path, 0) == 0;
#else
	return access(path, 0) == 0;
#endif
}

bool
hh_path_is_file(const char* path) {
	if(path == NULL) return false;
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
#endif
}

bool 
hh_path_is_root(const char* path) {
	if(path == NULL) return false;
#ifdef _WIN32
    if(hh_darrlen(path) != 4) return false;
	if(path[0] < 'A' || path[0] > 'Z') return false;
	return path[1] == ':' && path[2] == '/';
#else
    return hh_darrlen(path) == 2 && path[0] == '/';
#endif
}

char* 
HH_H__impl_path_join(char* path, ...) {
    if(path == NULL) return NULL;
    va_list args;
    va_start(args, path);
    const char* sub;
    while((sub = va_arg(args, const char*)) != NULL) {
        if(sub[0] == '/' || sub[0] == '\\') ++sub;
        (void) hh_darrpop(path);
        if (hh_darrlast(path) != '/') hh_darrputstr(path, "/");
        hh_darrputstr(path, sub);
        if (hh_darrlen(path) > 2 && hh_darrlast(path) == '/') (void) hh_darrpop(path);
    }
    va_end(args);
    hh_darrput(path, '\0');
    return path;
}

const char*
hh_path_name(const char* path) {
	if(path == NULL) return NULL;
	const char *prev = path;
    for(const char *p = path; *p; ++p) if(*p == '/') prev = p + 1;
	if(prev[0] == '\0') return NULL;
    return prev;
}

char*
hh_path_parent(char* path) {
	if(path == NULL) return NULL;
	if(hh_path_is_root(path)) return NULL;
	while(hh_darrlast(path) != '/') (void) hh_darrpop(path);
#ifdef _WIN32
	if(hh_darrlen(path) == 3 && path[0] >= 'A' && path[0] <= 'Z' && path[1] == ':' && path[2] == '/') {
		if(hh_darrlen(path) == 4) hh_darrfree(path);
		else hh_darrput(path, '\0');
	} else {
		hh_darrpop(path);
		hh_darrput(path, '\0');
	}
#else
	if(hh_darrlen(path) == 1 && path[0] == '/') {
		if(hh_darrlen(path) == 2) hh_darrfree(path);
		else hh_darrput(path, '\0');
	} else {
		(void) hh_darrpop(path);
		hh_darrput(path, '\0');
	}
#endif
	return path;
}

// calculate edition using preprocessor
#ifdef __STDC__
#define HH_EDITION 0L
#ifdef __STDC_VERSION__
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 1L
#if(__STDC_VERSION__ >= 199409L)
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 199409L
#endif // 199409L
#if(__STDC_VERSION__ >= 199901L)
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 199901L
#endif // 199901L
#if(__STDC_VERSION__ >= 201112L)
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 201112L
#endif // 201112L
#if(__STDC_VERSION__ >= 201710L)
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 201710L
#endif // 201710L
#if(__STDC_VERSION__ >= 202311L)
#ifdef HH_EDITION
#undef HH_EDITION
#endif // HH_EDITION
#define HH_EDITION 202311L
#endif // 202311L
#endif // __STDC_VERSION__
#endif // __STD__

bool
hh_edition_supported(enum hh_edition ed) {
	return HH_EDITION >= (long) (ed);
}

bool
hh_span_init(hh_span_t* span, const char* ptr, const char* delim) {
	span->ptr = ptr;
	span->delim = delim;
	span->len = 0;
	span->skips = 0;
	return hh_span_next(span);
}

bool
hh_span_next(hh_span_t* span) {
	const char* ptr = span->ptr + span->len + span->skips;
	span->len = 0;
	span->skips = 0;
	while(*ptr && strchr(" \t\r\n", *ptr)) ++ptr;
	span->ptr = ptr;
	if(!*ptr) return false;
	size_t delim_len = span->delim ? strlen(span->delim) : 0;
	if(span->delim) while(*ptr && strncmp(ptr, span->delim, delim_len) != 0 && !strchr(" \t\r\n", *ptr)) ++ptr;
	else while(*ptr && !strchr(" \t\r\n", *ptr)) ++ptr;
	span->len = (size_t) (ptr - span->ptr);
	if(span->len == 0) return false;
	if(span->delim && strncmp(ptr, span->delim, delim_len) == 0) span->skips = delim_len;
	return true;
}

bool
hh_span_next_line(hh_span_t* span) {
	const char* ptr = span->ptr + span->len + span->skips;
	span->len = 0;
	span->skips = 0;
	while(*ptr && *ptr != '\n') ++ptr;
	if(*ptr == '\n') ++ptr;
	span->ptr = ptr;
	if(*ptr == '\0') return false;
	return hh_span_next(span);
}

bool
hh_span_equals(const hh_span_t span, const char* other) {
	size_t len = strlen(other);
	if(span.len != len) return false;
	return strncmp(span.ptr, other, span.len) == 0;
}

bool
hh_span_parse(const hh_span_t* span, const char* fmt, void* out) {
	HH_ASSERT(fmt[0] == '%', "Unsupported format specifier [%s].", fmt);
	static char fmt_buf[HH_SPAN_BUF_LEN + 1];
	snprintf(fmt_buf, HH_SPAN_BUF_LEN, "%%%zu%s", span->len, &fmt[1]);
	return sscanf(span->ptr, fmt_buf, out);
}

bool
hh_span_parse_next(hh_span_t* span, const char* fmt, void* out) {
	return hh_span_parse(span, fmt, out) && hh_span_next(span);
}

#define HH_CHECK_STREAM(stream, cond, ...) if(!(cond)) { \
		fclose((stream)); \
		HH_ERR(__VA_ARGS__); \
	} \
	if(!(cond))

char* 
hh_read_entire_file(const char* path) {
    FILE* f = fopen(path, "rb");
	if(f == NULL) {
		HH_ERR("Failed to open file at path [%s].", path);
		return NULL;
	}
	HH_CHECK_STREAM(f, !fseek(f, 0, SEEK_END), 
		"Failed to seek to end of file while reading [%s].", path) return NULL;
    long size_temp = ftell(f);
	HH_CHECK_STREAM(f, size_temp >= 0, "Failed to read file size [%s].", path) 
		return NULL;
	unsigned long size = (unsigned long) size_temp;
    rewind(f);
	char* buf = NULL;
	(void) hh_darradd(buf, size);
	HH_CHECK_STREAM(f, buf != NULL, "Failed to allocate buffer for file contents [%s].", path) 
		return NULL;
    size_t read_size = fread(buf, 1, size, f);
	HH_CHECK_STREAM(f, read_size == (size_t) size, "Failed to read entire file into buffer [%s].", path) {
		hh_darrfree(buf);
		return NULL;
	}
	(void) hh_darradd(buf, '\0');
    fclose(f);
    return buf;
}
#undef HH_CHECK_STREAM

const char*
hh_skip_whitespace(const char* ptr) {
	while(strchr(" \t\r\n", *ptr) && (*ptr) != '\0') ++ptr;
	return ptr;
}

bool
hh_has_prefix(const char* str, const char* prefix) {
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool
hh_has_suffix(const char* str, const char* suffix) {
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);
    return len_suffix <= len_str && !strcmp(str + len_str - len_suffix, suffix);
}

/*-
 * Copyright (c) 2011 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

ptrdiff_t
hh_getdelim(char** buf, size_t* bufsiz, int delimiter, FILE* fp) {
	char *ptr, *eptr;
	if(*buf == NULL || *bufsiz == 0) {
		*bufsiz = BUFSIZ;
		if ((*buf = (char*) malloc(*bufsiz)) == NULL) return -1;
	}
	for(ptr = *buf, eptr = *buf + *bufsiz;;) {
		int c = fgetc(fp);
		if(c == -1) {
			if(feof(fp)) {
				ptrdiff_t diff = ptr - *buf;
				if(diff != 0) {
					*ptr = '\0';
					return diff;
				}
			}
			return -1;
		}
		*ptr++ = (char) c;
		if(c == delimiter) {
			*ptr = '\0';
			return ptr - *buf;
		}
		if(ptr + 2 >= eptr) {
			char *nbuf;
			size_t nbufsiz = *bufsiz * 2;
			ptrdiff_t d = ptr - *buf;
			if((nbuf = (char*) realloc(*buf, nbufsiz)) == NULL) return -1;
			*buf = nbuf;
			*bufsiz = nbufsiz;
			eptr = nbuf + nbufsiz;
			ptr = nbuf + d;
		}
	}
}

ptrdiff_t
hh_getline(char** buf, size_t* bufsiz, FILE* fp) {
	return hh_getdelim(buf, bufsiz, '\n', fp);
}
//
#endif // HH_IMPLEMENTATION

#ifndef HH_H__STRIP_PREFIXES
//
#define HH_H__STRIP_PREFIXES
//
#ifdef HH_STRIP_PREFIXES
#define MAX HH_MAX
#define MIN HH_MIN
#define ARR_LEN HH_ARR_LEN
#define UNUSED HH_UNUSED
#define FALLTHROUGH HH_FALLTHROUGH
#define DBG HH_DBG
#define MSG HH_MSG
#define ERR HH_ERR
#define DBG_BLOCK HH_DBG_BLOCK
#define MSG_BLOCK HH_MSG_BLOCK
#define ERR_BLOCK HH_ERR_BLOCK
#define STRINGIFY HH_STRINGIFY
#define STRINGIFY_BOOL HH_STRINGIFY_BOOL
#define ASSERT_BEFORE HH_ASSERT_BEFORE
#define ASSERT HH_ASSERT
#define ASSERT_UNREACHABLE HH_ASSERT_UNREACHABLE
#define UNREACHABLE HH_UNREACHABLE
#define malloc_checked hh_malloc_checked
#define calloc_checked hh_calloc_checked
#define darrclear hh_darrclear
#define darrfree hh_darrfree
#define darrlast hh_darrlast
#define darrput hh_darrput
#define darrpop hh_darrpop
#define darradd hh_darradd
#define darrlen hh_darrlen
#define darrcap hh_darrcap
#define darrswap hh_darrswap
#define darrswapdel hh_darrswapdel
#define darrputstr hh_darrputstr
#define path_alloc hh_path_alloc
#define path_exists hh_path_exists
#define path_is_file hh_path_is_file
#define path_is_root hh_path_is_root
#define path_join hh_path_join
#define path_name hh_path_name
#define path_parent hh_path_parent
#define path_free hh_path_free
#define edition_supported hh_edition_supported
#define EDITION_89 HH_EDITION_89
#define EDITION_90 HH_EDITION_90
#define EDITION_94 HH_EDITION_94
#define EDITION_99 HH_EDITION_99
#define EDITION_11 HH_EDITION_11
#define EDITION_17 HH_EDITION_17
#define EDITION_23 HH_EDITION_23
#define span_t hh_span_t
#define span_init hh_span_init
#define span_next hh_span_next
#define span_next_line hh_span_next_line
#define span_parse hh_span_parse
#define span_parse_next hh_span_parse_next
#define span_equals hh_span_equals
#define read_entire_file hh_read_entire_file
#define skip_whitespace hh_skip_whitespace
#define has_prefix hh_has_prefix
#define has_suffix hh_has_suffix
//
#endif // HH_STRIP_PREFIXES
//
#endif // HH_H__STRIP_PREFIXES
