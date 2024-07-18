#pragma once
#include <stddef.h>
void init_system();
void* pseudo_malloc(size_t size);
void pseudo_free(void* ptr);
void close_system();