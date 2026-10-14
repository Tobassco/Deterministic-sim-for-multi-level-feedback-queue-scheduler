#ifndef PARSE_H
#define PARSE_H

#include "process.h"
#include <stdint.h>

typedef struct {
    const char *filename;
    int num_levels;
    uint32_t *quantums;
    uint32_t boost_interval;
} Args;

int parse_args(int argc, char *argv[], Args *out);
Process **load_processes(const char *filename, int *count_out);

#endif