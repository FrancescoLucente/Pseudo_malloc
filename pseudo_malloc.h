#pragma once
#include <stddef.h>
#include "buddy_allocator.h"




void init_system();
void* pseudo_malloc(int size,BuddyAllocator* alloc);
void pseudo_free(void** ptr,BuddyAllocator* alloc);
void close_system();