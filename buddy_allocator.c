#include <assert.h>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>

#include "buddy_allocator.h"
#include "bit_map.h"



int levelIdx(size_t idx) { return (int)floor(log2(idx+1)); };

int buddyIdx(int idx) {
  if (idx==0) return 0;
  if (idx & 0x1) {
    return idx+1;
  }
  return idx-1;
}
int firstIdx(int level){
  return (1<<level)-1;
}
int lastIdx(int level){
  return (1<<(level+1))-2;
}
int parentIdx(int idx) { return (idx-1)/2; }

int startIdx(int idx) { return firstIdx(levelIdx(idx)); }

int find_level(BuddyAllocator* alloc ,int size){
  int level=0;
  int ret=-1;
  while (level<=alloc->num_levels){
    if (size>alloc->memory_size>>(level+1)) return level;
    if (level==alloc->num_levels) return level;
    level++;
  }
  return ret;
}


int count_childs(int idx,BuddyAllocator* alloc){
  if (levelIdx(idx)>alloc->num_levels) return 0;
  if (levelIdx(idx)==alloc->num_levels) return 1;
  return count_childs(2*idx+1,alloc)+count_childs(2*idx+2,alloc);
}


int count_parents(int idx,BuddyAllocator* alloc){
  if (levelIdx(idx)<=0) return 0;
  return 1+count_parents(parentIdx(idx),alloc);
  }




int BuddyAllocator_calcSize(int num_levels) {
  int list_items = (1 << (num_levels))-1; // maximum number of allocations, used
                                          // to determine the max list items
  
  return list_items;
}
void BuddyAllocator_update_childs(BuddyAllocator* alloc,int idx,int value){
  
  if (idx>=alloc->bitmap.num_bits-1||2*idx+1>=alloc->bitmap.num_bits-1) return;
  BitMap_setBit(&(alloc->bitmap),2*idx+1,value);
  BitMap_setBit(&(alloc->bitmap),2*idx+2,value);
  BuddyAllocator_update_childs(alloc,2*idx+1,value);
  BuddyAllocator_update_childs(alloc,2*idx+2,value);
}
void BuddyAllocator_merge(BuddyAllocator* alloc, int idx){
  if (idx<=0) return;
  int buddy=buddyIdx(idx);
  if (BitMap_bit(&(alloc->bitmap),buddy)==0){
    BitMap_setBit(&(alloc->bitmap),parentIdx(buddy),0);
    BuddyAllocator_merge(alloc, parentIdx(buddy));
  }
}
void BuddyAllocator_update_parents(BuddyAllocator* alloc, int idx,int value){
  int bit_padre=BitMap_bit(&(alloc->bitmap),parentIdx(idx));
  if (idx<=0||bit_padre==value) return;
  BitMap_setBit(&(alloc->bitmap),parentIdx(idx),value);
  BuddyAllocator_update_parents(alloc,parentIdx(idx),value);
}
int BuddyAllocator_init(BuddyAllocator *alloc, int num_levels, char *buffer,int buffer_size, char *memory,int memory_size, int min_bucket_size) {

  if (!buffer){ 
    printf("\nbuffer per bitmap non corretto");
    return 0;
    }
  if (!memory){
    printf("\nmemoria non corretta");
    return 0;
  }
  
  if(memory_size<=BuddyAllocator_calcSize(num_levels)){
    printf("\ndimensione memoria non corretta");
    return 0;
  }



  //controlli vari
  int max_levels = floor(log2(memory_size / min_bucket_size));
  if(num_levels > max_levels){
    printf("\nnumero livelli non corretto");
    return 0;
  }
  
  int max_items =(1<<(num_levels+1))-1;
  int bitmap_size=(max_items+7)/8;
  if(buffer_size < bitmap_size){
    printf("\ndimensione buffer non corretta");
    return 0;
  }
if (min_bucket_size<16){
  printf("\nnumero di livelli troppo basso");
  return 0;
}
  if (log2(memory_size) != floor(log2(memory_size))){
        memory_size = min_bucket_size << num_levels;
    }
  alloc->num_levels = num_levels;
  alloc->memory = memory;
  alloc->min_bucket_size = min_bucket_size;
  alloc->memory_size=memory_size;
 
  
  
  for (int i=0;i<bitmap_size;i++){
    buffer[i]=0;
  }
  
  
  BitMap_init(&(alloc->bitmap),(bitmap_size*8),buffer);

  

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d -- list-items: %d", num_levels, max_items);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1 << num_levels) * min_bucket_size);

  return 1;
  
  
  
};



