#ifndef HH_ARGS_H__
#define HH_ARGS_H__

#include <stdbool.h>

//
// HH_ARGS

// template for parsing functions
typedef void* (*hh_args_parse_t)(char*, int*);
typedef void  (*hh_args_clean_t)(void*);

bool
hh_args_parse(int argc, char* argv[]);
void
hh_args_clean(void);

// TODO: This needs a major rework with...
// - help message print outs
// - ability to leave flag_/flag_long_ NULL
// - etc

#ifdef HH_ARGS

struct hh_args_t {
#define HH_ARG_REQ(ty_, default_, name_, ...) ty_ name_;
#define HH_ARG_OPT(ty_, default_, name_, ...) ty_ name_;
	HH_ARGS
#undef HH_ARG_REQ
#undef HH_ARG_OPT
	void* inner_;
};

static struct hh_args_t HH_H__hh_args = {
#define HH_ARG_REQ(ty_, default_, name_, ...) .name_ = default_,
#define HH_ARG_OPT(ty_, default_, name_, ...) .name_ = default_,
	HH_ARGS
#undef HH_ARG_REQ
#undef HH_ARG_OPT
	.inner_ = NULL
}; extern struct hh_args_t* const hh_args;

#endif // HH_ARGS
#endif // HH_ARGS_H__


#ifdef HH_IMPLEMENTATION
#ifdef HH_ARGS

struct hh_args_t* const hh_args = &HH_H__hh_args;

bool
hh_args_parse(int argc, char* argv[]) {
	HH_ASSERT(argc > 0 && argv != NULL, "Unreachable!");
#define HH_ARG_REQ(...) + 1
#define HH_ARG_OPT(...) + 0
	int argc_min = 0 HH_ARGS;
#undef HH_ARG_REQ
#undef HH_ARG_OPT
	if(argc < 1 + argc_min) {
		HH_ERR("Expected at least %d CLI arguments, received %d.", 1 + argc_min, argc);
		return false;
	}
	int i = 1;
#define HH_ARG_REQ(ty_, default_, name_, flag_, flag_long_, desc_, parser_, ...) hh_args->name_ = (ty_) parser_(argv[i++]);
#define HH_ARG_OPT(...)
    HH_ARGS
#undef HH_ARG_REQ
#undef HH_ARG_OPT
	int ok;
	hh_args_parse_t temp;
	for(i = 1 + argc_min; i < argc; i++) {
#define HH_ARG_REQ(...)
#define HH_ARG_OPT(ty_, default_, name_, flag_, flag_long_, desc_, parser_, ...) \
		if(!strcmp(argv[i], flag_) || !strcmp(argv[i], flag_long_)) { \
			ok = 1; \
			temp = parser_; \
			if(temp) { \
				if(i + 1 >= argc) { \
					HH_ERR("Expected value for [" flag_ "] flag."); \
					return false; \
				} \
				hh_args->name_ = (ty_) (temp)(argv[++i], &ok); \
				if(!ok) { \
					HH_ERR("Unable to parse [%s] as %s.", argv[i], HH_STR(ty_)); \
					return false; \
				} \
			} else hh_args->name_ = (ty_) ((uintptr_t) (!(hh_args->name_))); \
			continue; \
		}
        HH_ARGS
#undef HH_ARG_REQ
#undef HH_ARG_OPT
        HH_ERR("Failed to parse argument [%s].", argv[i]);
    }
	return true;
}

void
hh_args_clean(void) {
	hh_args_clean_t temp;
#define HH_ARG_REQ(ty_, default_, name_, flag_, flag_long_, desc_, parser_, clean_) \
	temp = clean_; \
	if(temp) (temp)(hh_args->name_);
#define HH_ARG_OPT(ty_, default_, name_, flag_, flag_long_, desc_, parser_, clean_) \
	temp = clean_; \
	if(temp) (temp)((void*) hh_args->name_);
    HH_ARGS
#undef HH_ARG_REQ
#undef HH_ARG_OPT
}

#endif // HH_ARGS
#endif // HH_IMPLEMENTATION
