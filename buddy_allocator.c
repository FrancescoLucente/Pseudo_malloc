#include <assert.h>
#include <math.h> // for floor and log2
#include <stdio.h>
#include <stdlib.h>

#include "buddy_allocator.h"
#include "bit_map.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation

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
  while (level<alloc->num_levels){
    if (size>alloc->memory_size>>(level+1)) return level;
    if (level==alloc->num_levels-1) return level;
    level++;
  }
  return ret;
}

// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int list_items = (1 << (num_levels))-1; // maximum number of allocations, used
                                          // to determine the max list items
  int size=list_items;
  return list_items;
}
void BuddyAllocator_update_childs(BuddyAllocator* alloc,int idx,int value){
  
  if (idx>=alloc->bitmap.buffer_size||(2*idx)+1>alloc->bitmap.buffer_size) return;
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
  if (idx<0||BitMap_bit(&(alloc->bitmap),parentIdx(idx))==1) return;
  BitMap_setBit(&(alloc->bitmap),parentIdx(idx),value);
}
void BuddyAllocator_init(BuddyAllocator *alloc, int num_levels, char *buffer,int buffer_size, char *memory,int memory_size, int min_bucket_size) {

  if (!buffer){ 
    printf("buffer per bitmap non corretto");
    return;
    }
  if (!memory){
    printf("memoria non corretta");
    return;
  }
  //controlli vari
  int max_levels = floor(log2(memory_size / min_bucket_size));
  assert(num_levels <= max_levels);
  int max_items =(1<<(num_levels))-1;
  int bitmap_size=(max_items+7)/8;
  assert(buffer_size >= bitmap_size);
  assert(memory_size>=BuddyAllocator_calcSize(num_levels));




  
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

  
  
  
  
};



void BuddyAllocator_releaseBuddy(BuddyAllocator *alloc, int idx) {
  if (BitMap_bit(&(alloc->bitmap),idx)==0){
    printf("\nrisorsa già deallocata\n");
    return;
  }
  BitMap_setBit(&(alloc->bitmap),idx,0);
  BuddyAllocator_update_childs(alloc,idx,0);
  BuddyAllocator_merge(alloc,idx);
}

// allocates memory
void *BuddyAllocator_malloc(BuddyAllocator *alloc, int size) {
  // we determine the level of the page
  if (!alloc||size<=0){
    printf("\nerrore nei parametri\n");
    return NULL;
  }
  int total_size=size+8;

  int level=find_level(alloc,total_size);
  int i=firstIdx(level);
  int end=lastIdx(level);
  
  #ifdef _VERBOSE_
    printf("BuddyAllocator_malloc|requested: [ %d ] bytes -- level [ %d ] \n", size, level);
  #endif
  int found=0;
  printf("\n scorro la bitmap da %d a %d",i,end);
  while(i<end){
    if (BitMap_bit(&(alloc->bitmap),i)==0){
      BitMap_setBit(&(alloc->bitmap),i,1);
      printf("\nchiamo il padre\n");
      BuddyAllocator_update_parents(alloc,i,1);
      printf("\nchiamo il figlio\n");
      BuddyAllocator_update_childs(alloc,i,1);
      found=1;
      break;
    }
    i++;
  }

  if (!found){
    ("\nnessun blocco libero trovato\n");
    return NULL;
  }
  int block_size=alloc->min_bucket_size<<(alloc->num_levels-level);
  char* ret=(alloc->memory+(i-startIdx(i))*block_size);
  *((int*)ret)=0;
  *(((int*)ret)+1)=i;
  return (void*)(ret+8);
}
// releases allocated memory

void BuddyAllocator_free(BuddyAllocator *alloc, void *mem) {
#ifdef _VERBOSE_
  printf("\nBuddyAllocator_free|destroying [ %p ]\n", mem);
#endif
  if (!mem) {
    printf("\nBuddyAllocator_free|WARINING, cannot free this memory address\n");
    return; 
  }
  // we retrieve the buddy from the system
  int idx=*(((int*)mem)-1);
  BuddyAllocator_releaseBuddy(alloc,idx);
  
}

