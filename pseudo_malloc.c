#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include "bit_map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>







void* pseudo_malloc(size_t size,BuddyAllocator* alloc){
    void* result;
    printf("\nentro nella malloc\n");
    
   
    if (size<=0){
        printf("\ndimensione non corretta per un'allocazione\n");
        return NULL;
    }
    int total_size=size+8;
    if (size<1024){
        result=BuddyAllocator_malloc(alloc,total_size);
        if (!result){
            printf("\nerrore nell'allocamento con BuddyAllocator\n");
            return NULL;
            }
        
        
    }else{
        result=mmap(NULL,total_size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
        if (result == MAP_FAILED) {
            perror("\nErrore nell'allocazione con mmap\n");
            return NULL;
        }
        *((int*)result)=1;
        *(((int*)result)+1)=size;
    }
    *((size_t*)result)=total_size;
    return (void*)(((int*)result)+2);
}
void pseudo_free(void*ptr,BuddyAllocator* alloc){
    if(!ptr){
        printf("\nerrore, impossibile liberare una risorsa null\n");
        return;
    }
   int bit=*(((int*)ptr)-2);
    if(bit==0){
        BuddyAllocator_free(alloc,ptr);
    }else if (bit==1){
        if (munmap(ptr,(size_t) bit)==-1){
            perror("\nerrore nella rimozione della memoria mappata\n");
        }
    }
}