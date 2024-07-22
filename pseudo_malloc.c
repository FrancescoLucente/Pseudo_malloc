#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include "bit_map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>







void* pseudo_malloc(int size,BuddyAllocator* alloc){
    void* result;
    if (size<=0){
        printf("\ndimensione non corretta per un'allocazione\n");
        return NULL;
    }
    int total_size=size+8;
    if (size<1024){
        if (!alloc){
            printf("\n inserire buddy allocator valido");
            return NULL;
        }
        result=BuddyAllocator_malloc(alloc,total_size);
        if (!result){
            printf("\nerrore nell'allocamento con BuddyAllocator\n");
            return NULL;
            }
        return result;
        
    }else{
        result=mmap(NULL,total_size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
        if (result == MAP_FAILED) {
            perror("\nErrore nell'allocazione con mmap\n");
            return NULL;
        }
        *((int*)result)=1;
        *(((int*)result)+1)=total_size;
    }
    printf("\nnel puntatore %p ho assegnato il valore %d, nel puntatore %p ho assegnato il valore %d e restituisco il puntatore %p\n",
        (void*)((int*)result), *((int*)result),(void*)(((int*)result)+1),*(((int*)result)+1),(void*)(((int*)result)+2));
    return (void*)(((int*)result)+2);
}
void pseudo_free(void**memo,BuddyAllocator* alloc){
    if(*memo==NULL){
        printf("\nerrore, impossibile liberare una risorsa null\n");
        return;
    }
    void* ptr=*memo;
    printf("il puntatore è a questo indirizzo: %p", (void*)ptr);
   ptr=ptr-2*sizeof(int);
   printf("\n dopo la sottrazione il puntatore è a questo indirizzo: %p\n", (void*)ptr);
   int* mem=(int*)ptr;
   
   
   printf("il bit per capire con cosa deallocare è %d e il secondo valore è %d ",mem[0],mem[1]);
    if(mem[0]==0){
        
        BuddyAllocator_free(alloc,ptr);
        *memo=NULL;
    }else if (mem[0]==1){
        if (munmap((void*)ptr,mem[1])){
            perror("\nerrore nella rimozione della memoria mappata\n");
        }
        *memo=NULL;
    }else{
        printf("\nerrore indefinito nella free\n");
    }
}