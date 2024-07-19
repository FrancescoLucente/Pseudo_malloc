#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv){
    printf("prima prova con buddy \n");
    int* ptr1=(int*)pseudo_malloc(10*sizeof(int));
    if (!ptr1) {
        printf("errore nel buddy allocator");
        exit(-1);
    }
    for (int i=0;i<10;i++){
        ptr1[i]=i;
        printf(" %d",ptr1[i]);
    }
    printf("\nprovo pseudo_free");
    pseudo_free(ptr1);
    printf("prima prova con mmap\n");
    int* ptr2=(int*)pseudo_malloc(256*sizeof(int));
    if (!ptr2) {
        printf("errore nel buddy allocator");
        exit(-1);
    }
    for (int i=0;i<256;i++){
        ptr2[i]=i;
        printf(" %d",ptr2[i]);
    }
    printf("\nprovo pseudo_free");
    pseudo_free(ptr2);

}