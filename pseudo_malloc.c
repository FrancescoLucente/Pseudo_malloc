#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include "bit_map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>


#define SMALL_ALLOC 1024
#define LEVELS 15
#define BITMAP_SIZE 8096
#define MIN_BLOCK 32
#define MEMORY_SIZE (1024*1024)

static char bitmap_buffer[BITMAP_SIZE];
static char memory[MEMORY_SIZE];
BuddyAllocator alloc;
int initialized=0;

void init_system(){
    if (initialized) return;
    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory,MEMORY_SIZE, MIN_BLOCK);
    initialized=1;
}

void* pseudo_malloc(size_t size){
    void* result;
    if (!initialized){
        init_system();
    }
    if (size<=0){
        printf("dimensione non corretta per un'allocazione");
        return NULL;
    }
    size_t total_size=size+sizeof(size_t);
    if (size<SMALL_ALLOC){
        result=BuddyAllocator_malloc(&alloc,total_size);
        if (!result){
            printf("errore nell'allocamento con BuddyAllocator");
            return NULL;
            }
        
        
    }else{
        result=mmap(NULL,total_size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
        if (result == MAP_FAILED) {
            perror("Errore nell'allocazione con mmap");
            return NULL;
        }
        
    }
    *((size_t*)result)=total_size;
    return result+sizeof(size_t);
}
void pseudo_free(void*ptr){
    if(!ptr){
        printf("errore, impossibile liberare una risorsa null");
        return;
    }
    size_t size=(*((size_t*)ptr));
    if(size-sizeof(size_t)<SMALL_ALLOC){
        BuddyAllocator_free(&alloc,ptr);
    }else{
        if (munmap(ptr,size)==-1){
            perror("errore nella rimozione della memoria mappata");
        }
    }
}