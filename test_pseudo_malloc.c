#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>

#define SMALL_ALLOC 1024
#define LEVELS 15
#define BITMAP_SIZE 8192
#define MIN_BLOCK 32
#define MEMORY_SIZE (1024*1024)

static char bitmap_buffer[BITMAP_SIZE];
static char memory[MEMORY_SIZE];
BuddyAllocator alloc;
char* memory_fake;
char* bitmap_fake;
int main(int argc, char** argv){
    printf("Proviamo a inizializzare il sistema con valori non appropriati\n");
    int control;
    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory, 0,MIN_BLOCK);
     BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory, MEMORY_SIZE,9);
    BuddyAllocator_init(&alloc, LEVELS+10, bitmap_buffer, BITMAP_SIZE, memory, MEMORY_SIZE, MIN_BLOCK);
    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory_fake, MEMORY_SIZE, MIN_BLOCK);
    BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, 0, memory, MEMORY_SIZE, MIN_BLOCK);
    BuddyAllocator_init(&alloc, LEVELS, bitmap_fake, BITMAP_SIZE, memory, MEMORY_SIZE, MIN_BLOCK);
    printf("\n\n\nInizializzazione del sistema...\n");

    // Debug output per verificare i parametri
    printf("LEVELS: %d, BITMAP_SIZE: %zu, MEMORY_SIZE: %zu, MIN_BLOCK: %zu\n", 
           LEVELS, sizeof(bitmap_buffer), sizeof(memory), MIN_BLOCK);


    control=BuddyAllocator_init(&alloc, LEVELS, bitmap_buffer, BITMAP_SIZE, memory, MEMORY_SIZE, MIN_BLOCK);
    if (control==0) return 0;
    printf("\n\n\n Proviamo ad assegnare un po' di valori non corretti\n");
    void* ptr=pseudo_malloc(0,&alloc);
    ptr=pseudo_malloc(-2,&alloc);

    printf("\n\n\nprima prova con buddy \n");
    int* ptr1=(int*)pseudo_malloc(10*sizeof(int),&alloc);
    printf("questo è il puntatore che ci è stato dato %p", (void*)ptr1);
    if (!ptr1) {
        printf("errore nel buddy allocator");
        exit(-1);
    }
    BuddyAllocator_print(&alloc);
    printf("\n\n\n Ora alloco il secondo\n");
    int* ptr12=pseudo_malloc(10*sizeof(int),&alloc);
    printf("questo è il puntatore che ci è stato dato %p\n", (void*)ptr12);
    BuddyAllocator_print(&alloc);
    printf("\nallocazione andata bene! proviamo ad accedere\n");
    for (int i=0;i<10;i++){
        ptr1[i]=i;
        printf(" %d",ptr1[i]);
    }
    printf("\n ora accedo al secondo\n");
    for (int i=0;i<10;i++){
        ptr12[i]=i;
        printf(" %d",ptr12[i]);
    }
    printf("\nprovo ad allocare un buddy all'ultimo livello\n");
    int* ptr13=(int*)pseudo_malloc(3*sizeof(int),&alloc);
    BuddyAllocator_print(&alloc);
    printf("\nprovo pseudo_free con risorsa allocata con buddy\n");
    pseudo_free(&ptr1,&alloc);
    BuddyAllocator_print(&alloc);
    printf("\nprovo pseudo_free con terza risorsa allocata con buddy\n");
    pseudo_free(&ptr13,&alloc);
    BuddyAllocator_print(&alloc);
    printf("\nprovo pseudo_free con seconda risorsa allocata con buddy\n");
    pseudo_free(&ptr12,&alloc);
    BuddyAllocator_print(&alloc);
     printf("\nprovo pseudo_free con risorsa già deallocata\n");
    pseudo_free(&ptr12,&alloc);
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
    pseudo_free(&ptr2,NULL);
    printf("\n free con munmap riuscita!");
    printf("\nprovo pseudo_free con risorsa già deallocata\n");
    pseudo_free(&ptr2,NULL);

}