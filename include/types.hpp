#ifndef MG_TYPES_HPP
#define MG_TYPES_HPP

#include <stdint.h>

typedef unsigned int u32;
typedef int s32;
typedef unsigned char u8;
typedef signed char s8;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;

#define ARR_SIZE(array) (sizeof(array) / sizeof(*array))

#endif
