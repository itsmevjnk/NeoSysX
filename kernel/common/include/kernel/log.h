#ifndef __KERNEL_LOG_H
#define __KERNEL_LOG_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// TODO: implement proper kernel logging (kmsg style); these are just macros to append tags
#define LOG_INFO(fmt, ...) printf("[I] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) printf("[W] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("[E] " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) printf("[D] " fmt "\n", ##__VA_ARGS__)

#endif
