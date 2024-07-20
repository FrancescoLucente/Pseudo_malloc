#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>

#define SMALL_ALLOC 1024
#define LEVELS 15
#define BITMAP_SIZE 4096
#define MIN_BLOCK 32
#define MEMORY_SIZE (1024*1024)

static char bitmap_buffer[BITMAP_SIZE];
static char memory[MEMORY_SIZE];
BuddyAllocator alloc;
int main(int argc, char** argv){
    
    printf("Inizializzazione del sistema...\n");

    // Debug output per verificare i parametri
    printf("LEVELS: %d, BITMAP_SIZE: %zu, MEMORY_SIZE: %zu, MIN_BLOCK: %zu\n", 
           LEVELS, sizeof(bitmap_buffer), sizeof(memory), MIN_BLOCK);


    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory, MEMORY_SIZE, MIN_BLOCK);


    printf("prima prova con buddy \n");
    int* ptr1=(int*)pseudo_malloc(10*sizeof(int),&alloc);
    if (!ptr1) {
        printf("errore nel buddy allocator");
        exit(-1);
    }
    BuddyAllocator_print(&alloc);
    printf("\nallocazione andata bene! proviamo ad accedere\n");
    for (int i=0;i<10;i++){
        ptr1[i]=i;
        printf(" %d",ptr1[i]);
    }
    printf("\nprovo pseudo_free con risorsa allocata con buddy\n");
    pseudo_free(ptr1,&alloc);
    BuddyAllocator_print(&alloc);
    printf("\nprima prova con mmap\n");
    int* ptr2=(int*)pseudo_malloc(256*sizeof(int),NULL);
    if (!ptr2) {
        printf("errore nel mmap");
        exit(-1);
    }
     printf("\nallocazione andata bene! proviamo ad accedere\n");
    for (int i=0;i<256;i++){
        ptr2[i]=i;
        printf(" %d",ptr2[i]);
    }
    printf("\nprovo pseudo_free\n");
    pseudo_free(ptr2,NULL);
    printf("\n free con munmap riuscita!");

}