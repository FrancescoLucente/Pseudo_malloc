#pragma once
#include "bit_map.h"

#define MAX_LEVELS 16





typedef struct  {
  int num_levels;
  char* memory; 
  int memory_size;
  int min_bucket_size;
  BitMap bitmap; 
} BuddyAllocator;


int BuddyAllocator_calcSize(int num_levels);

int BuddyAllocator_init(BuddyAllocator *alloc, int num_levels, char *buffer,int buffer_size, char *memory,int memory_size, int min_bucket_size);


int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);



void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);


void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
void BuddyAllocator_print(BuddyAllocator* buddy);