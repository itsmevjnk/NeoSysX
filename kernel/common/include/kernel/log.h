#ifndef __KERNEL_LOG_H
#define __KERNEL_LOG_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// TODO: implement proper kernel logging (kmsg style); these are just macros to append tags
#define LOG_INFO(fmt, ...) printf("\x1B[92m[I]\x1B[0m " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) printf("\x1B[93m[W]\x1B[0m " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("\x1B[91m[E]\x1B[0m " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) printf("\x1B[95m[D]\x1B[0m " fmt "\n", ##__VA_ARGS__)

#endif