void BuddyAllocator_releaseBuddy(BuddyAllocator *alloc, int idx) {
  if (!alloc){ 
    printf("\n buddy allocator non valido");
    return;

  }
  if (BitMap_bit(&(alloc->bitmap),idx)==0){
    printf("\nrisorsa già deallocata\n");
    return;
  }
  BitMap_setBit(&(alloc->bitmap),idx,0);
  BuddyAllocator_update_childs(alloc,idx,0);
  BuddyAllocator_merge(alloc,idx);
}


void *BuddyAllocator_malloc(BuddyAllocator *alloc, int size) {
  
  if (!alloc||size<=0){
    printf("\nerrore nei parametri\n");
    return NULL;
  }
  

  int level=find_level(alloc,size);
  int i=firstIdx(level);
  int end=lastIdx(level);
  printf("il livello adatto per la dimensione è %d, il primo indice è %d e l'ultimo è %d\n",level,i,end);
  
  #ifdef _VERBOSE_
    printf("BuddyAllocator_malloc|requested: [ %d ] bytes -- level [ %d ] \n", size, level);
  #endif
  int found=0;
  printf("\n scorro la bitmap da %d a %d",i,end);
  while(i<end){
    if (BitMap_bit(&(alloc->bitmap),i)==0){
      printf("\ntrovato!il nostro indice ha valore %d\n",i);
      BitMap_setBit(&(alloc->bitmap),i,1);
      printf("\nverifichiamo di aver settato bene %d\n",BitMap_bit(&(alloc->bitmap),i));
      printf("\nchiamo il padre di %d che dovrebbe essere %d e tutti i padri sono %d\n",i,parentIdx(i),count_parents(i,alloc));
      BuddyAllocator_update_parents(alloc,i,1);
      printf("\nchiamo i figli di %d che dovrebbero essere %d e %d e in totale %d\n",i,2*i+1,2*i+2,count_childs(i,alloc));
      BuddyAllocator_update_childs(alloc,i,1);
      found=1;
      break;
    }
    i++;
  }

  if (found==0){
    ("\nnessun blocco libero trovato\n");
    return NULL;
  }
  int block_size=alloc->min_bucket_size<<(alloc->num_levels-level);
  char* ret=(alloc->memory+(i-startIdx(i))*block_size);
  printf("\nabbiamo allocato il blocco %desimo dei blocchi di grandezza %d bytes, che ha indice %d",i-startIdx(i),block_size,i);
  int* result=(int*)ret;
  *result=0;
  *(result+1)=i;
    printf("\n Stored bit %d at %p Stored index %d at %p and we return %p\n",*(result),(void*)result, *(result+1), (void*)(result+1),(void*)(result+2));

return (void*)(result + 2);

}


void BuddyAllocator_free(BuddyAllocator *alloc, void *mem) {
#ifdef _VERBOSE_
  printf("\nBuddyAllocator_free|destroying [ %p ]\n", mem);
#endif
  if (!mem) {
    printf("\nBuddyAllocator_free|WARINING, cannot free this memory address\n");
    return; 
  }
 
  int* tmp=(int*)mem;
  int idx=*(tmp+1);

  printf("\ninizio a deallocare partendo dall'indice %d   %p\n",idx,(tmp+1));
  BuddyAllocator_releaseBuddy(alloc,idx);
  
}
void BuddyAllocator_print(BuddyAllocator* buddy){
  printf("\n ecco lo stato del buddy\n");
  BitMap_print((&buddy->bitmap));
}

