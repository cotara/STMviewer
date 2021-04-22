#ifndef TYPES_H
#define TYPES_H

#include "stdint.h"

union union_float {
    float fl;
    char bytes[4];
};

union union_uint16 {
    uint16_t in;
    char bytes[2];
};

union union_uint32 {
    uint32_t in;
    char bytes[4];
};

union union_uint64 {
    uint64_t in;
    char bytes[8];
};

union union_double {
    double db;
    char bytes[8];
};

#endif // TYPES_H
