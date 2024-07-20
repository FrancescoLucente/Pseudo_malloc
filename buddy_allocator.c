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
  if (idx=0) return 0;
  if (idx & 0x1) {
    return idx+1;
  }
  return idx-1;
}
int firstIdx(int level){
  return (1<<level)-1;
}
int lastIdx(int level){
  return (1<<level+1)-2;
}
int parentIdx(int idx) { return (idx-1)/2; }

int startIdx(int idx) { return firstIdx(levelIdx(idx)); }

int find_level(BuddyAllocator* alloc ,int size){
  int level=0;
  while (level<alloc->num_levels){
    if (size>alloc->memory_size>>level+1) return level;
    if (level==alloc->num_levels-1) return level;
    level++;
  }
}

// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int list_items = (1 << (num_levels))-1; // maximum number of allocations, used
                                          // to determine the max list items
  int size=list_items;
  return list_items;
}
void BuddyAllocator_update_childs(BuddyAllocator* alloc,int idx,int value){
  int size=BuddyAllocator_calcSize(alloc->num_levels);
  if (idx>=size||(2*idx)+1>size) return;
  BitMap_setBit(alloc->bitmap,2*idx+1,value);
  BitMap_setBit(alloc->bitmap,2*idx+2,value);
  BuddyAllocator_update_childs(alloc,2*idx+1,value);
  BuddyAllocator_update_childs(alloc,2*idx+2,value);
}
void BuddyAllocator_merge(BuddyAllocator* alloc, int idx){
  if (idx<=0) return;
  int buddy=buddyIdx(idx);
  if (BitMap_bit(alloc->bitmap,buddy)==0){
    BitMap_setBit(alloc->bitmap,parentIdx(buddy),0);
    BuddyAllocator_merge(alloc, parentIdx(buddy));
  }
}
// creates an item from the index
// and puts it in the corresponding list
/*
BuddyListItem *BuddyAllocator_createListItem(BuddyAllocator *alloc, int idx,
                                             BuddyListItem *parent_ptr) {
  BuddyListItem *item =
    (BuddyListItem *)PoolAllocator_getBlock(&alloc->list_allocator);
  item->idx = idx;
  item->level = levelIdx(idx);
  item->start = alloc->memory + ((idx - (1 << levelIdx(idx)))
                                 << (alloc->num_levels - item->level)) *
    alloc->min_bucket_size;
  item->size =
    (1 << (alloc->num_levels - item->level)) * alloc->min_bucket_size;
  item->parent_ptr = parent_ptr;
  item->buddy_ptr = 0;
  List_pushBack(&alloc->free[item->level], (ListItem *)item);
#ifdef _VERBOSE_
  printf("BuddyAllocator_createListItem|item [ idx:%d, level:%d, start:%p, size:%d ]\n",
         item->idx,
         item->level,
         item->start,
         item->size);
#endif
  return item;
};

// detaches and destroys an item in the free lists
void BuddyAllocator_destroyListItem(BuddyAllocator *alloc,
                                    BuddyListItem *item) {
  int level = item->level;
  List_detach(&alloc->free[level], (ListItem *)item);
#ifdef _VERBOSE_
  printf("BuddyAllocator_destroyListItem|item [ level:%d, idx:%d, start:%p, size:%d ]\n",
         item->level,
         item->idx,
         item->start,
         item->size);
#endif
  PoolAllocatorResult release_result =
      PoolAllocator_releaseBlock(&alloc->list_allocator, item);
  assert(release_result == Success);
};*/

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
 
  
  
 
  
  BitMap_init(alloc->bitmap,(bitmap_size*8),(uint8_t*)buffer);

  

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d -- list-items: %d", num_levels, max_items);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1 << num_levels) * min_bucket_size);

  
  
  
  
};


