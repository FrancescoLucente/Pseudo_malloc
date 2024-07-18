#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include "bit_map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>


#define SMALL_ALLOC 1024
#define LEVELS 15
#define BITMAP_SIZE 4096
#define MIN_BLOCK 32
#define MEMORY_SIZE (1024*1024)

static char bitmap_buffer[BITMAP_SIZE];
static char memory[MEMORY_SIZE];
BuddyAllocator alloc;
BitMap bitmap;
int initialized=0;

void init_system(){
    if (initialized) return;
    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory, MIN_BLOCK);
    BitMap_init(&bitmap, MEMORY_SIZE / MIN_BLOCK, (uint8_t*)bitmap_buffer);

    for (int i = 0; i < BITMAP_SIZE; i++) {
        bitmap_buffer[i] = 0;
    }
    initialized=1;

        
}

void* pseudo_malloc(size_t size){
    if (!initialized){
        init_system();
    }
    if (size<=0){
        printf("dimensione non corretta per un'allocazione");
        return;
    }
    if (size<SMALL_ALLOC){

    }else{

    }
}
void* pseudo_free(void*ptr){

}