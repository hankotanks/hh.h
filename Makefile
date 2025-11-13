CC ?= cc

PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CFLAGS := -ggdb3 -std=c99 -Wall -Wextra -Wconversion -Wpedantic -I$(PROJECT_ROOT)
CFLAGS += -DPROJECT_ROOT=\"$(PROJECT_ROOT)\"

tests: tests.c
	$(RM) tests
	$(CC) $(CFLAGS) $^ -o $@