/*
// ----------------------------------------------------------------------- //
// ----------------------------------------------------------------------- //
//ia novel initialization function, just a single buffer also used by the pool allocator
void BuddyAllocator_initSingleBuffer(BuddyAllocator* b_allocator_,
                                     int max_num_levels_,
                                     uint64_t allocator_memory_size_,
                                     char* allocator_memory_) {
  assert(b_allocator_ && "BuddyAllocator_initSingleBuffer|ERROR, invalid allocator");
  assert(allocator_memory_size_ != 0 && allocator_memory_ &&
         "BuddyAllocator_initSingleBuffer|ERROR, invalid allocator memory");

  //ia total number of nodes in the tree
  const uint64_t total_nodes = pow(2, max_num_levels_ + 1) - 1;
  //ia compute the amount of memory required to store the
  //ia internal structures of the allocator (aka the pool allocator memory)
  const uint64_t internal_memory_required = ((sizeof(BuddyListItem) + sizeof(int))) * total_nodes;

  //ia compute the memory available to the user
  const uint64_t user_memory = (allocator_memory_size_ - internal_memory_required);
  //ia compute the minimum amount of memory that can be allocated with this levels
  const int64_t minimum_bucket_size = (user_memory >> (max_num_levels_));
  
  //ia given the number of levels and the minimum size of the buckets,
  //ia compute the amount of user memory required to work properly
  // const uint64_t maximum_bucket_number = (1 << (max_num_levels_ + 1)); 

  //ia total number of leafs (of size minimum_bucket_size)
  const uint64_t leaf_number = (user_memory / minimum_bucket_size);

  //ia compute memory required to work properly
  const uint64_t user_memory_required = minimum_bucket_size * leaf_number;
  const uint64_t total_memory_required = internal_memory_required + user_memory_required;

  //ia log something
#ifdef _VERBOSE_
  printf("BuddyAllocator_initSingleBuffer|current configuration\n");
  printf(" |- memory avalilable ....... [ %ld ]\n",allocator_memory_size_);
  printf(" |- tree levels ............. [ %02d ]\n",max_num_levels_);
  printf(" |- tree nodes .............. [ %ld ]\n",total_nodes);
  printf(" |- leaf number ............. [ %ld ]\n",leaf_number);
  printf(" |- minimum-bs .............. [ %ld ]\n",minimum_bucket_size);
  printf(" |- total internal memory ... [ %ld ]\n",internal_memory_required);
  printf(" |- total user memory ....... [ %ld ]\n",user_memory_required);
  printf(" |- total required memory ... [ %ld ]\n",total_memory_required);
  printf("BuddyAllocator_initSingleBuffer|---------------------\n");
#endif
  
  //ia if we do not have enough memory, throw an error
  if (total_memory_required > allocator_memory_size_) {
    printf("BuddyAllocator_initSingleBuffer|ERROR, invalid configuration\n");
    exit(EXIT_FAILURE);
  }

  //ia good we have enough room for this allocator, let's do the magic
  printf("BuddyAllocator_initSingleBuffer|configuration valid, constructing allocator\n");

  //ia keep track of the initial address of the memory
  // const char* memory_start_addr = &allocator_memory_[0]; 

  //ia give memory to the slab allocator
  char *list_start = allocator_memory_;
  PoolAllocatorResult init_result =
      PoolAllocator_init(&b_allocator_->list_allocator, sizeof(BuddyListItem),
                         total_nodes, list_start, internal_memory_required);
  printf("BuddyAllocator_initSingleBuffer|pool allocator result [ %s ]\n",
         PoolAllocator_strerror(init_result));

  // we initialize all lists
  for (int i = 0; i < MAX_LEVELS; ++i) {
    List_init(&b_allocator_->free[i]);
  }

  //ia we populate the fields of the buddy allocator
  b_allocator_->num_levels = max_num_levels_;
  b_allocator_->min_bucket_size = minimum_bucket_size;
  b_allocator_->memory = &allocator_memory_[0] + internal_memory_required;

  // we allocate a list_item to mark that there is one "materialized" list
  // in the first block
  BuddyAllocator_createListItem(b_allocator_, 1, 0);
  
 }

/*
int BuddyAllocator_getBuddy(BuddyAllocator *alloc, int level) {
  if (level < 0)
    return 0;
  assert(level <= alloc->num_levels);

  if (!alloc->free[level].size) { // no buddies on this level
    BuddyListItem *parent_ptr = BuddyAllocator_getBuddy(alloc, level - 1);
    if (!parent_ptr)
      return 0;

    // parent already detached from free list
    int left_idx = parent_ptr->idx << 1;
    int right_idx = left_idx + 1;
#ifdef _VERBOSE_
    printf("BuddyAllocator_getBuddy|split l:%d, left_idx: %d, right_idx: %d\n",
           level, left_idx, right_idx);
#endif
    BuddyListItem *left_ptr =
        BuddyAllocator_createListItem(alloc, left_idx, parent_ptr);
    BuddyListItem *right_ptr =
        BuddyAllocator_createListItem(alloc, right_idx, parent_ptr);
    // we need to update the buddy ptrs
    left_ptr->buddy_ptr = right_ptr;
    right_ptr->buddy_ptr = left_ptr;
  }
  // we detach the first
  if (alloc->free[level].size) {
    BuddyListItem *item = (BuddyListItem *)List_popFront(alloc->free + level);
    return item;
  }
  assert(0);
  return 0;
}*/

void BuddyAllocator_releaseBuddy(BuddyAllocator *alloc, int idx) {
  if (BitMap_bit(alloc->bitmap,idx)==0){
    printf("risorsa già deallocata");
    return;
  }
  BitMap_setBit(alloc->bitmap,idx,0);
  BuddyAllocator_update_childs(alloc,idx,0);
  BuddyAllocator_merge(alloc,idx);
}

// allocates memory
void *BuddyAllocator_malloc(BuddyAllocator *alloc, int size) {
  // we determine the level of the page
  if (!alloc||size<=0){
    printf("errore nei parametri");
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
  while(i<=end){
    if (BitMap_bit(alloc->bitmap,i)==0){
      BitMap_setBit(alloc->bitmap,i,1);
      BuddyAllocator_update_childs(alloc->bitmap,i,1);
      found=1;
      break;
    }
    i++;
  }
  if (!found){
    ("nessun blocco libero trovato");
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
  printf("BuddyAllocator_free|destroying [ %p ]\n", mem);
#endif
  if (!mem) {
    printf("BuddyAllocator_free|WARINING, cannot free this memory address\n");
    return; 
  }
  // we retrieve the buddy from the system
  int idx=*(((int*)mem)-1);
  BuddyAllocator_releaseBuddy(alloc,idx);
  
}

